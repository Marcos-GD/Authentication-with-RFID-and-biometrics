// =============== FingerPrint - Definições ============================
// Para utilizar o hardware serial:
// use #0 conectado com o tx do sensor,
// use #1 conectado com o rx do sensor.
// Descomente esta linha:
// #define FP_Serial Serial1
// ---------------------------------------------------------------------
// Para utilizar o software serial:
// pin #2 conectado com o tx do sensor
// pin #3 conectado com o rx do sensor
// comente essas duas linhas se utilizar o hardware serial
#include <SoftwareSerial.h>


#include <Adafruit_Fingerprint.h>
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&FP_Serial);

uint8_t id;

uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! BTSerial.available());
    num = BTSerial.parseInt();
  }
  return num;
}

void initFingerPrint()
{
    finger.begin(57600); // data rate do sensor biométrico
    // teste de comunicação do sensor biometrico
    if (finger.verifyPassword())  BTSerial.println(F("Sensor biometrico encontrado!"));
    else                          BTSerial.println(F("Sem conexao com o sensor biometrico!\nVerifique a conexao e reinicie para utiliza-lo!"));
}

bool getFingerprintIDez() {
  uint8_t p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return false;

  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_NOTFOUND){
    BTSerial.println(F("Digital não reconhecida!"));
    return false;
  }
  else if( p == FINGERPRINT_PACKETRECIEVEERR ) {
    BTSerial.println(F("Communication error"));
    return false;
  }
  else {
  // found a match!
  BTSerial.print(F("Encontrado ID #")); BTSerial.print(finger.fingerID); 
  BTSerial.print(F(" com confiança de ")); BTSerial.println(finger.confidence);
  return true; 
  }
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  BTSerial.print(F("Aguardando digital para armazenamento como #")); BTSerial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      BTSerial.println(F("Imagem tirada"));
      break;
    case FINGERPRINT_NOFINGER:
      BTSerial.println(F("."));
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      BTSerial.println(F("Communication error"));
      break;
    case FINGERPRINT_IMAGEFAIL:
      BTSerial.println(F("Imaging error"));
      break;
    default:
      BTSerial.println(F("Unknown error"));
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      BTSerial.println(F("Imagem convertida"));
      break;
    case FINGERPRINT_IMAGEMESS:
      BTSerial.println(F("Image too messy"));
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      BTSerial.println(F("Communication error"));
      return p;
    case FINGERPRINT_FEATUREFAIL:
      BTSerial.println(F("Could not find fingerprint features"));
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      BTSerial.println(F("Could not find fingerprint features"));
      return p;
    default:
      BTSerial.println(F("Unknown error"));
      return p;
  }
  
  BTSerial.println(F("Remova o dedo"));
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  BTSerial.print(F("ID ")); Serial.println(id);
  p = -1;
  BTSerial.println(F("Recoloque a mesma digital"));
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      BTSerial.println(F("Imagem tirada"));
      break;
    case FINGERPRINT_NOFINGER:
      BTSerial.print(F("."));
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      BTSerial.println(F("Communication error"));
      break;
    case FINGERPRINT_IMAGEFAIL:
      BTSerial.println(F("Imaging error"));
      break;
    default:
      BTSerial.println(F("Unknown error"));
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      BTSerial.println(F("Imagem convertida"));
      break;
    case FINGERPRINT_IMAGEMESS:
      BTSerial.println(F("Image too messy"));
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      BTSerial.println(F("Communication error"));
      return p;
    case FINGERPRINT_FEATUREFAIL:
      BTSerial.println(F("Could not find fingerprint features"));
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      BTSerial.println(F("Could not find fingerprint features"));
      return p;
    default:
      BTSerial.println(F("ERROR!"));
      return p;
  }
  
  // OK converted!
  BTSerial.print(F("Criando modelo para #"));  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    BTSerial.println(F("Digitais coincidem!"));
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    BTSerial.println(F("Digitais não coincidem"));
    return p;
  }   
  
  BTSerial.print(F("ID ")); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    BTSerial.println(F("Armazenado!"));
    return 1;
  } else {
    BTSerial.println(F("ERROR!"));
    return p;
  }
}

void fingerprintEnroll(){
  BTSerial.println(F("Por favor entre com um ID # (de 1 a 127) no qual voce quer salvar a digital..."));
  id = readnumber();
  if ( (id == 0) || (id > 127) ) {// ID #0 not allowed, try again!
     BTSerial.println(F("ID não permitido!"));
     return;
  }
  BTSerial.print(F("Armazenando ID #"));
  BTSerial.println(id);
  
  while (!  getFingerprintEnroll() );
}

void deleteFingerprint(){
    BTSerial.println(F("Entre com o id da digital a ser deletada:"));
    int p = finger.deleteModel(id);   
    if ( p == FINGERPRINT_OK ) {
        BTSerial.println(F("ID #"));
        BTSerial.println(id);
        BTSerial.println(F(" deletado com sucesso!"));
        return;
    }
    else BTSerial.println(F("Error!"));
    return;
}
