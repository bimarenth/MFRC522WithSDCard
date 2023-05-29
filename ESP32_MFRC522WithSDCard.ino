#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <MFRC522.h>

// Pin that used for MFRC522, please reffer another site for more detail wiring diagram
#define RST_PIN 27
#define SS_PIN  5

#define CS_SD 4 // Pin Chip Select for Micro SD Card module

#define DOOR 26
#define PIN_MODE 12

bool MODE_WRITE = false;
int wrongAttempt = 0;
bool isCardTrue;
String parameter;
byte line;

String allowedCard[200]; // Array that saved allowed card list from SD Card file

String uidCard; // Variable that save tag id from scanned card

File myFile; // Instance of the class SD Card
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class MFRC522

// Buzzer beep function
void beepBuzzer(int x, int y) {
  for (int i = 0; i < x; i++) {
    digitalWrite(BUZZER, HIGH);
    // Serial.println("Beep!!!");
    delay(y);
    digitalWrite(BUZZER, LOW);
    // Serial.println("Beep!!!");
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
    //      uidCard.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : " "));
    uidCard.concat(String(rfid.uid.uidByte[i], HEX));
    uidCard.toUpperCase();
  }
  Serial.println(uidCard);
  delay(1000); // 1 second halt
}

// Check if scanned card match with allowed card list
void checkCard(String scannedCard) {
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
void readData(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);

  myFile = fs.open(path);
  if (!myFile) {
    Serial.println("Failed to open file for reading");
    return;
  }

  while (myFile.available()) {
    // Serial.write(file.read());
    char data = myFile.read();
    if (isPrintable(data)) {
      parameter.concat(data);
    } else if (data == '\n') {
      allowedCard[line] = parameter;
      parameter = "";
      line++;
    }
  }

  myFile.close();
}

// Write new card to SD Card if it not listed before
void writeCard(String scannedCard) {
  digitalWrite(CS_SD, LOW); // Enables SD card chip select pin

  myFile = SD.open("RFID.txt", FILE_WRITE); // Open file

  // If the file opened ok, write to it
  if (myFile) {
    Serial.println("File opened ok");
    myFile.println(scannedCard);

    Serial.println("sucessfully written on SD card");
    myFile.close();
  } else {
    Serial.println("error opening FILE.txt");
  }

  digitalWrite(CS_SD, HIGH); // Disables SD card chip select pin

  delay(2000);
}

// Verify if scanned card has saved before in SD Card or not
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

void calFreeCard() {
  int freeArray = 0;
  for (int i = 0; i < 200; i++) {
    if (allowedCard[i] == 0) freeArray++;
  }

  Serial.print("Memori yang tersisa: ");
  Serial.print(freeArray);
  Serial.println(" Kartu");
}

// Setup code
void setup() {
  pinMode(DOOR, OUTPUT);
  pinMode(PIN_MODE, INPUT_PULLUP);
  SPI.begin();
  Serial.begin(115200);
  rfid.PCD_Init();

  if (!SD.begin(CS_SD)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  if (digitalRead(PIN_MODE) == 0) {
    MODE_WRITE = true;
    Serial.println("Mode Write");
    readData(SD, "/RFID.txt");
  } else {
    MODE_WRITE = false;
    Serial.println("Mode Read");
    readData(SD, "/RFID.txt");
  }

  //  for (int i = 0; i < 200; i++) {
  //    Serial.println(allowedCard[i]);
  //  }
}

// Loop code
void loop() {
  if (MODE_WRITE == false) {
    if (rfid.PICC_IsNewCardPresent()) {
      scanCard();
      checkCard(uidCard);
    }
  } else {
    if (rfid.PICC_IsNewCardPresent()) {
      scanCard();
      verifyWrite(uidCard);
      calFreeCard();
    }
  }
}
