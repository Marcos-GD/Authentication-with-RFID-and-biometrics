/*
 ** Typical pin layout used:
 * -------------------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino     ESP32
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro   WROOM32
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin         Pin
 * -------------------------------------------------------------------------------------------------
 * RST/Reset   RST          09            05        D9         RESET/ICSP-5     RST         22
 * SPI SS      SDA(SS)      10 / 08       53        D10        10               10          21
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16          23
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14          19
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15          18
 *==========================================================================================
 * Conexões usadas durante estes testes (ARDUINO UNO):
 * SDA    10 / 08
 * RST    09
 * SCK    13
 * MOSI   11
 * MISO   12
 * 3.3V   3.3V
 * GND    GND
 */
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN1 10
#define SS_PIN2 8

#define RST_PIN 9

String IDtag = ""; //Variável que armazenará o ID da Tag
int leitorEmUso = -1;

String TagsCadastradas[] = {
  "B83F572",
  "7DBBB2E"  //Cartao Teste
};

//MFRC522 LeitorRFID(SS_PIN, RST_PIN);    // Cria uma nova instância para o leitor e passa os pinos como parâmetro
MFRC522 LeitorRFID[2];

/*    Como deve ser utilizado em codigo (ARDUINO)
 * MFRC522 LeitorRFID;    
 * void setup() {
 *    Serial.begin(9600);                     // Inicializa a comunicação Serial
 *    SPI.begin();                            // Inicializa comunicacao SPI
 *    LeitorRFID.PCD_Init(SS_PIN, RST_PIN);   // Inicializa o leitor RFID
 * }
 * 
 * void loop(){
 *    LerRFID();
 *    CompararRFID();
 * }
 * 
 */  
// ADICIONAR MODIFICAÇÂO DE MAIS SENSORES!!!!


String LerRFID() {
  IDtag = ""; //Inicialmente IDtag deve estar vazia.
  Serial.println("Aproxime a TAG");
  leitorEmUso = -1;
  
  // Verifica se existe uma Tag presente
  while ( leitorEmUso == -1) {
    Serial.println(".");
    delay(100);
    
    if      ( LeitorRFID[0].PICC_IsNewCardPresent() && LeitorRFID[0].PICC_ReadCardSerial() ) leitorEmUso = 0;
    else if ( LeitorRFID[1].PICC_IsNewCardPresent() && LeitorRFID[1].PICC_ReadCardSerial() ) leitorEmUso = 1;
   // else if ( Serial.available() ) return "";
  }
  
  // Pega o ID da Tag através da função LeitorRFID.uid e Armazena o ID na variável IDtag
  for (byte i = 0; i < LeitorRFID[leitorEmUso].uid.size; i++) {
    IDtag.concat(String(LeitorRFID[leitorEmUso].uid.uidByte[i], HEX));
  }
  IDtag.toUpperCase();
  Serial.print("ID: "); Serial.println(IDtag);
  return IDtag;
}

void CompararRFID(){
  //Compara o valor do ID lido com os IDs armazenados no vetor TagsCadastradas[]
  for (int i = 0; i < (sizeof(TagsCadastradas) / sizeof(String)); i++) {
    if (  IDtag.equalsIgnoreCase(TagsCadastradas[i])  ) {
      Serial.println("ID cadastrada");
      Serial.print("LeitorRFID "); Serial.println( leitorEmUso + 1 );
      delay(2000);
      return;
    }
  }
  if ( IDtag == "" ) Serial.println("Cancelando reconhecimento!");
  else Serial.println("ID desconhecida!");
  delay(2000); //aguarda 2 segundos para efetuar uma nova leitura
}

