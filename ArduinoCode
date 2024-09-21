#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Initialize the LCD with the correct I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize the servo motor
Servo myservo;

// Define pin numbers for the HC-SR04 sensors
const int t1Trig = 5, t1Echo = 6;  // Parking spot 1 sensor
const int t2Trig = 7, t2Echo = 8;  // Parking spot 2 sensor
const int t3Trig = 9, t3Echo = 10; // Parking spot 3 sensor
const int entryTrig = 13, entryEcho = 12;  // Entry sensor
const int exitTrig = 11, exitEcho = 4;   // Exit sensor

// Variables to track parking spaces
int totalParkingSpaces = 3;  // Total parking spaces
int availableSpaces = 3;     // Available parking spaces
bool isCarParked[3] = {false, false, false};  // To track the status of each spot

// Variables for the boom barrier
int barrierState = 0;  // 0 = closed, 1 = open

void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Tanays Design");

  delay(2000);  // Display initial message for 2 seconds

  // Set pin modes for HC-SR04 sensors
  pinMode(t1Trig, OUTPUT);
  pinMode(t1Echo, INPUT);
  pinMode(t2Trig, OUTPUT);
  pinMode(t2Echo, INPUT);
  pinMode(t3Trig, OUTPUT);
  pinMode(t3Echo, INPUT);
  pinMode(entryTrig, OUTPUT);
  pinMode(entryEcho, INPUT);
  pinMode(exitTrig, OUTPUT);
  pinMode(exitEcho, INPUT);

  // Initialize the servo motor and set it to the closed position
  myservo.attach(3);  // Attach servo to pin 3
  myservo.write(0);   // 0 degrees = barrier closed
}

void loop() {
  // Read distances from parking sensors
  float d1 = getDistance(t1Trig, t1Echo);
  float d2 = getDistance(t2Trig, t2Echo);
  float d3 = getDistance(t3Trig, t3Echo);

  // Read distances from entry and exit sensors
  float entryDistance = getDistance(entryTrig, entryEcho);
  float exitDistance = getDistance(exitTrig, exitEcho);

  // Display sensor readings on Serial Monitor
  Serial.println("Sensor Readings:");
  Serial.println("Slot 1: " + String(d1) + " cm");
  Serial.println("Slot 2: " + String(d2) + " cm");
  Serial.println("Slot 3: " + String(d3) + " cm");
  Serial.println("Entry: " + String(entryDistance) + " cm");
  Serial.println("Exit: " + String(exitDistance) + " cm");

  // Update parking status based on distance (less than 10 cm means occupied)
  updateParkingStatus(d1, 0); // Slot 1
  updateParkingStatus(d2, 1); // Slot 2
  updateParkingStatus(d3, 2); // Slot 3

  // Boom barrier logic for both entry and exit
  manageBoomBarrier(entryDistance, exitDistance);

  // Display available spaces on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Spaces Left: ");
  lcd.print(availableSpaces);

  // Display which slots are available
  lcd.setCursor(0, 1);  // Move to the second row
  if (availableSpaces == 0) {
    lcd.print("No Free Slots   ");  // If no slots are available
  } else {
    lcd.print("Free: ");
    if (!isCarParked[0]) lcd.print("1 ");
    if (!isCarParked[1]) lcd.print("2 ");
    if (!isCarParked[2]) lcd.print("3 ");
  }

  delay(1000);  // Delay to avoid constant refreshing
}

// Function to get distance from HC-SR04 sensor
long getDistance(int trigPin, int echoPin) {
  long duration, cm;

  // Trigger the sensor by sending a 10-microsecond pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo signal
  duration = pulseIn(echoPin, HIGH);

  // If no signal is received, return -1
  if (duration == 0) {
    return -1;
  }

  // Convert the time into distance in centimeters
  cm = duration / 58.2;  // Correct conversion factor for centimeters
  return cm;
}

// Function to update parking status based on distance
void updateParkingStatus(float distance, int slot) {
  if (distance > 0 && distance < 10 && !isCarParked[slot]) {
    // A car is now parked in this slot
    isCarParked[slot] = true;
    availableSpaces -= 1;  // Decrease available spaces
    Serial.print("Car parked in Slot ");
    Serial.println(slot + 1);
  } 
  else if (distance >= 10 && isCarParked[slot]) {
    // The car has left this slot
    isCarParked[slot] = false;
    availableSpaces += 1;  // Increase available spaces
    Serial.print("Car left Slot ");
    Serial.println(slot + 1);
  }
}

// Function to manage the boom barrier logic for both entry and exit
void manageBoomBarrier(float entryDistance, float exitDistance) {
  if (barrierState == 0) {
    // Barrier is closed
    if (entryDistance > 0 && entryDistance < 10 && availableSpaces > 0) {
      // Car detected at entry and there's space, open the barrier
      Serial.println("Car detected at entry. Opening barrier...");
      myservo.write(90);  // 90 degrees = barrier open
      barrierState = 1;
    }
    else if (exitDistance > 0 && exitDistance < 10) {
      // Car detected at exit, open the barrier
      Serial.println("Car detected at exit. Opening barrier...");
      myservo.write(90);  // 90 degrees = barrier open
      barrierState = 1;
    }
  } 
  else if (barrierState == 1) {
    // Barrier is open
    if (entryDistance >= 10 && exitDistance >= 10) {
      // Both entry and exit sensors are clear, close the barrier
      Serial.println("No car detected. Closing barrier...");
      myservo.write(0);  // 0 degrees = barrier closed
      barrierState = 0;
    }
  }
}
