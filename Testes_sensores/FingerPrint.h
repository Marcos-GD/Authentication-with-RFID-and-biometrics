// =============== FingerPrint - Definições ============================
// Para utilizar o hardware serial:
// use #0 conectado com o tx do sensor,
// use #1 conectado com o rx do sensor.     
// Descomente esta linha:
// #define meuSerial Serial1
// ---------------------------------------------------------------------
// Para utilizar o software serial:
// pin #2 conectado com o tx do sensor
// pin #3 conectado com o rx do sensor
// comente essas duas linhas se utilizar o hardware serial
#include <SoftwareSerial.h>
SoftwareSerial meuSerial(2, 3);

#include <Adafruit_Fingerprint.h>
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&meuSerial);

uint8_t id;

uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void initFingerPrint()
{
    finger.begin(57600); // data rate do sensor biométrico
    // teste de comunicação do sensor biometrico
    if (finger.verifyPassword()) {

        Serial.println("Sensor biometrico encontrado!");
    } else {
        Serial.println("Sem conexao com o sensor biometrico!\nVerifique a conexao e reinicie.");
      //  while (1) { delay(1); }  
  }
}

bool getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return false;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return false;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return false;
  
  // found a match!
  Serial.print("Encontrado ID #"); Serial.print(finger.fingerID); 
  Serial.print(" com confiança de "); Serial.println(finger.confidence);
  return true; 
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Aguardando digital para armazenamento como #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagem tirada");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagem convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remova o dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Recoloque a mesma digital");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagem tirada");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagem convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("ERROR!");
      return p;
  }
  
  // OK converted!
  Serial.print("Criando modelo para #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Digitais coincidem!");
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Digitais não coincidem");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Armazenado!");
    return 1;
  } else {
    Serial.println("ERROR!");
    return p;
  }
}

void fingerprintEnroll(){
  Serial.println("Por favor entre com um ID # (de 1 a 127) no qual voce quer salvar a digital...");
  id = readnumber();
  if ( (id == 0) || (id > 127) ) {// ID #0 not allowed, try again!
     Serial.println("ID não permitido!");
     return;
  }
  Serial.print("Armazenando ID #");
  Serial.println(id);
  
  while (!  getFingerprintEnroll() );
}

void deleteFingerprint(){
    Serial.println("Entre com o id da digital a ser deletada:");
    int p = finger.deleteModel(id);   
    if ( p == FINGERPRINT_OK ) {
        Serial.println("ID #");
        Serial.println(id);
        Serial.println(" deletado com sucesso!");
        return;
    }
    else Serial.println("Error!");
    return;
}
