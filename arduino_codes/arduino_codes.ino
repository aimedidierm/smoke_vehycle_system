#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#include <SoftwareSerial.h>
SoftwareSerial gps(3,4); // RX, TX
SoftwareSerial sim(10,9);
//////////////////////////////////////////////
int _timeout;
String _buffer;
String number = "0788750979"; //-> change with your number
////////////////////////////////////////////////////////
#define sensor A1
#define led 13
#define buz 9
#define relay 7
//String str="";
char str[70];
String gpsString="";

char *test="$GPGGA";

String latitude="No Range      ";
String longitude="No Range     ";
String land1="";
String land2="";
String land3="";

int temp=0,i;
boolean gps_status=0;

int var=0;
void setup() 
{


  lcd.begin(16,2);
  lcd.init();                      // initialize the lcd 
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  gps.begin(9600);
  sim.begin(112500);
  pinMode(sensor, INPUT);
  pinMode(buz, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(relay,OUTPUT);

  lcd.print("Anti air ");
  lcd.setCursor(0,1);
  lcd.print("pollution system");
  delay(2000);
  //gsm_init();
  lcd.clear();
  Serial.println("AT+CNMI=2,2,0,0,0");
  lcd.print("GPS Initializing");
  lcd.setCursor(0,1);
  lcd.print("  No GPS Range  ");
  //get_gps();
  delay(2000);
  lcd.clear();
  lcd.print("GPS Range Found");
  lcd.setCursor(0,1);
  lcd.print("GPS is Ready");
  delay(2000);
  lcd.clear();
  lcd.print("System Ready");
  //temp=0;
     // Serial.println("Sistem Started...");
     //sim.begin(9600);
     // send_sms("Please take some action soon...\nThankyou");
    // callNumber();
}

void loop()
{
  callNumber();
  if (sim.available() > 0)
    Serial.write(sim.read());
  serialEvent();
  if(temp==1 && var==0)
  {
    get_gps();
    tracking();
  }
}
void serialEvent(){
  float adcValue=0;
  for(int i=0;i<10;i++)
  {
    adcValue+= analogRead(sensor);
    delay(10);
  }
    float v= (adcValue/10) * (5.0/1024.0);
    float mgL= 0.67 * v;
    Serial.print("BAC:");
    Serial.print(mgL);
    Serial.print(" mg/L");
    lcd.setCursor(0,0);
    lcd.print("BAC: ");
    lcd.print(mgL,4);
    lcd.print(" mg/L        ");
    lcd.setCursor(0,1);
    if(mgL > 0.8)
    { 
      lcd.print("Pollution");
      Serial.println("Pollution");
      digitalWrite(buz, HIGH);
      digitalWrite(led, HIGH);
      digitalWrite(relay, LOW);
      temp=1;
    }
    else
    {
      lcd.print("Normal  ");
      Serial.println("    Normal");
      digitalWrite(buz, LOW);
      digitalWrite(led, LOW);
      digitalWrite(relay, HIGH);
      temp=0;
      var=0;
    }
  }

void gpsEvent()
{
  gpsString="";
  while(1)
  {
   while (gps.available()>0)            //checking serial data from GPS
   {
    char inChar = (char)gps.read();
     gpsString+= inChar;                    //store data from GPS into gpsString
     i++;
     if (i < 7)                      
     {
      if(gpsString[i-1] != test[i-1])         //checking for $GPGGA sentence
      {
        i=0;
        gpsString="";
      }
     }
    if(inChar=='\r')
    {
     if(i>65)
     {
       gps_status=1;
       break;
     }
     else
     {
       i=0;
     }
    }
  }
   if(gps_status)
    break;
  }
}

void get_gps()
{
   gps_status=0;
   int x=0;
   while(gps_status==0)
   {
    gpsEvent();
    int str_lenth=i;
    latitude="";
    longitude="";
    int comma=0;
    while(x<str_lenth)
    {
      if(gpsString[x]==',')
      comma++;
      if(comma==2)        //extract latitude from string
      latitude+=gpsString[x+1];     
      else if(comma==4)        //extract longitude from string
      longitude+=gpsString[x+1];
      x++;
    }
    int l1=latitude.length();
    latitude[l1-1]=' ';
    l1=longitude.length();
    longitude[l1-1]=' ';
    lcd.clear();
    lcd.print("Lat:");
    lcd.print(latitude);
    lcd.setCursor(0,1);
    lcd.print("Long:");
    lcd.print(longitude);
    i=0;x=0;
    str_lenth=0;
    delay(2000);
   }
}

void init_sms()
{
  Serial.println("AT+CMGF=1");
  delay(400);
  Serial.println("AT+CMGS=\"+250788750979\"");   // use your 10 digit cell no. here
  delay(400);
}

void send_data(String message)
{
  Serial.println(message);
  delay(200);
}



void lcd_status()
{
  lcd.clear();
  lcd.print("Message Sent");
  delay(2000);
  lcd.clear();
  lcd.print("System Ready");
  return;
}

void tracking()
{
    init_sms();
    send_data("simeon :");
    Serial.print("Latitude:");
    send_data(latitude);
    Serial.print("Longitude:");
    send_data(longitude);
    Serial.print("Click here to see location:");
    Serial.print("http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=");
    Serial.print(latitude);
    Serial.print("+");              //28.612953, 77.231545   //28.612953,77.2293563
    Serial.print(longitude);
    send_data("Please take some action soon...\nThankyou");
    send_sms("Please take some action soon...\nThankyou");
    delay(2000);
    lcd_status();
    var=1;
}
void send_sms(String text)
{
  //Serial.println ("Sending Message");
  sim.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(200);
  //Serial.println ("Set SMS Number");
  sim.println("AT+CMGS=\"" + number + "\"\r"); //Mobile phone number to send message
  delay(200);
  String SMS = "Hello ";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);// ASCII code of CTRL+Z
  delay(200);
  _buffer = _readSerial();
}
void RecieveMessage()
{
  Serial.println ("SIM800L Read an SMS");
  sim.println("AT+CMGF=1");
  delay (200);
  sim.println("AT+CNMI=1,2,0,0,0"); // AT Command to receive a live SMS
  delay(200);
  Serial.write ("Unread Message done");

  
}
String _readSerial() {
  _timeout = 0;
  while  (!sim.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}
void callNumber() {
  sim.print (F("ATD"));
  sim.print (number);
  sim.print (F(";\r\n"));
  _buffer = _readSerial();
  Serial.println(_buffer);
}
