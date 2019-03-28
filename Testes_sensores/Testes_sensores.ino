//Mudar os pinos diretamente nas bibliotecas (temporario)
#include <SoftwareSerial.h>
#include "FingerPrint.h"

#include <SPI.h>
#include <MFRC522.h>
#include "RFIDSensor.h"

enum ListaComandos {
  ARMAZENAR_FP = 1,
  RECONHECER_FP,
  NUM_TEMPLATES,
  DEL_ID_FP,
  RECONHECER_RFID
  };

ListaComandos comando;

uint8_t lerComando() {
  uint8_t num = 0;
   
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void setup()
{ 
  Serial.begin(9600);
  SPI.begin();
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nTeste Sensores - Geral");

  // Testa comunicação com FingerPrint
  initFingerPrint();

  // Inicia os leitores RFID
  LeitorRFID[0].PCD_Init(SS_PIN1, RST_PIN);
  LeitorRFID[1].PCD_Init(SS_PIN2, RST_PIN);

}

void loop()
{ 
  Serial.println(F("Comandos:\n1 - Armazenar nova digital\n2 - Reconhecimento de digital\n3 - Numero de templates armazenados\n4 - Deletar uma ID\n5 - Ler RFID"));
  Serial.println(F("Aguardando comando: "));
  
  comando = lerComando();
  
  switch (comando) {
    case ARMAZENAR_FP:
      fingerprintEnroll();
      break;
    
    case RECONHECER_FP:
      Serial.println(F("Aguardando digital para verificação"));
      while (! getFingerprintIDez() );
      delay(50);           
      break;
    
    case NUM_TEMPLATES:
      finger.getTemplateCount();
      Serial.print(finger.templateCount); Serial.println(F(" templates encontrados no sensor biometrico."));
      break;
    
    case DEL_ID_FP:
      deleteFingerprint();
      break;
    
    case RECONHECER_RFID:
      LerRFID();
      CompararRFID();
      break;
    
    default:
      Serial.println(F("Entre com um comando valido! \n\n"));
  }
}
