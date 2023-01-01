#include <SoftwareSerial.h>
 #include <LiquidCrystal_I2C.h>
//I2C pins declaration
LiquidCrystal_I2C lcd(0x27,20,4);
SoftwareSerial mySerial(9, 10);
 
int gasValue = A0; // smoke / gas sensor connected with analog pin A1 of the arduino / mega.
int data = 0;
int buzzer = 10;
int G_led = 8; // choose the pin for the Green LED
int R_led = 9; // choose the pin for the Red Led


 
void setup()
{
  lcd.begin(16,2);//Defining 16 columns and 2 rows of lcd display
  lcd.backlight();
  pinMode(buzzer,OUTPUT);
  pinMode(R_led,OUTPUT); // declare Red LED as output
  pinMode(G_led,OUTPUT); // declare Green LED as output
randomSeed(analogRead(0));
mySerial.begin(9600); // Setting the baud rate of GSM Module
Serial.begin(9600); // Setting the baud rate of Serial Monitor (Arduino)
lcd.begin(16,2);
pinMode(gasValue, INPUT);
lcd.print ("    car gas ");
lcd.setCursor(0,1);
lcd.print (" control system ");
delay(3000);
lcd.clear();
}
 
void loop()
{
 
data = analogRead(gasValue);
Serial.print("Gas Level: ");
Serial.println(data);
lcd.print ("car gas control");
lcd.setCursor(0,1);
lcd.print("Gas Level: ");
lcd.print(data);
delay(1000);
 
if ( data > 16) //
{
  digitalWrite(buzzer, HIGH);
  digitalWrite(R_led, HIGH); // Turn LED on.
  digitalWrite(G_led, LOW); // Turn LED off.
SendMessage();
Serial.print("Gas detect alarm");
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Gas Level Exceed");
lcd.setCursor(0,1);
lcd.print("car stopped");
delay(5000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Go to garage for"); 
lcd.setCursor(0,1);
lcd.print("car maintenance");
delay(3000);
}
else
{
  digitalWrite(buzzer, LOW);
  digitalWrite(R_led, LOW); // Turn LED off.
  digitalWrite(G_led, HIGH); // Turn LED on.
Serial.print("Gas Level Low");
delay(1000);
}
 
lcd.clear();
}
 
void SendMessage()
{
Serial.println("I am in send");
mySerial.println("AT+CMGF=1"); //Sets the GSM Module in Text Mode
delay(1000); // Delay of 1000 milli seconds or 1 second
mySerial.println("AT+CMGS=\"+250789719600\"\r"); // Replace x with mobile number
delay(1000);
mySerial.println("Excess Gas Detected.");// The SMS text you want to send
mySerial.println(data);
delay(100);
mySerial.println((char)26);// ASCII code of CTRL+Z
delay(1000);
}
