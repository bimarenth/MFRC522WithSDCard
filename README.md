# MFRC522WithSDCard
R/W allowed RFID tag to SD Card with MFRC522 &amp; NodeMCU ESP8266


In read mode, this project allow you to Read list of allowed RFID tag from SD Card and copy that to Array in ESP8266.
In write mode, this project will scan RFID Card and verify that scanned card has saved before in SD Card or not, if card tag not in list of allowed card, it will save your tag id in SD Card.

library that needed in this project is :
- SPI
- MFRC522
- SD
