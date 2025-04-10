#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN  5   // RST pin connected to pin 5 on the Elegoo 2560
#define SS_PIN   53  // SDA (SS) pin connected to pin 53 on the Elegoo 2560

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600);
  while (!Serial);        // Wait for serial monitor
  SPI.begin();            // Init SPI bus
  mfrc522.PCD_Init();     // Init MFRC522 reader
  Serial.println(F("Scan a card to identify its type..."));
}

void loop() {
  // Look for a new card
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial())
    return;

  // Print card UID
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // Determine and print card type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.print(F("Card type: "));
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Stop communication with card
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  delay(1000); // Slight delay before next scan
}
