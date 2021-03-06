#include <Scheduler.h> //This is a custom library, download from https://github.com/mikaelpatel/Arduino-Scheduler#:~:text=Install,Arduino%20IDE%20File>Examples%20menu. 
#include<math.h>
#include<ThingSpeak.h>
#include<WiFi.h>
#include<WiFiUdp.h>
#include<SPI.h>
#include<SD.h>


#define capacity 4000 //capacity at time of manufacture in mAh convert this to coulomb for SoH calculation
#define V1 A0
#define V2 A1
#define V3 A3 //Keep going up to Vn for 'n' no. of cells in series
#define balancePin1 10
#define balancePin2 11
#define balancePin3 12 
#define pwr 13
#define baud 115200
#define Time 50e-3 //sampling time for coulomb counting
#define PinCS 10 //chip selection pin for SD card
#define MOSI  11 //connect SD card to these pins only
#define MISO  12
#define SCK   13
#define network_name "NAME"
#define network_password "PASSWORD"
#define channel_id "CHANNEL NUMBER"
#define API_Key "WRITE API KEY"

float SOC;
float voltage[3]; // Size of array should be exactly the same as number of cells in series
float v;
float charge;
float current;
float SOH;
float new_capacity = SOH*capacity/100;
WiFiClient client;
String myStatus = "";
unsigned char data[7] = {voltage[0], voltage[1], voltage[2],current, charge, SOC, SOH};

void SD_Setup()
{
  Serial.begin(baud);
  pinMode(PinCS, OUTPUT);
  if (SD.begin(PinCS))
  {
    Serial.println("SD Card is ready to use.");
  }
  else
  {
    Serial.println("SD Card initialisation failed");
    return;
  }
}
void Store_Data()
{
  File Parameters = SD.open("Parameters.txt", FILE_WRITE);
  if(Parameters)
  {
    Serial.println("Writing to file...");
    Parameters.println(data[7], DEC);
    Parameters.close();
    Serial.println("Done");
  }
}
void ThingSpeak_Setup()
{
  Serial.begin(baud);
  if(WiFi.status() == WL_NO_SHIELD)
  {
    Serial.println("Communication with module failed!");
    //terminate
    while(1);
  }
  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0")
  {
  Serial.println("Put version 1.1.0 only");
  }
  ThingSpeak.begin(client);
}

void Send_Data()
{
  ThingSpeak.setField(1, SOC); 
  ThingSpeak.setField(2, SOH);
  ThingSpeak.setField(3, current);
  ThingSpeak.setField(4, charge);
  ThingSpeak.setField(5, new_capacity);
  ThingSpeak.setField(6, voltage[0]);
  ThingSpeak.setField(7, voltage[1]);
  ThingSpeak.setField(8, voltage[3]);
}
void cc() //coulomb counting (essential for SOH monitoring)
 {
  charge = current*Time;
  delay(50);
  if(charge<=capacity*3600) //Let's use recursion for the fun of it :P
  {
    cc();
  }
 }
void SoH()
{
  if(v==4.2)
  {
    SOH = (charge/(capacity*3600))*100; //SOH is defined as remaining capacity/capacity at the time of manufacture
  }
}
void SoC()
{
  voltage[0] = (analogRead(V1) / 1024) * 5;
  voltage[1] = (analogRead(V2) / 1024) * 5;
  voltage[2] = (analogRead(V3) / 1024) * 5;
  v = min(voltage[0], voltage[1]);
  v = min (v, voltage[2]);
  SOC = exp((v - 2.9545) / 0.268); //SoC of entire series string will be same as SoC of cell with minimum voltage in it
}
void balance() //passive balancing
{ if (voltage[0] == voltage [1] && voltage[1] == voltage[2])
  {
      digitalWrite(pwr, HIGH);
      digitalWrite(balancePin1, HIGH);
      digitalWrite(balancePin2, HIGH);
      digitalWrite(balancePin3, HIGH);
    }
    else
  {
    while (voltage[0] == v)
    {
      digitalWrite(pwr, LOW);
      digitalWrite(balancePin1, LOW);
      digitalWrite(balancePin2, HIGH);
      digitalWrite(balancePin3, HIGH);
    }
    while (voltage[1] == v)
    {
      digitalWrite(pwr, LOW);
      digitalWrite(balancePin1, HIGH);
      digitalWrite(balancePin2, LOW);
      digitalWrite(balancePin3, HIGH);
    }
    while (voltage[2] == v)
    {
      digitalWrite(pwr, LOW);
      digitalWrite(balancePin1, HIGH);
      digitalWrite(balancePin2, HIGH);
      digitalWrite(balancePin3, LOW);
    }
  }
}
void task1() //we want all functions to be running at once but the Scheduler.start function takes only 2 arguments at once hence the task1, task2 functions
{
  Scheduler.start(SoC,balance); //Scheduler function allows us to enable scheduling in arduino i.e running more than one function at once
}
void task2()
{
  Scheduler.start(cc,SoH);
}

void setup() {

  pinMode(V1, INPUT);
  pinMode(V2, INPUT);
  pinMode(V3, INPUT);
  pinMode(balancePin1, OUTPUT);
  pinMode(balancePin2, OUTPUT);
  pinMode(balancePin3, OUTPUT);
  pinMode(pwr, OUTPUT);
  ThingSpeak_Setup();
  SD_Setup();
}

void loop()
{
  Scheduler.start(task1,task2);  //we want all functions to be running at once but the Scheduler.start function takes only 2 arguments at once hence the task1, task2 functions
  delay(50);
  Scheduler.start(Store_Data,Send_Data);
}
