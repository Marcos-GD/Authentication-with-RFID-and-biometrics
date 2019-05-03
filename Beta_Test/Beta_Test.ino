#include <SoftwareSerial.h>
SoftwareSerial BTSerial(4,5); // Serial Bluetooth
SoftwareSerial FP_Serial(2, 3); // Serial Leitor Biometrico

#include "FingerPrint.h"

#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN1 8
#define SS_PIN2 9
#define RST_PIN 10
#include "RFIDSensor.h"

#define DOORPIN   7
#define RELAYPIN  6

bool Relay  = false;
bool Door   = false; 

int test_FP = 0;

enum ListaComandos {
//padrão
  NADA_ENCONTRADO,  

//comandos BTSerial
  ARMAZENAR_FP,     
  NUM_TEMPLATES,
  DEL_ID_FP,
  LIGAR_RELAY,
  DESLIGAR_RELAY,
  OPENDOOR,
  
//Verificadores
  RECONHECER_FP,    
  RECONHECER_RFID
  };

ListaComandos comando;



void setup()
{ 
  Serial.begin(9600);
  BTSerial.begin(9600);
  SPI.begin();
  BTSerial.println(F("\n\nTeste Beta"));

  // Testa comunicação com FingerPrint
  initFingerPrint();

  // Inicia o leitor RFID
  RFID_Relay.PCD_Init(SS_PIN1, RST_PIN);
  RFID_Porta.PCD_Init(SS_PIN2, RST_PIN);

  pinMode(RELAYPIN, OUTPUT);
  pinMode(DOORPIN, OUTPUT);
  digitalWrite(RELAYPIN, LOW);
  digitalWrite(DOORPIN, LOW);
}

void loop()
{ 
  //testa RFID para ativação do Relay primeiro
  if ( Relay != true && RFID_Relay.PICC_IsNewCardPresent() && RFID_Relay.PICC_ReadCardSerial() )
  {
    Relay = compararRFID(RFID_Relay);
  }
  else if (!RFID_Relay.PICC_IsNewCardPresent() || !RFID_Relay.PICC_ReadCardSerial()) Relay = false;

  // Testa nova presença nos sensores
  comando = test_new_entity();
  switch (comando) {
    case NADA_ENCONTRADO:
      return;
      break;
    case ARMAZENAR_FP:
      fingerprintEnroll();
      break;
    case NUM_TEMPLATES:
      finger.getTemplateCount();
      BTSerial.print(finger.templateCount); BTSerial.println(F(" templates encontrados no sensor biometrico."));
      break;
    case DEL_ID_FP:
      deleteFingerprint();
      break;
    case RECONHECER_FP:
      Door = getFingerprintIDez();         
      break;
    case RECONHECER_RFID:
      if( compararRFID(RFID_Porta) ) Door = true;
      break;
    case LIGAR_RELAY:
      digitalWrite(RELAYPIN, HIGH);
      BTSerial.println(F("Energia LIGADA!"));
      Relay = true;
      break;
    case DESLIGAR_RELAY:
      digitalWrite(RELAYPIN, LOW);
      BTSerial.println(F("Energia DESLIGADA!"));
      Relay = false;
      break;
    case OPENDOOR:
      Door = true;
      break;
    default:
      BTSerial.println(F("Erro! comando não encontrado!"));
      return;
  }
  if(Door == true)
  {
    digitalWrite(DOORPIN, HIGH);
    delay(2000);
    digitalWrite(DOORPIN, LOW);
    Door = false;
  }
}

uint8_t test_new_entity(){
  test_FP = finger.getImage();
  if (  !BTSerial.available() && 
        test_FP == FINGERPRINT_NOFINGER && 
        ( !RFID_Porta.PICC_IsNewCardPresent() || !RFID_Porta.PICC_ReadCardSerial() ))
    return NADA_ENCONTRADO;
  
  else if ( test_FP == FINGERPRINT_PACKETRECIEVEERR )
  {
    BTSerial.println(F("Erro de comunicação com o FingerPrinter!"));
    return NADA_ENCONTRADO;
  }
  
  else if ( BTSerial.available() )
  {
    BTSerial.println(F("\nComandos:\n1 - Armazenar nova digital\n2 - Numero de templates armazenados\n3 - Deletar uma ID\n4 - Ligar energia\n5 - Desligar energia\n6 - Abrir Porta"));
    uint8_t comm = 0;
    while (! BTSerial.available());
    comm = BTSerial.parseInt();
    return comm;
  }
  
  else if ( test_FP == FINGERPRINT_OK ) return RECONHECER_FP;

  else return RECONHECER_RFID;
}