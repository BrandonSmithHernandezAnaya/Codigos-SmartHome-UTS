  /* 
  OSOYOO SMART HOME SYSTEM
 
  */
#define BLYNK_PRINT Serial

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <RFID.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>


RFID rfid(48,49);   //SDA,RST
bool state=true;
LiquidCrystal_I2C lcd(0x27,16,2);
int servoPin = 9; 
#define DHTPIN 5         

#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);
int fanmodule = 4 ;
int distance, initialDistance, currentDistance, i;                                                            
#define echoPin 26
#define trigPin 25
#define BUTTON 10
int BUTTON1 = 3;
int switchState = 0; 
int oldSwitchState = 0; 
int lightsOn = 0; // is the switch on = 1 or off = 0
#define LED   2 
#define buzzer  7 
int ButtonState;
int ButtonLastState;
int ButtonCounter;

int pir_hc_sr501 = 6;          
int pirState = LOW;             
int pirval = 0;                    
int firePin = A1;   
int fireValue = 0;   
int LED1 = 8;
int lightPin = A0;   
int lightValue = 0;
int redPin = 24;
int greenPin = 23;
int bluePin = 22;

BlynkTimer timer; 



//char auth[] = "aNj-iDjQQ5Bk3SXwSnyQrCWMuOOY2qrW";
char auth[] = "hTvNLgEInvzrpUm2CLHl6pvDXmp3XokB";

char ssid[] = "HOME-2AF2";
char pass[] = "0C09B2548C0102D8";


#include <SoftwareSerial.h>
SoftwareSerial EspSerial(A9, A8); // RX, TX

ESP8266 wifi(&EspSerial);
//***************************RGB**********************************************************************************************

BLYNK_WRITE(V7)
{

    // get a RED channel value
    int r = param[0].asInt();
    // get a GREEN channel value
    int g = param[1].asInt();
    // get a BLUE channel value
    int b = param[2].asInt();
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);

}


//***************************led Sensor************************************************************************************

void LIGHTSENSOR () {
    Serial.print("Light value：");
    lightValue = analogRead(lightPin);
    Serial.println(lightValue);

  if (lightValue > 900) 
  {
    Serial.println("White Light ON!");
    digitalWrite(LED1, HIGH);
  } 
  else 
  {
    digitalWrite(LED1, LOW);
}
}
//***************************Sensor de incendio************************************************************************************

void fireSensor(){
  fireValue = analogRead(firePin);
  Serial.print("Fire Value: ");
  Serial.println(fireValue);
  if (fireValue < 700) {
    Serial.println("Fire Alarm ON!");
    Blynk.email("ivanfernando117@gmail.com", "Fire!Fire!Fire!", "Detect Fire at home!");
   // Blynk.email("susan@vership.com", "Fire!Fire!Fire!", "Detect Fire at home!");
    Blynk.notify("Fire!Fire!Fire!");
    digitalWrite(LED, HIGH);
    digitalWrite(buzzer, HIGH);

  }
else
{
    digitalWrite(LED, LOW);
    digitalWrite(buzzer, LOW);
}
}
//***************************sensor de movimiento PIR************************************************************************************

void PIRSENSOR () {
    Serial.println("PIR SENSOR START!");
    pirval = digitalRead(pir_hc_sr501);  
    if (pirval == HIGH) {            
    digitalWrite(buzzer, HIGH);
    if (pirState == LOW) {
    
    Serial.println("Motion detected!");
    
    pirState = HIGH;
    Blynk.notify("Someone broke in!");
    //Blynk.email("your email address", "Someone broke in!", "Motion detected!");
    }
  } 
  else 
  {
    if (pirState == HIGH){
    
    digitalWrite(buzzer, LOW);
    Serial.println("Motion ended!");
    
    pirState = LOW;
    }
  }
}


//***************************sensor de ULTRASONIDO************************************************************************************

long getDistance () {
  Serial.println("HC-SR04 START!");
  float duration, distance;
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) * 0.0344;
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println(" cm");

 
  return distance;
}
//***************************entrada de mensaje ************************************************************************************

BLYNK_WRITE(V10)
{
  String i = param.asStr(); 
        Serial.println(i);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(i); 
    delay(5000); 
}

//***************************sensor de humedad y temperatura DHT11**********************************************************************
void DHTSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    Blynk.virtualWrite(V5, "NO DHT11!");
    Blynk.virtualWrite(V6, "NO DHT11!");

    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("DHT11 Failed!"); 
    delay(1000); 

  }
  else 
  {
   Serial.print("Humidity: "); 
   Serial.print(h);
   Serial.print(" %\t");
   Serial.print("Temperature: "); 
   Serial.print(t);
   Serial.println(" *C");
   lcd.clear();
   lcd.setCursor(0,0); 
   lcd.print("Hum:  "); 
   lcd.print(h); 
   lcd.print("%"); 
   
   lcd.setCursor(0,1); 
   lcd.print("Temp: "); 
   lcd.print(t); 
   lcd.print("C"); 
  }

  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
  if (t > 30) {
    Blynk.notify("The temperature is higher than 30°C! The fan is on!");
    digitalWrite(fanmodule,HIGH);
  }
  else
  {
    digitalWrite(fanmodule,LOW);

  }
}


//***************************motor de puerta SERVO****************************************************************************************

BLYNK_WRITE(V8)
{
  int pinValue = param.asInt();
  //switchState = pinValue; // read the pushButton State

 if (pinValue == HIGH)
 {
  for(int i=0;i<50;i++){
    int pulsewidth = (90 * 11) + 500;
    digitalWrite(servoPin, HIGH); 
    delayMicroseconds(pulsewidth); 
    digitalWrite(servoPin, LOW); 
    delayMicroseconds(20000 - pulsewidth);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hello, guys!");
  delay(1000); 

  lcd.clear(); 
 } 
 else if(pinValue == LOW)
 {
  for(int i=0;i<50;i++){
    int pulsewidth = (0 * 11) + 500;
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(pulsewidth);
    digitalWrite(servoPin, LOW); 
    delayMicroseconds(20000 - pulsewidth);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Close the door!"); 
  delay(1000); 
  lcd.clear(); 

 }
}

//***************************** mensaje SOS y boton  ************************************************************************************
void LEDstringcontrol()
{
  switchState = digitalRead(BUTTON1); 
 if (switchState != oldSwitchState) 
 {
 oldSwitchState = switchState;
 if (switchState == HIGH)
 {
 // toggle
 lightsOn = !lightsOn;
 }
 }
 if(lightsOn)
 {
  Serial.println("Press the SOS button for emergency help...");
 } 
 else 
 {
    Serial.println("Press the SOS button to cancel the alarm...");
    Blynk.email("ivanfernando117@gmail.com", "SOS!", "SOS!");
    // Blynk.email("your email address", "SOS!", "SOS!");
    Blynk.notify("SOS! Press the SOS button to cancel the alarm...");
 }
}



//*****************************sensor de radio frecuencia rfid y boton **********************************************************************************

void RFIDSENSOR() {
    Serial.println("RFID START!");
    unsigned char type[MAX_LEN];
    
    if (rfid.isCard( )) {
      Serial.println("Find the card!"); 
      
      ShowCardType(type);
      
      if (rfid.readCardSerial()) {
      Serial.print("The card's number is  : ");
      Serial.print(rfid.serNum[0],HEX);
      Serial.print(rfid.serNum[1],HEX);
      Serial.print(rfid.serNum[2],HEX);
      Serial.print(rfid.serNum[3],HEX);
      Serial.print(rfid.serNum[4],HEX);
      Serial.println(" ");
      ShowUser(rfid.serNum);
    }
    
    Serial.println(rfid.selectTag(rfid.serNum));
  }
  rfid.halt();
}
void ShowCardType( unsigned char* type)
{
  Serial.print("Card type: ");
  if(type[0]==0x04&&type[1]==0x00) 
    Serial.println("MFOne-S50");
  else if(type[0]==0x02&&type[1]==0x00)
    Serial.println("MFOne-S70");
  else if(type[0]==0x44&&type[1]==0x00)
    Serial.println("MF-UltraLight");
  else if(type[0]==0x08&&type[1]==0x00)
    Serial.println("MF-Pro");
  else if(type[0]==0x44&&type[1]==0x03)
    Serial.println("MF Desire");
  else
    Serial.println("Unknown");
}

void ShowUser( unsigned char* id)
{
  
  if( id[0]==0x71 && id[1]==0xBA && id[2]==0x2F && id[3]==0x2E ) {   
   Serial.println("Hello Mary!");
   Blynk.notify("Mary is home!");
   lcd.clear();
   lcd.setCursor(0,0); 
   lcd.print("HELLO Marry!"); 
   delay(1000); 
   lcd.clear(); 
   lcd.setCursor(0,1); 
   lcd.print("Welcome home!"); 
  

   for(int i=0;i<50;i++){
    int pulsewidth = (90 * 11) + 500;
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(pulsewidth); 
    digitalWrite(servoPin, LOW);
    delayMicroseconds(20000 - pulsewidth);
  }
    
   delay(3000);
  for(int i=0;i<50;i++){
    int pulsewidth = (0 * 11) + 500;
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(pulsewidth); 
    digitalWrite(servoPin, LOW); 
    delayMicroseconds(20000 - pulsewidth);
  }
  lcd.clear(); 

} 
  
  
else if(id[0]==0x2E && id[1]==0x7D && id[2]==0xDB && id[3]==0x2B) {
     Serial.println("Hello David!");
     Blynk.notify("David is home!");
     lcd.clear();
     lcd.setCursor(0,0); 
     lcd.print("HELLO David!"); 
     delay(1000); 
     lcd.setCursor(0,1); 
     lcd.print("Welcome home!"); 
     



   for(int i=0;i<50;i++)
   {
    int pulsewidth = (90 * 11) + 500; 
    digitalWrite(servoPin, HIGH);
    delayMicroseconds(pulsewidth); 
    digitalWrite(servoPin, LOW); 
    delayMicroseconds(20000 - pulsewidth);
   }
    Serial.println("Open th door");
    delay(3000);
    for(int i=0;i<50;i++)
    {
     int pulsewidth = (0 * 11) + 500; 
     digitalWrite(servoPin, HIGH); 
     delayMicroseconds(pulsewidth);
     digitalWrite(servoPin, LOW); 
     delayMicroseconds(20000 - pulsewidth);
    }
    lcd.clear(); 
  }
  else
  {
    Blynk.notify("Unidentified user is ready to open the door!");
    Serial.println("Hello unkown guy!");
    lcd.clear();
    lcd.setCursor(0,0); 
    lcd.print("Unknow user!"); 
    delay(2000); 
    lcd.clear(); 
 } 
}

void setup()
{
  pinMode(fanmodule, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(BUTTON, INPUT); 
  pinMode(BUTTON1, INPUT); 

  
  Serial.begin(9600);
  SPI.begin(); 
  dht.begin();

  pinMode(servoPin, OUTPUT); 
  lcd.init(); 
  lcd.backlight(); 
  rfid.init();
  while (!Serial) {
    ; 
  }
  Serial.println("start");
  
  EspSerial.begin(115200);
  EspSerial.write("AT+UART_DEF=9600,8,1,0,0\r\n");
  EspSerial.write("AT+RST\r\n");
  delay(100);
  EspSerial.begin(9600);

  Blynk.begin(auth, wifi, ssid, pass);
  
  //Blynk.begin(auth, wifi, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, wifi, ssid, pass, IPAddress(192,168,1,100), 8080);
  timer.setInterval(1000, RFIDSENSOR);
  timer.setInterval(1000L, DHTSensor);
  timer.setInterval(2500L, PIRSENSOR);
  timer.setInterval(1000L, fireSensor);
  timer.setInterval(1000L, LIGHTSENSOR);
  timer.setInterval(1000L, LEDstringcontrol);


}

void loop()
{
  Blynk.run();
  timer.run();

ButtonState = digitalRead(BUTTON);

  if(ButtonState != ButtonLastState)
  {
    if(ButtonState)
    {
       
        Serial.println("Door outside alarm off");
        ButtonCounter++;
 lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Alarm Stop!"); 
lcd.setCursor(0, 1);
lcd.print("Press to start!");  
    }
    else
    {
      Serial.println("Door outside alarm on");  

      

    }
    delay(100);
  }

  ButtonLastState = ButtonState;
  if(ButtonCounter%2)
  {
  return;

  
  }
  else
  {
    initialDistance = getDistance();
currentDistance = getDistance() + 15;
  lcd.clear();
lcd.setCursor(0, 0);
lcd.print("Alarm Working!"); 
lcd.setCursor(0, 1);
lcd.print("Press to stop!");  
if ( currentDistance < 200) {
   
    Serial.println("Sth at the door!");
    Blynk.email("ivanfernando117@gmail.com",  "Sth at the door!", "Warning!");
    //Blynk.email("821531489@qq.com", "Sth at the door!", "Warning!");
    Blynk.notify("Sth at the door!");
    digitalWrite (LED,HIGH);
    digitalWrite (buzzer,HIGH);
    delay(3000);

  }
 
  else
  {
    digitalWrite (LED,LOW);
    digitalWrite (buzzer,LOW);
  }
 }
}
