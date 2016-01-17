/*
  Photomaton V0.86
*/





//-----------------------------------------------------------------------------
// Initialize Things
//-----------------------------------------------------------------------------


//*****************************************************************************
//***** Printer
#include "SoftwareSerial.h"                 // Serial comms library
#include "Adafruit_Thermal.h"               // Printer library
#define TX_PIN 6                            // Transmit  YELLOW WIRE
#define RX_PIN 5                            // Receive   GREEN WIRE
SoftwareSerial mySerial(RX_PIN, TX_PIN);    // Declare SoftwareSerial
Adafruit_Thermal printer(&mySerial);        // Pass addr to printer constructor


//*****************************************************************************
//***** Card Reader
#include <SPI.h>                            // SPI library
#include <SD.h>                             // SD card SPI library
const int sdPin = 8;                        // SD card reader pin


//*****************************************************************************
//***** Set Variables
unsigned long     systemBoot;               // Seconds since system boot
unsigned long     lastPrint;                // Seconds since last print
const int         printInterval = 20;       // Minimum seconds between prints
const int         SDcardFileCount = 520;    // No of files on SD card
boolean           readyForPrint;            // If true, buttonpress will print

const int         buttonPin = 2;            // Connected button
const int         ledPin = 10;              // Led output
int               pinStatus = 0;            // Read pin status

const int         printWidth = 384;         //Width of images
const int         printHeight = 500;        //Height of images





//-----------------------------------------------------------------------------
// SETUP
void setup() {
  //---------------------------------------------------------------------------


  //***************************************************************************
  //***** Button Configuration
  pinMode (ledPin, OUTPUT);
  pinMode (buttonPin, INPUT_PULLUP);


  //***************************************************************************
  //***** Printer
  mySerial.begin(19200);                    // Init software serial
  printer.begin();                          // Init printer
  lastPrint = 0;


  //***************************************************************************
  //***** Card Reader
  pinMode (sdPin, OUTPUT);                  // Init card reader

  if (!SD.begin(sdPin)) {                   // Notify if card has issues
    printer.doubleHeightOn();
    printer.println(F("OH NOES!!!"));
    printer.doubleHeightOff();
    printer.println ("SD can't be read. Maybe ...");
    printer.feed(1);
    printer.println ("... the card is missing?");
    printer.println ("... the reader is damaged?");
    printer.feed(2);
    fResetPrinter();
    return;
  }
  printer.feed(2);

  randomSeed(analogRead(0));                // Unused analog as seed

}





//-----------------------------------------------------------------------------
// LOOP
void loop() {
  //---------------------------------------------------------------------------


  //***************************************************************************
  //***** Timer Managament
  systemBoot = millis() / 1000;


  if ((systemBoot - lastPrint) < printInterval) { // Seconds until print allowed
    readyForPrint = false;
    digitalWrite(ledPin, LOW);
  } else {
    readyForPrint = true;
    digitalWrite(ledPin, HIGH);
  }

  //***************************************************************************
  //***** Button Management
  fCheckButton ();
}





//-----------------------------------------------------------------------------
// OTHER
//-----------------------------------------------------------------------------


//***************************************************************************
//***** Button Press Function
int fCheckButton () {
  pinStatus = digitalRead(buttonPin);         // Read input value

  if (pinStatus == HIGH) {
  } else {
    if ( readyForPrint == true) {
      fPrintFile ();
    }
  }

}


//***************************************************************************
//***** Print a file
int fPrintFile () {

  int randomNumber = random (SDcardFileCount);
  String fileExt = ".bin";
  String sdFiName = randomNumber + fileExt;

  const char * c = sdFiName.c_str();


  File sdCardFile = SD.open (c, FILE_READ);

  while (sdCardFile.available()) {
    printer.printBitmap(printWidth, printHeight, dynamic_cast<Stream*>(&sdCardFile));
  }
  sdCardFile.close();

  delay(1000);                                  // 1 sec delay

  printer.feed(2);                              // Breathing room
  lastPrint = millis() / 1000;                  // Reset print timer
  fResetPrinter();                              // Reset printer
}


//***************************************************************************
//***** Reset the printer
int fResetPrinter () {
  printer.sleep();                              // Go to sleep
  delay(3000L);                                 // Wait 3 seconds
  printer.wake();                               // wake() before new print
  printer.setDefault();                         // Restore defaults
}
