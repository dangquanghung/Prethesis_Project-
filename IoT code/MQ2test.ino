#include "mq2.h" 
#include "MQ7.h" 

#include "WiFiEsp.h" 
#include "secrets.h"  
#include "ThingSpeak.h" 

// always include thingspeak header file after other header files and custom macros

char ssid[] = "Dai Bang";   // your network SSID (name) 
char pass[] = "dinhkhoa";   // your network password
int keyIndex = 0;         // your network key Index number (needed only for WEP)
WiFiEspClient  client;
//change this with the pin that you use
int pin1 = A0;
int pin2 = A1;
double lpg, co, smoke;
mq2 m2 = mq2();
MQ7 mq7(A1,5.0);

float valSensorLPG = 0;
float valSensorMQ7 = 0;

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#define ESP_BAUDRATE  19200
#else
#define ESP_BAUDRATE  115200
#endif

unsigned long myChannelNumber = 1576707;
const char * myWriteAPIKey = "1QPXRCQEIJY2KJLW";

// Initialize our values
String myStatus = "Running";

void setup() {
  //Initialize serial and wait for port to open
  Serial.begin(115200);  // Initialize serial
  m2.inits();
  m2.calibrate(); //Initialize mq2
  delay(20000);
  while(!Serial){
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  // initialize serial for ESP module  
  setEspBaudRate(ESP_BAUDRATE);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  Serial.print("Searching for ESP8266..."); 
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  Serial.println("found it!");
   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop(){
  
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }
  
  // set the fields with the values
  valSensorLPG = getSensorLPG ();  
  valSensorMQ7 = getSensorMQ7Data();
  Serial.print("LPG: ");
  Serial.println( valSensorLPG);
  Serial.print("CO: ");
  Serial.println(valSensorMQ7);

  ThingSpeak.setField(1, (float)valSensorLPG);
  ThingSpeak.setField(2, (float)valSensorMQ7);
  
  // figure out the status message
  // set the status
  ThingSpeak.setStatus(myStatus);


  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  
  // change the values
 
  delay(20000); // Wait 20 seconds to update the channel again
}


// This function attempts to set the ESP8266 baudrate. Boards with additional hardware serial ports
// can use 115200, otherwise software serial is limited to 19200.
void setEspBaudRate(unsigned long baudrate){
  long rates[6] = {115200,74880,57600,38400,19200,9600};

  Serial.print("Setting ESP8266 baudrate to ");
  Serial.print(baudrate);
  Serial.println("...");

  for(int i = 0; i < 6; i++){
    Serial1.begin(rates[i]);
    delay(100);
    Serial1.print("AT+UART_DEF=");
    Serial1.print(baudrate);
    Serial1.print(",8,1,0,0\r\n");
    delay(100);  
  }
    
  Serial1.begin(baudrate);
}
float getSensorMQ7Data(){
   co = mq7.getPPM();
   return co;
   delay(20000);
}
float getSensorLPG(){
  double data[5];
  m2.getValue(false,'L',data);
  lpg = data[1];
  return lpg;
  delay(20000);
 
}

//float getSensorSmoke(){
//  float* values = mq2.read(true); //set it false if you don't want to print the values to the Serial
//  // smoke = values[2];
//  smoke = mq2.readSmoke();
//  return smoke;
//  delay(120000);
//  
//}
//float getSensorCO(){
//  float* values = mq2.read(true); //set it false if you don't want to print the values to the Serial
//  //co = values[1];
//  co = mq2.readCO();
//  return co;
//  delay(1000);
//}
