#define BLYNK_TEMPLATE_ID "TMPLYwSD9WEZ"
#define BLYNK_DEVICE_NAME "Smart Seeding"
#define BLYNK_AUTH_TOKEN "oh3lV75Xwdj2ttkmLlYMT_-1s7tKs1kF"

#include <DallasTemperature.h>
#include <OneWire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial
#define ONE_WIRE_BUS D5 
#define DHTPIN D4 
#define DHTTYPE DHT11
#define pump D6
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); 
LiquidCrystal_I2C lcd(0x27, 16, 2);  
const int AirValue = 620;   
const int WaterValue = 310;  
int soilMoistureValue = 0;
int soilmoist=0;  
int humi, temp, fp, sistem;
int buttonState;

// Nilai set point
int SP_LOW=30;
int SP_HIGH=60;

// Token Blynk
char auth[] = "oh3lV75Xwdj2ttkmLlYMT_-1s7tKs1kF";
// Jaringan WIFI
char ssid[] = "Hospot";  // Nama Hotspot/WiFi
char pass[] = "rilham123";  //  Password 
//==========================================
BLYNK_WRITE(V4){ // Mengatur sistem otomatis atau tidak
  buttonState = param.asInt();
  if(buttonState==HIGH){
    sistem=1;//automatic
    fp=0;
    lcd.setCursor(12,1);
    lcd.print("Aut");
    delay(10);
  }
  else if(buttonState==LOW){
    sistem=0;//manual
    lcd.setCursor(12,1);
    lcd.print("Man");
    delay(10);    
  }
}
//==========================================
BLYNK_WRITE(V5){ // Mengatur aktivasi pump
  buttonState = param.asInt();
    if(buttonState==LOW){
      digitalWrite(pump,HIGH); 
      delay(10);
      }
    else if(buttonState==HIGH){
      digitalWrite(pump,LOW); 
      delay(10);    
      }
}
//=====================================
BLYNK_WRITE(V6){ // Mengatur backlight LCD
  buttonState = param.asInt();
  if(buttonState==HIGH){
  lcd.backlight();
  Serial.print("LCD Backlight :");
  Serial.println("ON");
  delay(10);
  }
  else if(buttonState==LOW){
  lcd.noBacklight();
  Serial.print("LCD Backlight :");
  Serial.println("OFF");
  delay(10);    
  }
}
//=====================================
void setup(void)
{
  lcd.begin(16,2);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.print("  Smart Seeding  ");
  lcd.setCursor(0, 1);
  lcd.print("  by SISTEL2020  ");
  Serial.begin(9600); 
  sensors.begin();
  dht.begin();   
  pinMode(pump,OUTPUT);
  digitalWrite(pump,LOW);
  delay(1200);
  digitalWrite(pump,HIGH);
  delay(1200);
  digitalWrite(pump,LOW);
  delay(1200);
  digitalWrite(pump,HIGH);
  delay(1200);
  
  Blynk.begin(auth, ssid, pass);   
  delay(1500);
  lcd.clear();
  lcd.print("Mst=   %, T=   C");    
  lcd.setCursor(0, 1);
  lcd.print("Hum=   %, M=Man");  
}
//=====================================
void loop(void)
{ 
  Blynk.run(); 
  sensors.requestTemperatures();  
  temp=sensors.getTempCByIndex(0);  
    if(temp >= 100)
    {
      temp=100;
    }
  else if(temp <=0)
    {
      temp=0 ;
    }             
  Serial.print("Temp :");
  Serial.print(temp);
  Serial.println("°");
  lcd.setCursor(12,0);
  lcd.print(temp);  
  soilMoistureValue = analogRead(A0);  //put Sensor insert into soil
  soilmoist= map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  if(soilmoist >= 100)
    {
      soilmoist=100;
    }
  else if(soilmoist <=0)
    {
      soilmoist=0;
    }
  Serial.print("Soil Moisture :");
  Serial.print(soilmoist);
  Serial.println("%");
  lcd.setCursor(4,0);
  lcd.print(soilmoist); 
  lcd.print(" "); 
  humi = dht.readHumidity();
  if(humi >= 100)
    {
      humi=100;
    }
  else if(humi <=0)
    {
      humi=0 ;
    }
  if (isnan(humi) ) {
    Serial.println("DHT11 tidak terbaca... !");
    return;
  }
  else{  
    Serial.print("Humi:");
    Serial.print(humi);  
    Serial.println("%"); 
    lcd.setCursor(4,1);
    lcd.print(humi);
  }
  // Sistem Otomatis
  if(sistem==1){
  if((soilmoist<SP_LOW)&&(fp==0)){
    digitalWrite(pump,LOW);
    fp=1;
  }
  else if((soilmoist>SP_HIGH)&&(fp==1)){
    digitalWrite(pump,HIGH);
    fp=0;
  } 
  }
  
  Blynk.virtualWrite(V1, temp);
  Blynk.virtualWrite(V2, soilmoist);
  Blynk.virtualWrite(V3, humi);

  Serial.print("Sistem=");
  Serial.println(sistem);
  Serial.print("fp=");
  Serial.println(fp);
  
  delay(1500);
}
