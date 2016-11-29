/*
By:David Teles
IST 2016
NEEC
---------------------------------------------------------------Controlo de acesso RFID e Keypad------------------------------------------------
  Este programa consiste na leitura do numero de identificaçao unico de cada cartao e o codigo a si associado pela porta uart do arduino.
No display encontra-se o nome do local ate que seja detectado um cartao, nesse momento sera pedido um codigo que devera ser introduzido no keypad.
Toda a informaçao NAO E PROCESSADA no arduino, mas sim num modulo adicional (Ex:RPI) que le a informaçao da porta serial e compara com uma base de dados.

Projecto desenvolvido para o NEEC (Nucleo de Estudantes de Engenharia Electrotecnica e Computadores)
*/
/*
 * Typical pin layout used:
 * ------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino
 *             Reader/PCD   Uno           Mega      Nano v3
 * Signal      Pin          Pin           Pin       Pin
 * ------------------------------------------------------------
 * RST/Reset   RST          9             5         D9
 * SPI SS      SDA(SS)      10            53        D10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11
 * SPI MISO    MISO         12 / ICSP-1   50        D12
 * SPI SCK     SCK          13 / ICSP-3   52        D13
 */
 
//RFID
#include <SPI.h>        //Comunicação com o leitor rfid
#include <MFRC522.h>    //Biblioteca para simplificar a descodificação dos cartões
#define Reset		9       //Reset
#define SlaveSelect	10  //Select do rfid
MFRC522 mfrc522(SlaveSelect, Reset);
int successRead;   
char sr;                   
byte readCard[4];           // ID do cartao
//Keypad
#include <Keypad.h>
char customKey;
const byte ROWS = 4;      //Dimensoes do keypad   
const byte COLS = 3;
char keys[ROWS][COLS] = { //Ler o keypad
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {8, 7, 6, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4, 3, 2}; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int WatingCardLed=A0, WaintingPinLed=A1;  //Indicadores Led
int Buzzer=A2;    //Buzzer 


void setup() {
  
 pinMode(WatingCardLed,OUTPUT);
 pinMode(WaintingPinLed,OUTPUT);
 pinMode(Buzzer, OUTPUT);
 digitalWrite(WatingCardLed,HIGH);
 digitalWrite(WaintingPinLed,HIGH);
 digitalWrite(Buzzer, LOW);
 
 //Protocologos de comunicaçao
 Serial.begin(9600);		// Iniciar porta de comunicaçao serial
 SPI.begin();			// Iniciar Comunicaçao SPI
 mfrc522.PCD_Init();            // Iniciar o leitor de cartoes
 mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);   //Activar a potencia maxima do leitor
 sr='a';
 delay(1000);
 digitalWrite(WatingCardLed,LOW);
 digitalWrite(WaintingPinLed,LOW);
 delay(1000);
 digitalWrite(WatingCardLed,HIGH);
 digitalWrite(WaintingPinLed,LOW);
}

void loop() {

 customKey = 'a'; //Para que o do while loop volte a funcionar
  
  do {

    successRead = getID(); // 1 quando houver no fim de ler um cartao
    getID;
  }
  while (!successRead); // O programa nao avança enquanto nenhum cartao for lido
   
  }
 
  




int getID() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //Se nao existir um novo cartao parar
  
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Ler o cartao apenas uma vez
    
    return 0;
  }
  digitalWrite(Buzzer, HIGH);
  delay(200);
  digitalWrite(Buzzer, LOW);
  //Escreve sempre ID antes do numero unico do cartão
  Serial.print("ID");
  for (int i = 0; i < 4; i++) {
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  //Enter na linha de comandos serial
  Serial.println("");

  //Esperar que o servidor/computador confirme que o cartão está na base de dados enviando um # 
  while(sr!='#'){
    digitalWrite(WatingCardLed,LOW);
    digitalWrite(WaintingPinLed,LOW);
    sr=Serial.read();

    //Ler o keypad
    customKey = customKeypad.getKey();

    //Se o computador enviar um * ou o utilizador carregar no * do keypad volta a ler cartões
    if(sr=='*'||customKey=='*'){
      //Terminar os processos sempre com *
      digitalWrite(WatingCardLed,HIGH);
      digitalWrite(WaintingPinLed,LOW);
      digitalWrite(Buzzer, HIGH);
      delay(100);
      digitalWrite(Buzzer, LOW);
      delay(100);
      digitalWrite(Buzzer, HIGH);
      delay(100);
      digitalWrite(Buzzer, LOW);
      delay(100);
      digitalWrite(Buzzer, HIGH);
      delay(100);
      digitalWrite(Buzzer, LOW);
      if(customKey=='*'){
         delay(100);
         digitalWrite(Buzzer, HIGH);
         delay(200);
         digitalWrite(Buzzer, LOW);
         Serial.println('*');
      }
      return 0;
    }
  
 }
  
 sr='a';
 Serial.print('#');
 do {
  digitalWrite(WatingCardLed,LOW);
  digitalWrite(WaintingPinLed,HIGH);  
  customKey = customKeypad.getKey();
  
  if (customKey){
    digitalWrite(Buzzer, HIGH);
    delay(100);
    digitalWrite(Buzzer, LOW);
    Serial.print(customKey);
  }
  
  if(customKey=='*'){
     break;
  } 
    
 } while ( customKey!= '#');
 digitalWrite(Buzzer, HIGH);
 delay(200);
 digitalWrite(Buzzer, LOW);
 digitalWrite(WatingCardLed,HIGH);
 digitalWrite(WaintingPinLed,LOW);
 Serial.println("");

 mfrc522.PICC_HaltA(); // Parar de ler
}


 
