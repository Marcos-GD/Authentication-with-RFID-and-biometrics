#include <SPI.h>
    #include <MFRC522.h>

    #define RST_PIN         9          // Configurable, see typical pin layout above
    #define SS_1_PIN        10         // Configurable, see typical pin layout above
    #define SS_2_PIN        2         // Configurable, see typical pin layout above

    //#define SS_3_PIN        10

    #define LedPin          8
    #define NR_OF_READERS  2

    byte ssPins[] = {SS_1_PIN, SS_2_PIN};
    int a=0;
    int b=0;

    MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.

    /**
       * Initialize.
     */
    void setup() {

    Serial.begin(115200); // Initialize serial communications with the PC
    //while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    pinMode(LedPin,OUTPUT);
    SPI.begin();        // Init SPI bus

    for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
    }
    }

    /**
    * Main loop.
       */
       void loop() {
       for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
       // Look for new cards
    
    
    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      Serial.print(F("Reader: "));
      Serial.print(reader);
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(" Card UID:"));
      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
      Serial.println();
     Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
      Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));
              if (mfrc522[reader].uid.uidByte[0] == 140 &
              mfrc522[reader].uid.uidByte[1] == 120 & 
              mfrc522[reader].uid.uidByte[2] == 34 & 
              mfrc522[reader].uid.uidByte[3] == 43 & 
              reader==0 ) {
              Serial.println(F("pirmā karte atrasta"));  
              a++; 
              }
                if (mfrc522[reader].uid.uidByte[0] != 140 &
              mfrc522[reader].uid.uidByte[1] != 120 & 
              mfrc522[reader].uid.uidByte[2] != 34 & 
              mfrc522[reader].uid.uidByte[3] != 43 & 
              reader==0 ) {
              Serial.println(F("pirmā karte nodzes"));  
              a=0; 
              }
              
             if (mfrc522[reader].uid.uidByte[0] == 5 &
              mfrc522[reader].uid.uidByte[1] == 233 & 
              mfrc522[reader].uid.uidByte[2] == 26 & 
              mfrc522[reader].uid.uidByte[3] == 43 & 
              reader==1 ) {
              Serial.println(F("otrā karte atrasta"));
              b++;   
              }
          if (mfrc522[reader].uid.uidByte[0] != 5 &
              mfrc522[reader].uid.uidByte[1] != 233 & 
              mfrc522[reader].uid.uidByte[2] != 26 & 
              mfrc522[reader].uid.uidByte[3] != 43 & 
              reader==1 ) {
              Serial.println(F("otrā karte nodzēs"));
              b=0;   
              }
              
              if (mfrc522[reader].uid.uidByte[0] == 140 &
              mfrc522[reader].uid.uidByte[1] == 120 & 
              mfrc522[reader].uid.uidByte[2] == 34 & 
              mfrc522[reader].uid.uidByte[3] == 43 & 
              reader==1 ) {
                Serial.println(F("otrā karte nodzēs"));
              b=0;   
              }
              
                       if (mfrc522[reader].uid.uidByte[0] == 5 &
              mfrc522[reader].uid.uidByte[1] == 233 & 
              mfrc522[reader].uid.uidByte[2] == 26 & 
              mfrc522[reader].uid.uidByte[3] == 43 & 
             reader==0 ) {
              Serial.println(F("pirmā karte nodzes"));  
              a=0; 
              }
    Serial.println();

      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
      } //if (mfrc522[reader].PICC_IsNewC
      } //for(uint8_t reader
      if (a>0 & b>0)
      {
      digitalWrite(LedPin,HIGH);
       delay(1000);
      digitalWrite(LedPin,LOW); 
      }

  

    }

    /**
    * Helper routine to dump a byte array as hex values to Serial.
    */
    void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
     }
    }