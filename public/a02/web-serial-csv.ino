/*
 * webSerial for testing javascript connection with an arduino
 * 
 * Latest work at   https://github.com/hpssjellis/webMLserial
 * 
 * Note: On the Arduino Serial monitor make sure no line-ending or if statements will not work
 * 
 * Android https://hpssjellis.github.io/web-serial-polyfill/index.html
 * Laptops Desktops  https://hpssjellis.github.io/my-examples-of-arduino-webUSB-webSerial/public/index.html
 * IOS     not really sure
 * 
 */



#include <Arduino.h> // Only needed for https://platformio.org/
#include <Arduino_LSM9DS1.h>

#define CONVERT_G_TO_MS2    9.80665f
#define FREQUENCY_HZ        2
#define INTERVAL_MS         (1000 / (FREQUENCY_HZ + 1))
#define COLLECTION_SECONDS  20

float myFreq = FREQUENCY_HZ;
int mySeconds = COLLECTION_SECONDS;
int myMaxData = myFreq * mySeconds;
int myCount = 0;
int myDelay = INTERVAL_MS;   // non-block delay in milliseconds
unsigned long myStart; 
unsigned long myReset = 0; 


String readString;
bool mySendData = true;


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // onboard LED, HIGH = off
  while (!Serial) {}                // do nothing and wait
  
  myStart = millis();   // set delay
  randomSeed(analogRead(A0));  // AO pin on XIAO does not have to be connected to anything
  Serial.println("timestamp,accX,accY,accZ");  // CSV file heading titles
  
  if (!IMU.begin()) {
     Serial.println("Failed to initialize IMU!");
     while (1);
  }
}

void loop() {
  float x, y, z;
  if ( (millis() - myStart ) >= myDelay) {       
     myStart = millis();      //  reset the delay time
     myCount += 1;
     IMU.readAcceleration(x, y, z);
     //Serial.println( String(myStart)+ "," + String(analogRead(A0)) + "," + String(analogRead(A1)) + "," +  String(analogRead(A2))  );
     if (myCount > myMaxData){
        mySendData = false;   // stop sending data when amount reached
     }
     if (mySendData){
        Serial.println( String(myStart-myReset)+ "," + String(x) + "," + String(y) + "," +  String(z)  );
     } 
   }

   while (Serial.available()) {
    delay(3);  
    char myChar = Serial.read();
    readString += myChar; 
  }

  if (readString.length() > 0) {
    readString.trim();  // get rid of last weird character
    Serial.println(readString); 

    
    if (readString[0] == 'a'){
      digitalWrite(LED_BUILTIN, LOW); //onboard LED LOW = on
    }
    if (readString[0] == 'b'){
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if (readString == "start"){
       myCount = 0;
       myStart = millis();      //  reset the delay time
       myReset = millis(); 
       mySendData = true;
    }
    if (readString == "stop"){
      mySendData = false;
    }    
    if (readString[0] == 'd'){
       int myIn = readString.substring(1).toInt();
       Serial.println("Setting the delay for " + String(myIn) + " milliseconds");
       myDelay = myIn;
       // myDelay = (1000 / (myFreq + 1));
       if (myDelay > 0){
          myFreq = 1000 / (myDelay * 1.000); 
       } else {myFreq = 0.00001;}
       Serial.println("Setting the frquency to " + String(myFreq,3) + " Hz");
       myMaxData = myFreq * mySeconds;
       myCount = 0;
       myStart = millis();      //  reset the delay time
       myReset = millis();
       mySendData = true;
    }

        if (readString[0] == 'c'){
       int myIn = readString.substring(1).toInt();
       Serial.println("Collecting for " + String(myIn) + " seconds");
       mySeconds = myIn;
       myMaxData = myFreq * mySeconds;
       myCount = 0;
       myStart = millis();      //  reset the delay time
       myReset = millis(); 
       mySendData = true;
    }
        if (readString[0] == 'f'){
       float myIn = readString.substring(1).toFloat();
       Serial.println("Setting the frquency to " + String(myIn,3) + " Hz");
       myFreq = myIn;
       if (myFreq > 0){
       myDelay = (1000 / (myFreq)); }
       else {myDelay = 1000;}
       myMaxData = myFreq * mySeconds;
       myCount = 0;
       myStart = millis();      //  reset the delay time 
       myReset = millis();  
       mySendData = true;    
    }


    
    if (readString == "firstline" ){
      Serial.println("timestamp,accX,accY,accZ");  // CSV file heading titles
    }
    readString="";
  } 
  
}

  
