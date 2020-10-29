
// Analog Solar Data 1.0 for Arduino
// This script will pull the XML feed from hamqsl.com and convert to a PWM output on pins 3 6 9 11. Output is also normalized to correct for any variance in meter movements.
// Additionaly pins 14 15 16 17 are also configured as a backlight for the display when the UTC data time falls within a pre-defined range


#include "string.h"
#include "WiFiEsp.h"

// Define Constants
// Max string length may have to be adjusted depending on data to be extracted
#define MAX_STRING_LEN  25


// Setup vars
char tagStr[MAX_STRING_LEN] = "";
char dataStr[MAX_STRING_LEN] = "";
char tmpStr[MAX_STRING_LEN] = "";
char endTag[3] = {'<', '/', '\0'};
int len;
int ledPin6 = 6;
int ledPin3 = 3;
int ledPin9 = 9;
int ledPin11 = 11;


// Flags to differentiate XML tags from document elements (ie. data)
boolean tagFlag = false;
boolean dataFlag = false;


// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(4, 5); // RX, TX
#endif

char ssid[] = "SSID";            // your network SSID (name)
char pass[] = "Password";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "hamqsl.com";

// Start ethernet client

WiFiEspClient client;

void setup()
{

Serial.println("Startup Sequence");
//Wipe the needles back and forth on Startup

  // fade in from min to max in increments of 5 points:
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin6, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin6, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
  // fade in from min to max in increments of 5 points:
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin3, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin3, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
  // fade in from min to max in increments of 5 points:
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin9, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin9, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
  // fade in from min to max in increments of 5 points:
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin11, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin11, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin3, fadeValue);
    analogWrite(ledPin6, fadeValue);
    analogWrite(ledPin9, fadeValue);
    analogWrite(ledPin11, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
  
    // fade out from max to min in increments of 5 points:
  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin3, fadeValue);
    analogWrite(ledPin6, fadeValue);
    analogWrite(ledPin9, fadeValue);
    analogWrite(ledPin11, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
 
//Blink all lights on startup  
pinMode(14, OUTPUT);

  digitalWrite(14, HIGH);   
  delay(1000);              
  digitalWrite(14, LOW);   
  delay(1000);
  digitalWrite(14, HIGH);   
  delay(1000);              
  digitalWrite(14, LOW);   
  delay(1000);   
  pinMode(15, OUTPUT);

  digitalWrite(15, HIGH);   
  delay(1000);              
  digitalWrite(15, LOW);   
  delay(1000);
  digitalWrite(15, HIGH);   
  delay(1000);              
  digitalWrite(15, LOW);   
  delay(1000);
  pinMode(16, OUTPUT);

  digitalWrite(16, HIGH);   
  delay(1000);              
  digitalWrite(16, LOW);   
  delay(1000);
  digitalWrite(16, HIGH);   
  delay(1000);              
  digitalWrite(16, LOW);   
  delay(1000);
  pinMode(17, OUTPUT);

  digitalWrite(17, HIGH);   
  delay(1000);              
  digitalWrite(17, LOW);   
  delay(1000);
  digitalWrite(17, HIGH);   
  delay(1000);              
  digitalWrite(17, LOW);   
  delay(1000);
  

  delay(10000);
  //Startup Serial 
 Serial.begin(9600);
  Serial1.begin(9600);
 Serial.println("connecting...");
 WiFi.init(&Serial1);

 

 // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
   // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");
  
 
 printWifiStatus();
 
 if (client.connect(server,80)) {
   Serial.println("connected");
   client.println("GET /solarxml.php HTTP/1.1"); 
   client.println("Host: hamqsl.com");
   client.println("Connection: close");   
   client.println();
    } else {
   Serial.println("connection failed");
 }  
}

void loop() {

 // Read serial data in from web:
 while (client.available()) {
   serialEvent();
 }

 if (!client.connected()) {
   Serial.println();
   Serial.println("Disconnected");
   client.stop();

   // Time until next update
   Serial.println("Waiting");
   /* for (int t = 1; t <= 15; t++)*/ {
     delay(3600000); // 1 minute
   }
 printWifiStatus();
   if (client.connect(server,80)) {
     Serial.println("Reconnected");
     client.println("GET /solarxml.php HTTP/1.1");
     client.println("Host: hamqsl.com");
     client.println("Connection: close");    
     client.println();
     //delay(2000);
   } else {
     Serial.println("Reconnect failed");
    
   }      
 }
}

// Process each char from web
void serialEvent() {

  // Read a char
     char inChar = client.read();
  //Serial.print(".");
 
  if (inChar == '<') {
     addChar(inChar, tmpStr);
     tagFlag = true;
     dataFlag = false;

  } else if (inChar == '>') {
     addChar(inChar, tmpStr);

     if (tagFlag) {      
        strncpy(tagStr, tmpStr, strlen(tmpStr)+1);
     }

     // Clear tmp
     clearStr(tmpStr);

     tagFlag = false;
     dataFlag = true;      
     
  } else if (inChar != 10) {
     if (tagFlag) {
        // Add tag char to string
        addChar(inChar, tmpStr);

        // Check for </XML> end tag, ignore it
        if ( tagFlag && strcmp(tmpStr, endTag) == 0 ) {
           clearStr(tmpStr);
           tagFlag = false;
           dataFlag = false;
        }
     }
     
     if (dataFlag) {
        // Add data char to string
        addChar(inChar, dataStr);
             }
  }  
 
  // If a LF, process the line
  if (inChar == 10 ) {



     // Find specific tags and print data
     if (matchTag("<solarflux>")) {
           Serial.print("Solarflux: ");
        Serial.print(dataStr);
        // Convert serial char to int
        int flux = atoi(dataStr);
        // Normalise values based on testing of needles on voltmeters 
        float fluxnormalized = (((float(flux) - 62.5)/237.5) * 241.0) + 4.0;
        round(fluxnormalized);
        int fluxpin = fluxnormalized;
        Serial.print("Normalized:");
        Serial.print(fluxpin);
        
        analogWrite(ledPin6,fluxpin);
               
        
     }
     if (matchTag("<sunspots>")) {
           Serial.print(", # of Sunspots: ");
        Serial.print(dataStr);
        // Convert serial char to int
        int spots = atoi(dataStr);
        // Normalise values based on testing of needles on voltmeters 
        float spotsnormalized = (((float(spots))/250.0) * 252.0) + 3.0;
        round(spotsnormalized);
        int spotspin = spotsnormalized;
        Serial.print("Normalized:");
        Serial.print(spotspin);
        
        
        analogWrite(ledPin3,spotspin);
     }
     if (matchTag("<muf>")) {
           Serial.print(", muf: ");
        Serial.print(dataStr);
        // Convert serial char to int
        int muf = atoi(dataStr);
        // Normalise values based on testing of needles on voltmeters 
        float mufnormalized = ((float(muf))/100.0) * 255.0;
        round(mufnormalized);
        int mufpin = mufnormalized;
        Serial.print("Normalized:");
        Serial.print(mufpin);
       
        
        
        analogWrite(ledPin9,mufpin);
     }

if (matchTag("<xray>")) {
        Serial.print(", X-Ray: ");
        Serial.print(dataStr);
         
        String xraydata =String(dataStr);
        String xrayletter = xraydata.substring(0,1);
        String xrayvalue = xraydata.substring(1,5);
        float xraylettertonum;
        if (xrayletter == "A") {
        xraylettertonum = 0.0;}
        else
        if (xrayletter == "B") {
          xraylettertonum = 10.0;}
          else
          if (xrayletter == "C") {
            xraylettertonum = 20.0;}
            else
            if (xrayletter == "M") {
              xraylettertonum = 30.0;}
              else
              if (xrayletter == "X") {
                xraylettertonum = 40.0;}
                else
                {Serial.print("No Letter Found");
              }

 
        
        // Normalise values based on testing of needles on voltmeters 
        float xrayfloatvalue = xrayvalue.toFloat();
        float xraynormalized = (((xrayfloatvalue + xraylettertonum)/49.9) * 242.0) + 3.0;
        round(xraynormalized);
        int xraypin = xraynormalized;
        Serial.print("Normalized:");
        Serial.print(xraypin);
        
        analogWrite(ledPin11,xraypin);
     }
     // Optional code for backlighting on timer follows
     
     if (matchTag("<updated>")) {
           Serial.print("updated: ");
        Serial.print(dataStr);
                 
        String timeupdated =String(dataStr);
        String timestringutc = timeupdated.substring(13,19);
        float xraylettertonum;
        int timeutc = timestringutc.toInt();
        Serial.print("timeutc:");
        Serial.print(timeutc);
                if (timeutc > 200 and timeutc < 700) 
        {digitalWrite(14,HIGH);
         digitalWrite(15,HIGH);
          digitalWrite(16,HIGH);
           digitalWrite(17,HIGH);}
        else
        {digitalWrite(14,LOW);
        digitalWrite(15,LOW);
        digitalWrite(16,LOW);
        digitalWrite(17,LOW);}
        
        
       
        
     }
      
               
  
     // Clear all strings
     clearStr(tmpStr);
     clearStr(tagStr);
     clearStr(dataStr);

     // Clear Flags
     tagFlag = false;
     dataFlag = false;
  
}
}
/////////////////////
// Other Functions //
/////////////////////

// Function to clear a string
void clearStr (char* str) {
  int len = strlen(str);
  for (int c = 0; c < len; c++) {
     str[c] = 0;
  }
}

//Function to add a char to a string and check its length
void addChar (char ch, char* str) {
  char *tagMsg  = "<TRUNCATED_TAG>";
  char *dataMsg = "-TRUNCATED_DATA-";

  // Check the max size of the string to make sure it doesn't grow too
  // big.  If string is beyond MAX_STRING_LEN assume it is unimportant
  // and replace it with a warning message.
  if (strlen(str) > MAX_STRING_LEN - 2) {
     if (tagFlag) {
        clearStr(tagStr);
        strcpy(tagStr,tagMsg);
     }
     if (dataFlag) {
        clearStr(dataStr);
        strcpy(dataStr,dataMsg);
     }

     // Clear the temp buffer and flags to stop current processing
     clearStr(tmpStr);
     tagFlag = false;
     dataFlag = false;

  } else {
     // Add char to string
     str[strlen(str)] = ch;
  }
}

// Function to check the current tag for a specific string
boolean matchTag (char* searchTag) {
  if ( strcmp(tagStr, searchTag) == 0 ) {
     return true;
  } else {
     return false;
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
