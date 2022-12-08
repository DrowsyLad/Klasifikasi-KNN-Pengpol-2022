#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define trigPin 3
#define echoPin 2

#define S0_CS 7
#define S1_CS 6
#define S2_CS 9
#define S3_CS 8
#define OUT_CS 10


LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int getGreenPW() {
  // Set sensor to read Green only
  digitalWrite(S2_CS,HIGH);
  digitalWrite(S3_CS,HIGH);
  return pulseIn(OUT_CS, LOW);
}

int getBluePW() {
  // Set sensor to read Blue only
  digitalWrite(S2_CS,LOW);
  digitalWrite(S3_CS,HIGH);
  return pulseIn(OUT_CS, LOW);
}

int getRedPW() {
  // Set sensor to read Red only
  digitalWrite(S2_CS,LOW);
  digitalWrite(S3_CS,LOW);
  return pulseIn(OUT_CS, LOW);
}

void readRGB(float* redValue, float* blueValue, float* greenValue){
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
  for (int i = 0; i < 5; i++)
  {
    int redPW = getRedPW();
    *redValue = 
                redPW;
//                map(redPW, redMin, redMax, 100, 0);
    if(*redValue < 0){
      *redValue = 0;
    }
    delay(100);

    // Read Green Pulse Width
    int greenPW = getGreenPW();
    *greenValue = 
                  greenPW;
//                  map(greenPW, greenMin, greenMax, 100, 0);
    if(*greenValue < 0){
      *greenValue = 0;
    }
    delay(100);

    // Read Blue Pulse Width
    int bluePW = getBluePW();
    *blueValue = 
                  bluePW;
//                  map(bluePW, blueMin, blueMax, 100, 0);
    if(*blueValue < 0){
      *blueValue = 0;
    }
    delay(100);  

    *redValue += *redValue;
    *greenValue += *greenValue;
    *blueValue += *blueValue;
  }

  *redValue /= 5;
  *greenValue /= 5;
  *blueValue /= 5;

}

float readSize(){
  long duration; // variable for the duration of sound wave travel
  float distance; // variable for the distance measurement
  float max_size = 12, size; //size of object
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
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  size = max_size - distance;

  return size;
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

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  delay(500);
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("LCD initialized");
  lcd.setCursor(1,1);
  lcd.print("Welcome!");
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(1);
  setup_colorSensor();
}

void test_readSize(){
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
}

void test_readRGB(){
  float redValue, greenValue, blueValue;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Reading color...:");
  Serial.println("Begin reading color...");
  Serial.println("Reading color...\n");
  Serial.print("Color:\n");
  readRGB(&redValue, &greenValue, &blueValue);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Red color:");
  lcd.setCursor(0,1);
  lcd.print(redValue);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Green color:");
  lcd.setCursor(0,1);
  lcd.print(greenValue);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Blue color:");
  lcd.setCursor(0,1);
  lcd.print(blueValue);
  delay(2000);
}

void loop()
{
  test_readSize();
  test_readRGB();
}
