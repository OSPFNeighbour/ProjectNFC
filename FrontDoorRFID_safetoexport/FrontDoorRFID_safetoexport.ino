
byte MF_GET_SNR[8] = { 0xAA, 0x00, 0x03, 0x25, 0x26, 0x00, 0x00, 0xBB};
unsigned int readData[10]; // data read from serial
int counter = -1; // counter to keep position in the buffer
int readVal = 0; // individual character read from serial

boolean readyToTag = true; //flag used to delay the reading of tags.
long lastTagmills; // time of last tag process.
byte tagId[20]; // final tag ID converted to a string
#define tagRepeatDelay 1000 //number of milliseconds between RFID reads

#define SPEAKER 2 //3 on my other nano

#define RELAY 53


#define TAG_LEN 7
#define ARR_LEN 2
char target_tag[ARR_LEN][TAG_LEN] = 
{{0xx, 0xx, 0xx, 0xx, 0xx, 0xx, 0xx}, //serial 1
{0xx, 0xx, 0xx, 0xx, 0xx, 0xx, 0xx}};  //serial 2




void openDoor() {
  digitalWrite(RELAY, HIGH);
  delay(3000);
  digitalWrite(RELAY, LOW);
  memset(tagId,0,sizeof(tagId));
  memset(readData,0,sizeof(readData));

}


//Audible feedback to the user
void beep(unsigned char delayms){
  analogWrite(SPEAKER, 250);      // Almost any value can be used except 0 and 255
  delay(delayms);          // wait for a delayms ms
  analogWrite(SPEAKER, 0);       // 0 turns it off
  delay(delayms);          // wait for a delayms ms   
} 

void setup()  
{
  Serial.begin(9600);
  pinMode(3, OUTPUT);      //pinout setup


  pinMode(RELAY, OUTPUT);


  // No longer using NewSoftSerial but instead an Arduino Mega
  Serial1.begin(9600);
Serial.println("READY 15.30");

}

void parseTag() {

  int i;
  int ii = 3; //start at 5
  for (i = 0; i < 7; ++i) { //7 count
           Serial.print(readData[ii],HEX);
           Serial.print(" ");

    tagId[i] = readData[ii];
    ++ii;
  }
  
  Serial.println("--");
//  tagId[ii+1] = 0;
}
void printTag() {
    digitalWrite(3, HIGH); //LED on
    

  beep(120); //give a friendly beep
        for (int i=0; i<7; i++) { 
         Serial.print(tagId[i],HEX); 
         Serial.print(" ");
       }
        Serial.println("");
  
  Serial.println("");


  boolean found = 0;
   for(int i = 0; i < ARR_LEN; i++)
   {
     if(memcmp(tagId, target_tag[i], TAG_LEN) == 0 )
     {
        found = 1;
        beep(120);
        openDoor();
     }
   
   }
   
   if (found == 0) {
     beep(220);
   };




  
}

void processTag() {
  if (readyToTag) //if the hold down delay has passed
  {


    parseTag();




    // print it
    printTag();

    lastTagmills = millis();
    readyToTag = false;

  }
}




void loop()
{
  Serial1.write(MF_GET_SNR, 8);
  delay(50);

  while(Serial1.available() > 0) {

readVal = Serial1.read();

    // a "AA" signals the beginning of a tag
    if (readVal == 0xAA) {
      counter = 0; // start reading
    }  else if (readVal == 0xBB) {
      // process the tag we just read
      if (counter > 5) {
          for(int j = counter; j <= 10; j++)
         {
           readData[j+1] = 0x00;
           Serial.print("pad");
         }

        processTag();
    } else {
        counter = -1;
      }

    } else if (counter >= 0) {
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
    digitalWrite(3, LOW); //LED on

  }

}

