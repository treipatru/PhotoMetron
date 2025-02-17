/*
  PhotoMetron V1.0 - A button-operated memories machine!
  
  Developed on Arduino Uno by andrei@planet34.org with help from the
  open-source community.

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
unsigned long     systemBoot;               // S since system boot
unsigned long     lastPrint;                // S since last print
unsigned long     printInterval = 60000;    // Minimum ms between prints
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
  printer.feed(1);

  randomSeed(analogRead(0));                // Unused analog as seed
}





//-----------------------------------------------------------------------------
// LOOP
void loop() {
  //---------------------------------------------------------------------------


  //***************************************************************************
  //***** Timer Managament
  systemBoot = millis();


  if ((systemBoot - lastPrint) < printInterval) {
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
  //Compose random filename
  int randomNumber = random (1, SDcardFileCount + 1);
  String fileExt = ".bin";
  String sdFiName = randomNumber + fileExt;

  //Transform it to something usable for the printer
  const char * c = sdFiName.c_str();

  //Print image header
  fPrintSeparator ();
  printer.println();

  //Print actual image
  File sdCardFile = SD.open (c, FILE_READ);

  while (sdCardFile.available()) {
    printer.printBitmap(printWidth, printHeight, dynamic_cast<Stream*>(&sdCardFile));
  }
  sdCardFile.close();

  delay(1000);                                  // 1 sec delay

  //Print image footer
  fPrintSeparator ();
  printer.justify('C');
  printer.println("- " + String(randomNumber, DEC) + " -");
  fPrintSeparator ();

  //Finish printing task
  printer.feed(4);                              // Breathing room
  lastPrint = millis();                         // Reset print timer
  printInterval = random(50400000, 86400000);   // Reset interval 14-24H
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

//***************************************************************************
//***** Print separator
int fPrintSeparator () {
  for (int i = 0; i < 32; i++) {
    printer.write(0xB0);
  }
}
