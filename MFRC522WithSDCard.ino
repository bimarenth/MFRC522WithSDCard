#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>

// Pin that used for MFRC522, please reffer another site for more detail wiring diagram
#define RST_PIN 0
#define SS_PIN  2

// Pin Chip Select for Micro SD Card module
#define CS_SD 4

#define DOOR 5
#define PIN_MODE A0

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class MFRC522

bool MODE_WRITE;
int wrongAttempt = 0;

String uidCard; // Variable that save tag id from scanned card

File myFile; // Instance of the class SD Card

String allowedCard[200]; // Array that saved allowed card list from SD Card file

?? Buzzer beep function
void beepBuzzer(int x, int y) {
  for (int i = 0; i < x; i++) {
    // digitalWrite(BUZZER, HIGH);
    Serial.println("Beep!!!");
    delay(y);
    // digitalWrite(BUZZER, LOW);
    Serial.println("Beep!!!");
    delay(y);
  }
}

// Function for switch relay
void openDoor(int y) {
  digitalWrite(DOOR, HIGH);
  //  Serial.println("Cekrek!!");
  delay(y);
  digitalWrite(DOOR, LOW);
  //  Serial.println("Cekrek!!");
}

// Scan RFID tag and save it to uidCard variable
void scanCard() {
  uidCard = "";

  rfid.PICC_ReadCardSerial();
  Serial.print("Tag UID: ");
  for (byte i = 0; i < rfid.uid.size; i++) {
    //      readCard.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : " "));
    uidCard.concat(String(rfid.uid.uidByte[i], HEX));
    uidCard.toUpperCase();
  }
  Serial.println(uidCard);
  delay(1000); // 1 second halt
}

// Check if scanned card match with allowed card list
void checkCard(String scannedCard) {
  bool isCardTrue;
  
  for (int i = 0; i < 200; i++) {
    (allowedCard[i] == scannedCard) ? isCardTrue = true : isCardTrue = false;
    if (isCardTrue == true) break;
  }

  if (isCardTrue == true) {
    wrongAttempt = 0;
    Serial.println("Pintu kebuka");
    openDoor(2000);

  } else {
    Serial.println("Salah");
    wrongAttempt++;
    Serial.println("Jumlah salah: " + String(wrongAttempt));
    if (wrongAttempt > 3) {
      Serial.println("Salah 3x");
      beepBuzzer(2, 100);
    }
  }
}

// function for save SD Card allowed card lists to Array allowedCard
void readData(bool writeMode) {
  String parameter;
  byte line;
  
  myFile = SD.open("RFID.txt");
  if (myFile) {
    while (myFile.available()) {
      char data = myFile.read();
      if (isPrintable(data)) {
        parameter.concat(data);
      } else if (data == '\n') {
        allowedCard[line] = parameter;
        parameter = "";
        line++;
      }
    }
  }

  myFile.close();
  Serial.println("Success read from SD Card");
  
  if (writeMode == true) {
    int freeArray;
    for (int i = 0; i < 200; i++) {
      //      Serial.println(allowedCard[i]);
      if (allowedCard[i] == "") freeArray++;
    }
    Serial.print("Memori yang tersisa: ");
    Serial.print(freeArray);
    Serial.println(" Kartu");
  }
}

// Write new card to SD Card if it not listed before
void writeCard(String scannedCard) {
  // Enables SD card chip select pin
  digitalWrite(CS_SD, LOW);

  // Open file
  myFile = SD.open("RFID.txt", FILE_WRITE);

  // If the file opened ok, write to it
  if (myFile) {
    Serial.println("File opened ok");
    myFile.println(scannedCard);

    Serial.println("sucessfully written on SD card");
    myFile.close();
  }
  else {
    Serial.println("error opening FILE.txt");
  }
  // Disables SD card chip select pin
  digitalWrite(CS_SD, HIGH);

  delay(2000);
}

void verifyWrite(String scannedCard) {
  bool writeToSD = false;

  for (int i = 0; i < 200; i++) {
    (allowedCard[i] == scannedCard) ? writeToSD = true : writeToSD = false;
    if (isCardTrue == true) break;
  }

  if (writeToSD == true) {
    writeCard(scannedCard);
  } else {
    Serial.println("Kartu sudah terdaftar");
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(DOOR, OUTPUT);
  pinMode(PIN_MODE, INPUT_PULLUP);

  SPI.begin();
  Serial.begin(115200);

  rfid.PCD_Init();

  if (!SD.begin(CS_SD)) {
    while (true) {
      beepBuzzer(3, 200);
      delay(500);
    }
    return;
  }

  if (analogRead(PIN_MODE) >= 1023) {
    MODE_WRITE = true;
    Serial.println("Mode Write");
    readData(MODE_WRITE);
  } else {
    MODE_WRITE = false;
    Serial.println("Mode Read");
    readData(MODE_WRITE);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (MODE_WRITE == false) {
    if (rfid.PICC_IsNewCardPresent()) {
      scanCard();
      checkCard(uidCard);
    }
  } else {
    if (rfid.PICC_IsNewCardPresent()) {
      scanCard();
      verifyWrite(uidCard);
    }
  }

}
