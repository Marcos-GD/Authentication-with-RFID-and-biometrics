//Mudar os pinos diretamente nas bibliotecas (temporario)
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(4,5); // Serial Bluetooth
SoftwareSerial FP_Serial(2, 3); // Serial Leitor Biometrico

#include "FingerPrint.h"

#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
#include "RFIDSensor.h"

int test_FP = 0;

enum ListaComandos {
  NADA_ENCONTRADO,
  ARMAZENAR_FP,
  NUM_TEMPLATES,
  DEL_ID_FP,
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
  LeitorRFID.PCD_Init();

}

void loop()
{ 
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
      getFingerprintIDez();         
      break;
    case RECONHECER_RFID:
      compararRFID();
      break;
    default:
      BTSerial.println(F("Erro! comando não encontrado!"));
      return;
  }
}

uint8_t test_new_entity(){
  test_FP = finger.getImage();
  while ( !BTSerial.available() && test_FP == FINGERPRINT_NOFINGER && ( !LeitorRFID.PICC_IsNewCardPresent() || !LeitorRFID.PICC_ReadCardSerial() ) ) return NADA_ENCONTRADO;
  if ( test_FP == FINGERPRINT_PACKETRECIEVEERR ) { BTSerial.println(F("Erro de comunicação com o FingerPrinter!")); return NADA_ENCONTRADO; }
  else if ( BTSerial.available() ){
    BTSerial.println(F("\nComandos:\n1 - Armazenar nova digital\n2 - Numero de templates armazenados\n3 - Deletar uma ID"));
    uint8_t comm = 0;
    while (! BTSerial.available());
    comm = BTSerial.parseInt();
    return comm;
    }
  else if ( test_FP == FINGERPRINT_OK ) return RECONHECER_FP;
  else return RECONHECER_RFID;
}
