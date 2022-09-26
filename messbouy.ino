#include <SPI.h>
#include <SD.h>

/*
 * With the currente implementation the max runtime is 49 Days. Unsignes longs have a range of 0 to 4,294,967,295.
 * Hence after 49 days the currentMills variable runs out of space.
 */

// CONSTANTS
const int BUTTON_PIN = 2;

const int MEASURE_PERIOD = 10000; //milliseconds is the unit (1 second is 1000 ms)

// VARIABLES
byte prevButtonState = 0;    // indicates the previous state of the Button (0 = not pressed, 1 = pressed)
int rotationCounter = 0;  //used as counter for the impeller rotations

unsigned long periodEnd;  //indicates when each measure period ends
String dataString = ""; //used for saving the data values to SD
File dataFile;  //variable for the dataFile


void writeDataPoint(int rotations, unsigned long time){
  dataString = String(time) + ";" + String(rotations);
  dataFile = SD.open("Data.csv", FILE_WRITE);
    if (dataFile){
      dataFile.println(dataString);
      dataFile.close();
    } else {
      Serial.println("error opening datafile");
    }
  Serial.println(dataString);
}


// runs once at startup
void setup() {
  Serial.begin(9600); // start serial communication (with computer)

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.print("Initializing SD card...");
  //init SD card
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");


  if(!SD.exists("Data.csv")){
    dataFile = SD.open("Data.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println("Timestamp;Rotations");
      // close the file:
      dataFile.close();
      Serial.println("done.");
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening currentData.csv");
    }
  }
  

  // init pins
  pinMode(BUTTON_PIN, INPUT); // initialize the button pin as input:
  // set first period
  periodEnd = millis() + MEASURE_PERIOD;
}

// keeps on running
void loop() {
 // do during period 
 if(millis() < periodEnd) {
     // if button is pressed
    if (digitalRead(BUTTON_PIN) == HIGH && prevButtonState == 0) {
      rotationCounter += 1; // add one to the counter
      prevButtonState = 1;
    } else if (digitalRead(BUTTON_PIN) == LOW){
      prevButtonState = 0;
    }
  } else if (millis() >= periodEnd) {
    writeDataPoint(rotationCounter, millis());
    rotationCounter = 0;
    periodEnd = millis() + MEASURE_PERIOD;
  }
}
