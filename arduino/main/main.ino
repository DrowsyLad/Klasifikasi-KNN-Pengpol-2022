// #include "HX711.h"
#include <LiquidCrystal_I2C.h>
#include <BluetoothSerial.h>
// #include <Servo.h>


//Pin Define
#define S0_CS 14
#define S1_CS 12
#define S2_CS 26
#define S3_CS 25
#define OUT_CS 27

#define echoPin 35
#define trigPin 32

// Class object init
LiquidCrystal_I2C lcd(0x27, 16, 2);
BluetoothSerial SerialBT;

//Calibration State
int redMin = 76;
int greenMin = 86;
int blueMin = 55;
int redMax = 320;
int greenMax = 348;
int blueMax = 233;

//Variables for Color Pulse Width Measurements
int redPW = 0;
int greenPW = 0;
int bluePW = 0;

// int redValue = 0;
// int greenValue = 0;
// int blueValue = 0;

byte average = 0;
char output;
int i = 0;

byte state = 0;
static unsigned long t_awal;
static unsigned long t_setelah;
static unsigned long treshold = 2000;

void reset_all(){
  lcd.clear();
  lcd.setCursor(0,0);
  i = 0;
}

char receive_data(){
  String x;
  char jenis;
  while(!Serial.available());
  x = Serial.readStringUntil('\n');
  (x == "s") ? jenis = 'S' : jenis = 'A';
  return jenis;
}

void readRGB(float* redValue, float* blueValue, float* greenValue){
  int loop_count = 5;
  //Color Sensor
  int redMin = 76;
  int greenMin = 86;
  int blueMin = 55;
  int redMax = 320;
  int greenMax = 348;
  int blueMax = 233;
//  int redMin = 0;
//  int greenMin = 0;
//  int blueMin = 0;
//  int redMax = 310;
//  int greenMax = 240;
//  int blueMax = 360;
  for (int i = 0; i < loop_count; i++)
  {
    int redPW = getRedPW();
    *redValue = 
                redPW;
//                map(redPW, redMin, redMax, 100, 0);
    if(*redValue < 0){
      *redValue = 0;
    }
    delay(500);

    // Read Green Pulse Width
    int greenPW = getGreenPW();
    *greenValue = 
                  greenPW;
//                  map(greenPW, greenMin, greenMax, 100, 0);
    if(*greenValue < 0){
      *greenValue = 0;
    }
    delay(500);

    // Read Blue Pulse Width
    int bluePW = getBluePW();
    *blueValue = 
                  bluePW;
//                  map(bluePW, blueMin, blueMax, 100, 0);
    if(*blueValue < 0){
      *blueValue = 0;
    }
    delay(500);  

    *redValue += *redValue;
    *greenValue += *greenValue;
    *blueValue += *blueValue;
  }

  *redValue /= loop_count;
  *greenValue /= loop_count;
  *blueValue /= loop_count;

}

float readSize(){
  long duration; // variable for the duration of sound wave travel
  float distance; // variable for the distance measurement
  float max_size = 0, size; //size of object
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
//  Serial.print("Distance: ");
//  Serial.print(distance);
//  Serial.println(" cm");

  // size = round(max_size - distance);

  return distance;
}

void setup_LCD(){
//  lcd.begin(16,2);
  lcd.init();
  lcd.backlight();
}

void setup_colorSensor(){
  // Set S0 - S3 as outputs
  pinMode(S0_CS, OUTPUT);
  pinMode(S1_CS, OUTPUT);
  pinMode(S2_CS, OUTPUT);
  pinMode(S3_CS, OUTPUT);
  // Set Pulse Width scaling to 20%
  digitalWrite(S0_CS,HIGH);
  digitalWrite(S1_CS,LOW);
  // Set Sensor output as input
  pinMode(OUT_CS, INPUT);
}

void setup_ultrasonicSensor(){
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
}

int getGreenPW() {
  // Set sensor to read Green only
  digitalWrite(S2_CS,HIGH);
  digitalWrite(S3_CS,HIGH);
  int PW;
  PW = pulseIn(OUT_CS, LOW);
  return PW;
}

int getBluePW() {
  // Set sensor to read Blue only
  digitalWrite(S2_CS,LOW);
  digitalWrite(S3_CS,HIGH);
  int PW;
  PW = pulseIn(OUT_CS, LOW);
  return PW;
}

int getRedPW() {
  // Set sensor to read Red only
  digitalWrite(S2_CS,LOW);
  digitalWrite(S3_CS,LOW);
  int PW;
  PW = pulseIn(OUT_CS, LOW);
  return PW;
}

void printLines(String line1, String line2 = ""){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}

void send_data(int size, int red, int green, int blue){
  delay(500);
  // Serial.println("s");
  delay(50);
  String data = "";
  data = String(size) + "," + String(red);// + "," + String(green) + "," + String(blue);
  delay(500);
  Serial.println(data);
  printLines("Waiting for", "server reply..");
  while(!(Serial.read() == 'f'));
}

bool receiveCommand(String* command){
  if(SerialBT.available()){
    *command = SerialBT.readString();
    return true;
  }
  else return false;
}

void ambilDataset(){
  String command = "None";
  printLines("Place your", "object inside");
  delay(1000);
  // for(int i = 5; i >= 0; i--){
  //   printLines("Starting in " + String(i));
  //   delay(1000);
  // }
  printLines("Press button", "to start!");
  while(command != "Start"){
    receiveCommand(&command);
    delay(100);
  }
  float size, red, green, blue;
  printLines("Reading size..");
  size = readSize();
  delay(1000);
  printLines("Reading colors..");
  readRGB(&red, &green, &blue);
  printLines("Sending data", "to server..");
  send_data(size, red, green, blue);
  delay(500);
  printLines("Data accepted", "by server");
  delay(1000);
  printLines("Restarting..");
  delay(1000);
}

void mainProcess(){
  String command = "None";
  printLines("Place your", "object inside");
  delay(2000);
  printLines("Press button", "to start!");
  while(command != "Start"){
    receiveCommand(&command);
    delay(50);
  }
  command = "None";
  float size, red, green, blue;
  printLines("Reading size..");
  delay(1000);
  size = readSize();
  printLines("Reading colors..");
  readRGB(&red, &green, &blue);
  printLines("Sending data", "to server..");
  send_data(size, red, green, blue);
  delay(500);
  printLines("Data accepted", "by server");
  char outputKNN = receive_data();
  String kelasKNN;
  if(outputKNN == 'S') kelasKNN = "Stroberi";
  else kelasKNN = "Apel Batu";
  printLines("Hasil klasifikasi:", kelasKNN);
  delay(5000);
  printLines("Press button", "to restart");
  while(command != "Start"){
    receiveCommand(&command);
    delay(50);
  }
  printLines("Restarting..");
  delay(1000);
}

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(1);
  setup_LCD();
  lcd.setCursor(0,0);
  printLines("LCD OK...");
  delay(1000);
  SerialBT.begin("ESP32_Pengpol");
  printLines("Bluetooth device", "initialized..");
  delay(1000);
  while(!Serial.available()) {
    // Serial.println("Waiting to start...");
    printLines("Connecting to", "Server..");
    delay(1000);
  }
  lcd.clear();
  lcd.print("Setting Up...");
  // Serial.println("Setting up...");
  delay(1000);
  setup_ultrasonicSensor();
  printLines("Ultrasonic Sensor", "OK...");
  delay(1000);
  setup_colorSensor();
  printLines("Color Sensor", "OK...");
  delay(1000);
  t_awal = millis();
  lcd.clear();
  lcd.print("All set, Ready!");
  delay(2000);
  lcd.clear();
}

void loop(){
  // FSM();
//  ambilDataset();
  mainProcess();
}
