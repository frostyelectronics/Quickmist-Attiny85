# Quickmist-Attiny85
4 green LEDs and 1 red LED with timers for ATTiny85

## About
- Power concumption without LEDs = 0,057mA (=146.2 days on a 200mAh coin cell) 
- 2xLED = 0,156mA (red+green)
- 3xLED = 0,192mA (red+green+green)
- 4xLED = 0,1226mA (red+green+green+green)
- Power concumption with all 5 LEDs lit = 0,257mA (= 32.4 days on a 200mAh coin cell)

## Instructions:
### Hardware
- Resistors: 1-4 = 22kOhm, 5 = 4,7kOhm. Resistor 5 is a little lower because it's on pin1 (reset-pin that has been configured as IO)
- LEDs: 1-4 = Green, 5 = Red

### Programming
- Connect USBTinyISP to ATTiny85.
- Upload the sketch [2019-03-12-attiny85-nicorette-jlcpcb-06.ino](2019-03-12-attiny85-nicorette-jlcpcb-06.ino) to ATTiny
- Set the reset-pin (pin1 (PB5)) to IO using a high voltage programmer. Connect the programmer to an Arduino UNO and upload sketch [2019-01-30-attiny85-Set_fuses_03.ino](2019-01-30-attiny85-Set_fuses_03.ino)
- [High voltage programmer circuit](high-voltage-programmer.png)


