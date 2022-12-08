// #include "HX711.h"
#include <LiquidCrystal_I2C.h>
#include <BluetoothSerial.h>
// #include <Servo.h>

#define LOADCELL
// #define SERVO
#define LCD
#define ULTRASONIC

//Pin Define
#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  4
#define SERVO_ATAS 12
#define SERVO_BAWAH 2
#define S0_CS 14
#define S1_CS 12
#define S2_CS 26
#define S3_CS 25
#define OUT_CS 27

#define echoPin 35
#define trigPin 32

// Class object init
// HX711 scale;
LiquidCrystal_I2C lcd(0x27, 16, 2);
BluetoothSerial SerialBT;

//Calibration State
//Load Cell
float calibration_factor = 360;
//Color Sensor
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

void send_data(int size, int red, int green, int blue){
  delay(500);
//  Serial.println('s');
  String data = "";
  data = (String(size) + "," + String(red) + "," + String(green) + "," + String(blue));
  delay(500);
  Serial.println(data);
//  while(!(Serial.read() == 'f'));
}

void reset_all(){
  lcd.clear();
  lcd.setCursor(0,0);
//  redValue = 0;
//  greenValue = 0;
//  blueValue = 0;
//  myServo_bot.write(90);
//  myServo_top.write(00);
  i = 0;
}

// void process_bottom_servo(char output){   
//   delay(500);
//   (output == 'S') ? myServo_bot.write(90 - 70) : myServo_bot.write(90 + 70);
//   delay(500);
// }

// void servo_init(){
//   myServo_bot.attach(2);
//   myServo_top.attach(12);
//   myServo_top.write(00);
//   myServo_bot.write(90);
// }

//void process_top_servo(){
//  delay(500);
//  myServo_top.write(150);
//  delay(500);
//  myServo_top.write(00);
//}

char receive_data(){
  String x;
  char jenis;
  while(!Serial.available());
  x = Serial.readStringUntil('\n');
  (x == "s") ? jenis = 'S' : jenis = 'K';
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

// int cekWeight(){
//   int berat = 0;
//   scale.set_scale(calibration_factor);
//   berat = scale.get_units();
//   return berat;
// }

// int readWeight(){
//   average = 0;
//   scale.set_scale(calibration_factor);
//   for(int i = 0; i < 5; i++){
//     average += scale.get_units();
//   }
//   average /= 5;
//   return average;
// }

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

void setup_loadCell(){
  // #ifdef LOADCELL
  // scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  // scale.set_scale();
  // scale.tare(); //Reset the scale to 0
  // long zero_factor = scale.read_average(); //Get a baseline reading
  // Serial.println("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  // Serial.println(zero_factor);
  // #endif
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
/*
void FSM(){
  t_setelah = millis() - t_awal;
  float ultrasonic_output = 0, redValue = 0, greenValue = 0, blueValue = 0;
  switch(state){
    case 0:
      if(i == 0){
        lcd.print("Put object on");
        lcd.setCursor(0,1);
        lcd.print("top of Load Cell");
        i++;
      }
      Serial.println("Put object on top of load cell");
      if(cekWeight() > 2 && t_setelah > treshold * 4){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Dont move object");
        lcd.setCursor(0,1);
        lcd.print("Processing...");
        state = 1;
        t_awal = millis();   
      }
      break;

    case 1:      
      if(t_setelah > treshold && cekWeight() > 2){
        
        lcd.clear();
        lcd.setCursor(1,0);
        lcd.print("Reading");
        delay(500);
        lcd.print(".");
        delay(500);
        lcd.print(".");
        delay(500);
        lcd.print(".");
        delay(500);
        lcd.setCursor(1,1);
        lcd.print("Size: ");
        lcd.print(readSize());
        delay(500);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Weight:");
        Serial.println("Begin reading weight...");
        Serial.println("Reading weight...");
        int w = readWeight();
        Serial.print("Weight: ");
        Serial.print(String(w) + "gram");
        lcd.print(String(w) + " Gram");
        delay(2000);
        state = 2;
        t_awal = millis();
      }
      break;

    case 2:
      if(t_setelah > treshold && cekWeight() > 2){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Color:");
        Serial.println("Begin reading color...");
        Serial.println("Reading color...\n");
        Serial.print("Color:\n");
        readRGB(&redValue, &greenValue, &blueValue);
        lcd.setCursor(0,1);
        lcd.print("R:");
        lcd.print(String(redValue));
        delay(1000);
        lcd.print(",G:");
        lcd.print(String(greenValue));
        delay(1000);
        lcd.print(",B:");
        lcd.print(String(blueValue));
        delay(3000);
        state = 3;
        t_awal = millis();
      }
      break;

    case 3:
      if(t_setelah > treshold){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Begin Classifica-");
        lcd.setCursor(0,1);
        lcd.print("tion...");
        Serial.println("Begin classification...");
        send_data(redValue, ultrasonic_output);
        output = receive_data();
        if(output == 'S'){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Jeruk");
          lcd.setCursor(0,1);
          lcd.print("Santang Madu");
        } else {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Jeruk");
          lcd.setCursor(0,1);
          lcd.print("Kunci");
        }
        state = 4;
        t_awal = millis();
      }

    case 4:
      if((t_setelah > treshold) && (output == 'K' || output == 'S')){
        Serial.print("Servo Bottom move...");
        process_bottom_servo(output);
        state = 5;
        t_awal = millis();
      }

    case 5:
      if(t_setelah > treshold){
        Serial.print("Servo Top move...");
        process_top_servo();
        state = 6;
        t_awal = millis();
      }

    case 6:
      if(t_setelah > treshold * 2){
        Serial.print("End of process...");
        if(cekWeight() < 3){
          reset_all();
          state = 0;
          t_awal = millis();
        }
      }
      break;
    }
  } 
*/

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(1);
  // while(!Serial.available()) {
  //   Serial.println("waiting to start...");
  //   delay(1000);
  // }
  setup_LCD();
  lcd.setCursor(0,0);
  Serial.println("LCD OK...");
  delay(1000);
  SerialBT.begin("ESP32_Pengpol");
  printLines("Bluetooth device", "initialized..");
  delay(1000);
  lcd.clear();
  lcd.print("Setting Up...");
  Serial.println("Setting up...");
  delay(1000);
  // setup_loadCell();
  // Serial.println("Load Cell OK...");
  // delay(1000);
  setup_ultrasonicSensor();
  Serial.println("Ultrasonic Sensor OK...");
  delay(1000);
  setup_colorSensor();
  Serial.println("Color Sensor OK...");
  delay(1000);
  // servo_init();
  // Serial.println("Servo OK...");
  // delay(1000);
  t_awal = millis();
  lcd.clear();
  lcd.print("All set, Ready!");
  delay(2000);
  Serial.println("All set, Ready!");
  delay(100);
  lcd.clear();
}

void loop(){
  // FSM();
  ambilDataset();
}
