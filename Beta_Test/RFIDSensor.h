/*
 ** Typical pin layout used:
 * -------------------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino     ESP32
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro   WROOM32
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin         Pin
 * -------------------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST         22
 * SPI SS      SDA(SS)      10            53        D10        10               10          21
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16          23
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14          19
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15          18
 *==========================================================================================
 * Conexões usadas durante estes testes (ARDUINO UNO):
 * SDA    10
 * RST    09
 * SCK    13
 * MOSI   11
 * MISO   12
 * 3.3V   3.3V
 * GND    GND
 */
#include <SPI.h>
#include <MFRC522.h>


String IDtag = ""; //Variável que armazenará o ID da Tag
bool escape = false; // variável responsavel por sair do laço de espera pelo cartão

String TagsCadastradas[] = {
  "B83F572",
  "7DBBB2E"  //Cartao Teste
};

MFRC522 LeitorRFID(SS_PIN, RST_PIN);    // Cria uma nova instância para o leitor e passa os pinos como parâmetro

bool compararRFID(){
  IDtag = ""; //Inicialmente IDtag deve estar vazia.
  // Pega o ID da Tag através da função LeitorRFID.uid e Armazena o ID na variável IDtag
  for (byte i = 0; i < LeitorRFID.uid.size; i++) {
    IDtag.concat(String(LeitorRFID.uid.uidByte[i], HEX));
  }
  IDtag.toUpperCase();
  BTSerial.print("ID: "); BTSerial.println(IDtag);
  //Compara o valor do ID lido com os IDs armazenados no vetor TagsCadastradas[]
  for (int i = 0; i < (sizeof(TagsCadastradas) / sizeof(String)); i++) {
    if (  IDtag.equalsIgnoreCase(TagsCadastradas[i])  ) {
      BTSerial.println("ID cadastrada");
      delay(2000);
      return true;
    }
  }
  //else
  BTSerial.println("ID desconhecida!");
  delay(2000); //aguarda 2 segundos para efetuar uma nova leitura
  return false;
}

