
#define echoPin A1
#define trigPin A2
//int echoPin = A1, trigPin = A2;


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
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  size = round(max_size - distance);

  return size;
}

void setup() {
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(1);
//  while(!Serial.available()) {
//    Serial.println("waiting to start...");
//    delay(1000);
//  }
}

void loop() {
  // put your main code here, to run repeatedly:
  readSize();
  delay(500);
}
