# MFRC522WithSDCard
R/W allowed RFID tag to SD Card with MFRC522 &amp; NodeMCU ESP8266


In read mode, this project allow you to Read list of allowed RFID tag from SD Card and copy that to Array in ESP8266.
In write mode, this project will scan RFID Card and verify that scanned card has saved before in SD Card or not, if card tag not in list of allowed card, it will save your tag id in SD Card.

library that needed in this project is :
- SPI
- MFRC522
- SD

Wiring Guide
ESP8266         MFRC522         SD Card Module

D1              -               CS
D4              SDA (SS)        -
D5              SCK             SCK
D6              MISO            MISO (Using R1k / R220 from MFRC522)
D7              MOSI            MOSI
-               IRQ             -
GND             GND             GND
D3              RST             -
3.3V            3.3V            -
5v (VV / Vin)   -               VCC

![image](https://github.com/bimarenth/MFRC522WithSDCard/assets/94059195/dba2be90-728e-4316-aa7c-df863aa79e39)
Source : https://forum.arduino.cc/t/sd-card-and-rfid-not-working-together/492955/5
