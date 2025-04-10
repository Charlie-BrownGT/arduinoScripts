#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN  5
#define SS_PIN   53

MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

byte cardData[1024]; // Store full card content

void setup() {
  Serial.begin(9600);
  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println(F("MIFARE Classic Cloner"));

  // Set default key (FF FF FF FF FF FF)
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("Present source card to read from..."));
  readCard();
  Serial.println(F("Now present blank card to write to..."));
  writeCard();
}

void loop() {
  // nothing to do
}

void readCard() {
  waitForCard();
  for (byte sector = 0; sector < 16; sector++) {
    for (byte block = 0; block < 4; block++) {
      byte blockAddr = sector * 4 + block;
      byte buffer[18];
      byte size = sizeof(buffer);
      MFRC522::StatusCode status;

      // Authenticate
      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Auth failed at block ")); Serial.println(blockAddr);
        continue;
      }

      // Read block
      status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Read failed at block ")); Serial.println(blockAddr);
        continue;
      }

      // Copy to cardData
      memcpy(&cardData[blockAddr * 16], buffer, 16);
      Serial.print(F("Read block ")); Serial.print(blockAddr); Serial.println(F(" successfully"));
    }
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void writeCard() {
  waitForCard();
  for (byte sector = 0; sector < 16; sector++) {
    for (byte block = 0; block < 4; block++) {
      byte blockAddr = sector * 4 + block;

      // Skip UID block unless writing to a magic card (block 0)
      if (blockAddr == 0) continue;

      MFRC522::StatusCode status;

      // Authenticate
      status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522.uid));
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Auth failed at block ")); Serial.println(blockAddr);
        continue;
      }

      // Write block
      status = mfrc522.MIFARE_Write(blockAddr, &cardData[blockAddr * 16], 16);
      if (status != MFRC522::STATUS_OK) {
        Serial.print(F("Write failed at block ")); Serial.println(blockAddr);
        continue;
      }

      Serial.print(F("Wrote block ")); Serial.print(blockAddr); Serial.println(F(" successfully"));
    }
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void waitForCard() {
  while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(100);
  }
}
