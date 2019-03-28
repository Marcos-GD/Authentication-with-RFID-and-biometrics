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
    if (finger.verifyPassword())  BTSerial.println("Sensor biometrico encontrado!");
    else                          BTSerial.println("Sem conexao com o sensor biometrico!\nVerifique a conexao e reinicie para utiliza-lo!");
}

void getFingerprintIDez() {
  uint8_t p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return;

  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_NOTFOUND){
    BTSerial.println("Digital não reconhecida!");
    return;
  }
  else if( p == FINGERPRINT_PACKETRECIEVEERR ) {
    BTSerial.println("Communication error");
    return;
  }
  else {
  // found a match!
  BTSerial.print("Encontrado ID #"); BTSerial.print(finger.fingerID); 
  BTSerial.print(" com confiança de "); BTSerial.println(finger.confidence);
  return; 
  }
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  BTSerial.print("Aguardando digital para armazenamento como #"); BTSerial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      BTSerial.println("Imagem tirada");
      break;
    case FINGERPRINT_NOFINGER:
      BTSerial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      BTSerial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      BTSerial.println("Imaging error");
      break;
    default:
      BTSerial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      BTSerial.println("Imagem convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      BTSerial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      BTSerial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      BTSerial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      BTSerial.println("Could not find fingerprint features");
      return p;
    default:
      BTSerial.println("Unknown error");
      return p;
  }
  
  BTSerial.println("Remova o dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  BTSerial.print("ID "); Serial.println(id);
  p = -1;
  BTSerial.println("Recoloque a mesma digital");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      BTSerial.println("Imagem tirada");
      break;
    case FINGERPRINT_NOFINGER:
      BTSerial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      BTSerial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      BTSerial.println("Imaging error");
      break;
    default:
      BTSerial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      BTSerial.println("Imagem convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      BTSerial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      BTSerial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      BTSerial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      BTSerial.println("Could not find fingerprint features");
      return p;
    default:
      BTSerial.println("ERROR!");
      return p;
  }
  
  // OK converted!
  BTSerial.print("Criando modelo para #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    BTSerial.println("Digitais coincidem!");
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    BTSerial.println("Digitais não coincidem");
    return p;
  }   
  
  BTSerial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    BTSerial.println("Armazenado!");
    return 1;
  } else {
    BTSerial.println("ERROR!");
    return p;
  }
}

void fingerprintEnroll(){
  BTSerial.println("Por favor entre com um ID # (de 1 a 127) no qual voce quer salvar a digital...");
  id = readnumber();
  if ( (id == 0) || (id > 127) ) {// ID #0 not allowed, try again!
     BTSerial.println("ID não permitido!");
     return;
  }
  BTSerial.print("Armazenando ID #");
  BTSerial.println(id);
  
  while (!  getFingerprintEnroll() );
}

void deleteFingerprint(){
    BTSerial.println("Entre com o id da digital a ser deletada:");
    int p = finger.deleteModel(id);   
    if ( p == FINGERPRINT_OK ) {
        BTSerial.println("ID #");
        BTSerial.println(id);
        BTSerial.println(" deletado com sucesso!");
        return;
    }
    else BTSerial.println("Error!");
    return;
}
