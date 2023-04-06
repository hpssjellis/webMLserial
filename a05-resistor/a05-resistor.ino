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
 * 
 * I am having weird issues with analogRead A0 so using A1
 */



#include <Arduino.h> // Only needed for https://platformio.org/



#define MYDEBUG             false   // set to true if you need more information


#define CONVERT_G_TO_MS2    9.80665f
#define FREQUENCY_HZ        50
#define INTERVAL_MS         (1000 / (FREQUENCY_HZ + 1))
#define COLLECTION_SECONDS  1


bool myRaw = false;
float myFreq = FREQUENCY_HZ;
int mySeconds = COLLECTION_SECONDS;
int myMilliSeconds = COLLECTION_SECONDS * 1000;
int myMaxData = myFreq * mySeconds;
int myCount = 0;
int myDelay = INTERVAL_MS;   // non-block delay in milliseconds
unsigned long myStart; 
unsigned long myReset = 0; 


String readString;
bool mySendData = true;
int myA1;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // onboard LED, HIGH = off
  while (!Serial) {}                // do nothing and wait
  
  myStart = millis();   // set delay
  randomSeed(analogRead(A0));  // A1 pin on XIAO or Portenta does not have to be connected to anything
  Serial.println("timestamp,A1");  // CSV file heading titles
  

}

void loop() {

  if ( (millis() - myStart ) >= myDelay) {       
     myStart = millis();      //  reset the delay time
     myCount += 1;
     myA1 = analogRead(A1);
     //Serial.println( String(myStart)+ "," + String(analogRead(A0)) + "," + String(analogRead(A1)) + "," +  String(analogRead(A2))  );
     if (myCount > myMaxData){
        mySendData = false;   // stop sending data when amount reached
     }
     if (mySendData){  // yes send the requested data
       // Serial.println( String(myStart-myReset)+ "," + String(x) + "," + String(y) + "," +  String(z)  );
        if (myRaw) { // yes send raw data Note: no end of line.  
           Serial.print(String(myA1) + ", " );
        } else{    // no send formatted data
           Serial.println( String(myStart-myReset) + "," + String(myA1) );
        }
     } 
   }

   while (Serial.available()) {
    delay(3);  
    char myChar = Serial.read();
    readString += myChar; 
  }

  if (readString.length() > 0) {
    readString.trim();  // get rid of last weird character
  
    if (MYDEBUG){  
      Serial.println(readString); 
    }
    
    if (readString[0] == 'a'){
      digitalWrite(LED_BUILTIN, LOW); //onboard LED LOW = on
    }
    if (readString[0] == 'b'){
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if (readString == "start"){
       myRaw = false;
       myCount = 0;
       myStart = millis();      //  reset the delay time
       myReset = millis(); 
       mySendData = true;
    }
    if (readString == "raw"){
       myRaw = true;
       myCount = 0;
       myStart = millis();      //  reset the delay time
       myReset = millis(); 
       mySendData = true;
    }
    if (readString == "stop"){
      myCount = 0;
      myStart = millis();      //  reset the delay time
      myReset = millis(); 
      mySendData = false;
    }    
    if (readString[0] == 'd' && isDigit(readString[1]) ){
       int myIn = readString.substring(1).toInt();
       if (MYDEBUG){ 
          Serial.println("Setting the delay for " + String(myIn) + " milliseconds");
       }
       myDelay = myIn;
       // myDelay = (1000 / (myFreq + 1));
       if (myDelay > 0){
          myFreq = 1000 / (myDelay * 1.000); 
       } else {myFreq = 0.00001;}
       if (MYDEBUG){ 
          Serial.println("Setting the frquency to " + String(myFreq,3) + " Hz");
       }
       myMaxData = myFreq * mySeconds;
       myCount = 0;
       myStart = millis();      //  reset the delay time
       myReset = millis();
       mySendData = true;
    }

     if (readString[0] == 'c'  && isDigit(readString[1]) ){
       int myIn = readString.substring(1).toInt();
       if (MYDEBUG){
          Serial.println("Collecting for " + String(myIn) + " seconds");
       }
       mySeconds = myIn;
       myMaxData = myFreq * mySeconds;
       myCount = 0;
       myStart = millis();      //  reset the delay time
       myReset = millis(); 
       mySendData = true;
    }

     if (readString[0] == 'm'  && isDigit(readString[1]) ){
       int myIn = readString.substring(1).toInt();
       if (MYDEBUG){
          Serial.println("Collecting for " + String(myIn) + " milliseconds");
       }
       mySeconds = myIn/1000;   // fix this we need it as a float or something better perhaps maxData / 1000 etc.
       myMaxData = myFreq * mySeconds;
       myCount = 0;
       myStart = millis();      //  reset the delay time
       myReset = millis(); 
       mySendData = true;
    }


    
        if (readString[0] == 'f'  && isDigit(readString[1]) ){
       float myIn = readString.substring(1).toFloat();
       if (MYDEBUG){
          Serial.println("Setting the frquency to " + String(myIn,3) + " Hz");
       }
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
      Serial.println("timestamp,A1");  // CSV file heading titles
    }
    readString = "";   // blank out the readString
  } 
  
}

  
