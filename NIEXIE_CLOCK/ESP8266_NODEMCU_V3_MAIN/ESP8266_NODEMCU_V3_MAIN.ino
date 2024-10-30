#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <EEPROM.h>
#define SSID            "NixieClock"  // SSID NAME
#define PASSWORD        "12345678"    // DEFINE PASSWORD
#define PORTAL_TIMEOUT  600           // 600s
#define NTP_SERVER      "pool.ntp.org"
#define NTP_TIMEOUT     3600000       // 3600000 -> 1 hour without NTP response before reboot
#define NTP_INTERVAL    1000          // 1000mS -> 1 seconds betwteen NTP queries
#define BOOT_DELAY      10000         // 10000mS -> 10 seconds delay in starting the program
#define EEPROM_SIZE 12                // size of memory EEPROM
unsigned long getTime();
void sendNTPpacket(IPAddress& address);
// Globals
// wifimanager can run in a blocking mode or a non blocking mode
// Be sure to know how to process loops with no delay() if using non blocking
bool MyWifiManager_nonblocking = false; // change to true to use non blocking
bool flagatimezone = true;
WiFiUDP UDP;
WiFiManager MyWifiManager;
WiFiManagerParameter custom_field; // global param ( for non blocking w params )
IPAddress timeServerIP;
const int NTP_PACKET_SIZE = 48;
byte NTPBuffer[NTP_PACKET_SIZE];
unsigned long prevNTP = 0;
unsigned long lastNTPResponse = millis();
unsigned long prevActualTime = 0;
unsigned long currentMillis = 0;
int actualTime = 0;
unsigned long minutyCale;
unsigned long godzinyCale;
unsigned long sekundyCale;
int timezone;
 
String inString = "";

void setup() {
  Serial.begin(115200);
  delay(BOOT_DELAY); // Allow for NixieClock to boot so messages can be viewed
  EEPROM.begin(EEPROM_SIZE);
  int address = 0;
  int boardId = 18;
  
  //Read data from eeprom
  address = 0;
  int readId;
  EEPROM.get(address, readId);
  //Serial.print("Read Id = ");
  //Serial.println(readId);
  address += sizeof(readId); //update address value

  int readParam;
  timezone=EEPROM.get(address, readParam); //readParam=EEPROM.readFloat(address);
  //Serial.print("Read param = ");
  //Serial.println(readParam);
  //Serial.println(timezone);
  MyWifiManager.setDebugOutput(false);
  
  // Configuration portal stays up for this amount of time on powerup
  //MyWifiManager.setConfigPortalTimeout(PORTAL_TIMEOUT);

  //exit after config instead of connecting
  MyWifiManager.setBreakAfterConfig(true);

  
  MyWifiManager.setAPClientCheck(true); // avoid timeout if client connected to softap
  pinMode(2, INPUT_PULLUP);
  
  //tries to connect to last known settings
  //if it does not connect it starts an access point and goes into a blocking loop awaiting configuration
  
  if(MyWifiManager_nonblocking) MyWifiManager.setConfigPortalBlocking(false);
  
   // add a custom input field
  int customFieldLength = 40;


   //new (&custom_field) WiFiManagerParameter("customfieldid", "Custom Field Label", "Custom Field Value", customFieldLength,"placeholder=\"Custom Field Placeholder\"");
  
   // testcustom html input type(checkbox)
   //new (&custom_field) WiFiManagerParameter("customfieldid", "Custom Field Label", "Custom Field Value", customFieldLength,"placeholder=\"Custom Field Placeholder\" type=\"checkbox\""); // custom html type
  
   //test custom html(radio)
  // to wewnątrz mojego TIMEZONE const char* custom_radio_str = "<br/><label for='customfieldid'>Custom Field Label</label><input type='radio' name='customfieldid' value='1' checked> One<br><input type='radio' name='customfieldid' value='2'> Two<br><input type='radio' name='customfieldid' value='3'> Three";
  
  const char* custom_radio_str ="<br><label for='customfieldid'>Set your time zone<br></label>" 
  "<br><input type='radio' name='customfieldid' value='-43200' checked> (UTC-12:00)<br><input type='radio' name='customfieldid' value='-39600'checked> (UTC-11:00)"
  "<br><input type='radio' name='customfieldid' value='-36000' checked> (UTC-10:00)<br><input type='radio' name='customfieldid' value='-32400'checked> (UTC-09:00)"
  "<br><input type='radio' name='customfieldid' value='-28800' checked> (UTC-08:00)<br><input type='radio' name='customfieldid' value='-25200'checked> (UTC-07:00)"
  "<br><input type='radio' name='customfieldid' value='-21600' checked> (UTC-06:00)<br><input type='radio' name='customfieldid' value='-18000'checked> (UTC-05:00)"
  "<br><input type='radio' name='customfieldid' value='-14400' checked> (UTC-04:00)<br><input type='radio' name='customfieldid' value='-10800'checked> (UTC-03:00)"
  "<br><input type='radio' name='customfieldid' value='-7200' checked> (UTC-02:00)<br><input type='radio' name='customfieldid' value='-3600'checked> (UTC-01:00)"
  "<br><input type='radio' name='customfieldid' value='0' checked> (UTC+00:00)<br><input type='radio' name='customfieldid' value='3600'checked> (UTC+01:00)"
  "<br><input type='radio' name='customfieldid' value='7200' checked> (UTC+02:00)<br><input type='radio' name='customfieldid' value='10800'checked> (UTC+03:00)"
  "<br><input type='radio' name='customfieldid' value='14400' checked> (UTC+04:00)<br><input type='radio' name='customfieldid' value='18000'checked> (UTC+05:00)" 
  "<br><input type='radio' name='customfieldid' value='21600' checked> (UTC+06:00)<br><input type='radio' name='customfieldid' value='25200'checked> (UTC+07:00)"
  "<br><input type='radio' name='customfieldid' value='28800' checked> (UTC+08:00)<br><input type='radio' name='customfieldid' value='32400'checked> (UTC+09:00)"
  "<br><input type='radio' name='customfieldid' value='36000' checked> (UTC+10:00)<br><input type='radio' name='customfieldid' value='39600'checked> (UTC+11:00)"
  "<br><input type='radio' name='customfieldid' value='43200' checked> (UTC+12:00)";  

  new (&custom_field) WiFiManagerParameter(custom_radio_str); // custom html input
  //const char* custom_radio_str = "<br/><label for='customfieldid'>customfieldid Label</label><input type='radio' name='customfieldid' value='1' checked> One<br><input type='radio' name='(UTC+01:00) Sarajevo, Skopje, Warsaw' value='2' > Two<br><input type='radio' name='customfieldid' value='3'> Three"; 
  MyWifiManager.addParameter(&custom_field);
  MyWifiManager.setSaveParamsCallback(saveParamCallback);

  // custom menu via array or vector
  // 
  // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
  const char* menu[] = {"wifi","param","sep","info","sep","restart","exit"}; 
  MyWifiManager.setMenu(menu,7);
  //std::vector<const char *> menu = {"wifi","info","param","sep","restart","exit"};
  //MyWifiManager.setMenu(menu);

  // set dark theme
  MyWifiManager.setClass("invert");


  //Serial.println("Connecting to AP");  ODZNACZ JAK BEDZIESZ KODOWAŁ DALEJ ZEBY WIDZIEC
  if (!MyWifiManager.autoConnect(SSID, PASSWORD)) {
    Serial.println("E AP Error Resetting ESP8266");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  UDP.begin(123);
  
  if(!WiFi.hostByName(NTP_SERVER, timeServerIP)) { // Get the IP address of the NTP server
    Serial.println("E DNS Error");
    Serial.flush();
    ESP.reset();
  }
  sendNTPpacket(timeServerIP); 
}

void loop() {
    
  currentMillis = millis();
   if(MyWifiManager_nonblocking) MyWifiManager.process(); // avoid delays() in loop when non-blocking and other long running code  
  //checkButton();
  // put your main code here, to run repeatedly:
  
  if (currentMillis - prevNTP > NTP_INTERVAL) { // If a NTP_INTERVAL has passed since last NTP request
    sendNTPpacket(timeServerIP);               // Send an NTP request
    prevNTP = currentMillis;
  }
  //saveParamCallback();
  //Serial.println(timezone);
  unsigned long time = getTime();                    // Check if an NTP response has arrived and get the (UNIX) time
  godzinyCale = ((time+timezone) % 86400) / 3600;
	minutyCale = ((time+timezone) % 86400 % 3600) / 60;
  sekundyCale = ((time+timezone) % 60);
  int godzina1 = godzinyCale / 10;
  int godzina2 = godzinyCale % 10;
  int minuta1 = minutyCale / 10;
  int minuta2 = minutyCale % 10; 
  int sekunda1 = sekundyCale / 10;
  int sekunda2 = sekundyCale % 10;
  if (time) {                                  // If a new timestamp has been received
   //Serial.println(time);
    
    Serial.print("czas=");
    Serial.print(godzina1);
    Serial.print(",");         // Print Unix time
    Serial.print(godzina2);
    Serial.print(",");
    
    Serial.print(minuta1);
    Serial.print(",");
    Serial.print(minuta2);
    Serial.print(",");

    Serial.print(sekunda1);
    Serial.print(",");
    Serial.println(sekunda2);
    //Serial.println(timezone);
    //Serial.println(EEPROM.get(0, readParam));    
    //Serial.println(getParam("customfieldid"));
    Serial.flush();
  
    lastNTPResponse = currentMillis;
  } 
  else if ((currentMillis - lastNTPResponse) > NTP_TIMEOUT) {
    Serial.println("E Timeout Error");  
    Serial.flush();
    ESP.reset();
  }

  //reset settings if GPIO2 pulled low on boot
  if (!digitalRead(2)) {
    MyWifiManager.resetSettings();
    ESP.reset();
  }
} // end loop

unsigned long getTime() {
  if (UDP.parsePacket() == 0) { return 0; } // If there's no response (yet)
    
  UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  
  // Combine the 4 timestamp bytes into one 32-bit number
  unsigned long NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
  
  // Convert NTP time to a UNIX timestamp:
  // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
  const unsigned long seventyYears = 2208988800UL;
  
  // subtract seventy years:
  unsigned long UNIXTime = NTPTime - seventyYears;
  
  return UNIXTime;
}

void sendNTPpacket(IPAddress& address) {
  memset(NTPBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
  
  // Initialize values needed to form NTP request
  NTPBuffer[0] = 0b11100011;   // LI, Version, Mode
  
  // send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); // NTP requests are to port 123
  UDP.write(NTPBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}

String getParam(String name){
  //read parameter from server, for customhmtl input
  String value;
  if(MyWifiManager.server->hasArg(name)) {
    value = MyWifiManager.server->arg(name);
  }
  return value;
}

void saveParamCallback(){
  
  inString=(getParam("customfieldid")); 
  timezone=inString.toInt();
  
  EEPROM.begin(EEPROM_SIZE);
  int address = 0;
  int boardId = 18;
  EEPROM.put(address, boardId);
  address += sizeof(boardId); //update address value
  int param = timezone;
  EEPROM.put(address, param);
  EEPROM.commit();

  //Read data from eeprom
  address = 0;
  float readId;
  EEPROM.get(address, readId);
  //Serial.print("Read Id = ");
  //Serial.println(readId);
  address += sizeof(readId); //update address value

  int readParam;
  EEPROM.get(address, readParam); //readParam=EEPROM.readFloat(address);
  //Serial.print("Read param = ");
  Serial.println(readParam);

  EEPROM.end();
}
