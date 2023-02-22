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

// for proximity, gesture, colour
#include <Arduino_APDS9960.h> 

/* For LSM9DS1 9-axis IMU sensor */
#include <Arduino_LSM9DS1.h>

#define CONVERT_G_TO_MS2    9.80665f
#define FREQUENCY_HZ        50
#define INTERVAL_MS         (1000 / (FREQUENCY_HZ + 1))
#define COLLECTION_SECONDS  10


int proximity, gesture, colorR, colorG, colorB;

/* variables to hold LSM9DS1 accelerometer data */
float accelerometerX, accelerometerY, accelerometerZ;
/* variables to hold LSM9DS1 gyroscope data */
float gyroscopeX, gyroscopeY, gyroscopeZ;
/* variables to hold LSM9DS1 magnetic data */
float magneticX, magneticY, magneticZ;






int myMaxData = FREQUENCY_HZ * COLLECTION_SECONDS;
int myCount = 0;
int myDelay = 30000;  // 30000 = 30 seconds    // INTERVAL_MS;   // non-block delay in milliseconds
unsigned long myStart; 

String readString;
bool mySendData = true;


void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // onboard LED, HIGH = off
  while (!Serial) {}                // do nothing and wait
  
  myStart = millis();   // set delay
  randomSeed(analogRead(A0));  // AO pin on XIAO does not have to be connected to anything
 
  
 // Serial.println("timestamp,accX,accY,accZ");  // CSV file heading titles
 // No timestamp needed for single data csv files, but might as well include it
  
   Serial.println("timestamp,proximity,gesture,colorR,colorG,colorB,accX,accY,accZ,gyroX,gyroY,gyroZ,magX,magY,magZ"); 
   
  /* IMU setup for LSM9DS1*/
  /* default setup has all sensors active in continous mode. Sample rates
   *  are as follows: magneticFieldSampleRate = 20Hz, gyroscopeYroscopeSampleRate = 109Hz,
   * accelerationSampleRate = 109Hz */ 
  if (!IMU.begin()) {
     Serial.println("Failed to initialize IMU!");
     while (1);
  }
  APDS.setGestureSensitivity(70);  // 0 to 100
    if (!APDS.begin()){
      Serial.println("Error initializing APDS9960 sensor.");
    /* Hacky way of stopping program executation in event of failure. */
      while(1);
    }
    // As per Arduino_APDS9960.h, 0=100%, 1=150%, 2=200%, 3=300%. Obviously more
    // boost results in more power consumption. 
    APDS.setLEDBoost(3);   // default 0


  
}

void loop() {
 // float x, y, z;
  if ( (millis() - myStart ) >= myDelay) {       
     myStart = millis();      //  reset the delay time
     myCount += 1;


 //Grab new data if available.

   if (APDS.proximityAvailable()){
      proximity = 240 +(APDS.readProximity()*-1);  
   }
   if ( APDS.colorAvailable()) {
      APDS.readColor(colorR, colorG, colorB);
   }  
   if (APDS.gestureAvailable()) {
    // a gesture was detected, read and print to Serial Monitor
      int gesture = APDS.readGesture();
   } 
  /* If new acceleration data is available on the LSM9DS1 get the data.*/
  if(IMU.accelerationAvailable())
  {
    IMU.readAcceleration(accelerometerX, accelerometerY, accelerometerZ);
    accelerometerX = 9.8 * accelerometerX;   // acceleration to Force F = gA = 9.8 *a
    accelerometerY = 9.8 * accelerometerY;  
    accelerometerZ = 9.8 * accelerometerZ;  
  }
  
  /* If new gyroscope data is available on the LSM9DS1 get the data.*/
  if(IMU.gyroscopeAvailable())
  {
    IMU.readGyroscope(gyroscopeX, gyroscopeY, gyroscopeZ);
    gyroscopeX = 1.0 *gyroscopeX; 
    gyroscopeY = 1.0 *gyroscopeY; 
    gyroscopeZ = 1.0 *gyroscopeZ; 
  }
  
  /* If new magnetic data is available on the LSM9DS1 get the data.*/
  if (IMU.magneticFieldAvailable())
  {
    IMU.readMagneticField(magneticX, magneticY, magneticZ);
    magneticX = 1.0 * magneticX; 
    magneticY = 1.0 * magneticY; 
    magneticZ = 1.0 * magneticZ; 
  }




     
   //  IMU.readAcceleration(x, y, z);


     
  if (mySendData){
    
     // Proximity first        
     //  Serial.print(String(proximity) + ".00,");  // to format like a float, probably not needed
    Serial.print(String(proximity) + "," +String(gesture)+ "," + String(colorR) + "," +String(colorG) + "," +String(colorB) + "," );  

    // Serial.printf("%f,%f,%f,", accelerometerX, accelerometerY, accelerometerZ);
    Serial.print(String(accelerometerX, 2) + ","+String(accelerometerY, 2) + ","+String(accelerometerZ, 2) + ","); 
     
    // Serial.printf("%f,%f,%f,", gyroscopeX, gyroscopeY, gyroscopeZ);
    Serial.print(String(gyroscopeX, 2) + ","+String(gyroscopeY, 2) + ","+String(gyroscopeZ, 2) + ","); 
     
    // Serial.printf("%f,%f,%f,", magneticX, magneticY, magneticZ);
    Serial.print(String(magneticX, 2) + ","+String(magneticY, 2) + ","+String(magneticZ, 2)   );  

    // more data colection could go here
    
    Serial.println();   // just to finish the set of data
               
     } // end sendData
   }  // end millis()

   while (Serial.available()) {
    delay(3);  
    char myChar = Serial.read();
    readString += myChar; 
  }

  if (readString.length() > 0) {
    if (readString == "a"){
      digitalWrite(LED_BUILTIN, LOW); //onboard LED LOW = on
    }
    if (readString == "b"){
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if (readString == "start"){
      mySendData = true;
      myCount = 0;
    }
    if (readString == "stop"){
      mySendData = false;
    }
    if (readString == "firstline"){
     Serial.println("timestamp,proximity,gesture,colorR,colorG,colorB,accX,accY,accZ,gyroX,gyroY,gyroZ,magX,magY,magZ"); 
     // CSV file heading titles
    }
    readString="";
  } 
  
}

  
