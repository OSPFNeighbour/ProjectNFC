#include <SoftwareSerial.h>
#define codeVersion "V2 2014-09-03"

//
//Tim Dykes 2014
//


//RFID READER setup
SoftwareSerial rfid = SoftwareSerial(11, 10); //pin2 Rx, pin3 Tx //11,10 on my nano

//Pinouts for the LED and Speaker
#define greenLED 12

#define SPEAKER 3 //3 on my other nano

#define tagRepeatDelay 1000 //number of milliseconds between RFID reads

// variables to keep state
int readVal = 0; // individual character read from serial
unsigned int readData[10]; // data read from serial
int counter = -1; // counter to keep position in the buffer
char tagId[11]; // final tag ID converted to a string

boolean readyToTag = true; //flag used to delay the reading of tags.
long lastTagmills; // time of last tag process.

void setup() {
  Serial.begin(9600); //Serial on the USB
  rfid.begin(9600); // set the data rate for the SoftwareSerial port for the RFID reader
  pinMode(greenLED, OUTPUT);   //pinout setup   
  pinMode(SPEAKER, OUTPUT);      //pinout setup
  Serial.println("READY - " codeVersion);
} 

//Audible feedback to the user
void beep(unsigned char delayms){
  analogWrite(SPEAKER, 250);      // Almost any value can be used except 0 and 255
  delay(delayms);          // wait for a delayms ms
  analogWrite(SPEAKER, 0);       // 0 turns it off
  delay(delayms);          // wait for a delayms ms   
} 

void processTag() {

  if (readyToTag) //if the hold down delay has passed
  {
    digitalWrite(greenLED, HIGH); //LED on
    beep(120); //give a friendly beep

    parseTag();

    // print it
    printTag();

    lastTagmills = millis();
    readyToTag = false;

  }
}
void parseTag() {
  int i;
  int ii = 3;
  for (i = 0; i < 7; ++i) {
    tagId[i] = readData[ii];
    ++ii;
  }
  tagId[10] = 0;
}
void printTag() {

  long decimalTagid = strtol(tagId,NULL,16); // need a long int version of the tag ID
  
  if(decimalTagid < 10000000) //need a 8 digit number for this particular project. so append a leading 0.
  {
      Serial.print("0");
  }
  Serial.println(decimalTagid);
}

// this function clears the rest of data on the serial, to prevent multiple scans
void clearSerial() {
  while (rfid.read() >= 0) {
    ; // do nothing
  }
}
void loop() // run over and over and over and over and over and over and over and over and over and over and over and over
{

  if (rfid.available() > 0) {
    // read the incoming byte:

    readVal = rfid.read();

    // a "2" signals the beginning of a tag
    if (readVal == 2) {
      counter = 0; // start reading
    } 
    // a "3" signals the end of a tag
    else if (readVal == 3) {
      // process the tag we just read
      processTag();

      // clear serial to prevent multiple reads
      clearSerial();

      // reset reading state
      counter = -1;
    }
    // if we are in the middle of reading a tag
    else if (counter >= 0) {
      // save valuee
      readData[counter] = readVal;

      // increment counter
      ++counter;
    } 
  }




  //Reset the holdown timer on the RFID Read
  if (millis() - lastTagmills >= tagRepeatDelay && !readyToTag)
  {
    readyToTag = true;
    digitalWrite(greenLED, LOW); //LED off

  }
} //end loop. lets do it again!







