#include <TinyGPS.h>
#include <AltSoftSerial.h>
#include<LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
bool check = true;
boolean newData = false;
boolean alert;
TinyGPS gps;// The TinyGPS++ object
int temp = 0, i;
//AltSoftSerial(uint8_t rxPin, uint8_t txPin, bool inverse = false);
// The serial connection to the GSM device
AltSoftSerial gsmSerial;//pin 9=Tx, pin 8=Rx
String stringVal = "";
float latitude;
float longitude;
void setup()
{
 gsmSerial.begin(9600);
 Serial.begin(9600);
 lcd.begin(16, 2);
 lcd.print("Vehicle Tracking");
 lcd.setCursor(0, 1);
 lcd.print(" System ");
 delay(100);
void loop()
{
 if (check)
 {
 gsm_init();
 lcd.clear();
 gsmSerial.println("AT+CNMI=2,2,0,0,0");
 lcd.print("GPS Initializing");
 lcd.clear();
 lcd.print("GPS is Ready");
 delay(200);
 lcd.clear();
 lcd.print("System Ready");
 delay(500);
 temp = 0;
 check = false;
 }
serialEvent();
 while (temp)
 {
 while (Serial.available())
 { 
 gps.encode(Serial.read()); 
// if (gps.location.isUpdated())
 tracking();
 temp = 0;
 }
 digitalWrite(13, LOW);
 }
}
void serialEvent()
{ 
while (gsmSerial.available())
 { 
 gsmSerial.println(" Vehicle Tracking");
 if (gsmSerial.find("Vehicle Tracking"))
 { lcd.clear();
 lcd.println("Message Received!!");
 temp = 1;
 break;
 }
 else
 temp=0;
}
}
void gsm_init()
{ 
lcd.print("Finding Module..");
 boolean at_flag = true;
 while (at_flag)
 {
 gsmSerial.println("AT");
 delay(100);
 while (gsmSerial.available())
 { 
 if (gsmSerial.find("OK"))
 at_flag = false;
 }
 delay(100);
 at_flag = false;
 }
 lcd.clear();
lcd.print("Module Connected..");
 delay(100);
 lcd.clear();
 lcd.print("Disabling ECHO");
 boolean echo_flag = true;
while (echo_flag)
 {
 gsmSerial.println("ATE0");
 gsmSerial.print("\r");
 while (gsmSerial.available() > 0)
 {
 if (gsmSerial.find("OK"))
 echo_flag = false;
 }
 delay(100);
 }
 lcd.clear();
lcd.print("Echo OFF");
 delay(100);
lcd.clear();
lcd.print("Finding Network..");
boolean net_flag = true;
 while (net_flag)
{
 gsmSerial.println("AT+CPIN?");
 while (gsmSerial.available())
 {
 if (gsmSerial.find("+CPIN:READY"))
 net_flag = false;
 }
 delay(100);
 }
 lcd.clear();
lcd.print("Network Found..");
delay(100);
lcd.clear();
}
void init_sms()
{
 gsmSerial.println("AT+CMGF=1");
delay(400);
 gsmSerial.println("AT+CMGS=\"+251932077104\""); // use your 10 digit cell no. here
delay(400);
}
void send_data(String message)
{
 gsmSerial.print(message);
delay(200);
}
void send_sms()
{
gsmSerial.write(26);
}
void lcd_status()
{
 lcd.clear();
lcd.print("Message Sent");
delay(100);
lcd.clear();
lcd.print("System Ready");
 delay(100);
}
void tracking()
//{ 
  for (unsigned long start = millis(); millis() - start < 1000;)

 {
 while (Serial.available())
 {
 char c = Serial.read();
 if (gps.encode(c))
 newData = true;
 }
 //}
}
 if (newData) //If newData is true
 {
 if (alert)
 return;
 float flat, flon;
 gps.f_get_position(&flat, &flon);
 lcd.clear();
 lcd.print("GPS Range Found");
 lcd.clear();
 lcd.print("Lat:");
 lcd.print(latitude);
 lcd.setCursor(0,1);
 lcd.print("Long:");
 lcd.print(longitude);
 delay(200);
 init_sms();
 send_data("Vehicle Tracking Alert:");
gsmSerial.println(" ");
 send_data("Your Vehicle Current Location is:");
gsmSerial.println(" ");
 gsmSerial.print("Latitude = ");
 gsmSerial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
 gsmSerial.print(" Longitude = ");
 gsmSerial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
 gsmSerial.print("https://www.google.com/maps/@");
 gsmSerial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
 gsmSerial.print(',');
 gsmSerial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
 gsmSerial.print(",14z");
 send_sms();
 delay(200);
 lcd_status();
 newData = false;
 alert = true;
 }
}
