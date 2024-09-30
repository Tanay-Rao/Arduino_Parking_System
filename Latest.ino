#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Initialize LCD with the correct I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Initialize the servo motor for barrier control
Servo barrierServo;

// Pin definitions
const int ENTRYIR = 3; // IR sensor for entry
const int INTRANSITIR = 2; // IR sensor for in-transit

// Parking spot sensors
const int T1TRIG = 5;
const int T1ECHO = 6;            
const int T2TRIG = 7; 
const int T2ECHO = 8;           
const int T3TRIG = 9;
const int T3ECHO = 10;
const int T4TRIG = 11;
const int T4ECHO = 12;

const int TOTALPARKINGSPACES = 4; // Total parking available
const int MAXINTRANSIT = TOTALPARKINGSPACES; // In-transit cars can't exceed 4

const int DELAYTIME = 2000;

// Variables to track parking
int availableSpaces = TOTALPARKINGSPACES;
bool isCarParked[4] = {false, false, false, false}; // Status of parking spots
int carsInTransit = 0; // Total cars in transit

// Barrier state
int barrierState = 0;  // 0 = closed, 1 = open

void setup() {
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Parking System");
  delay(2000); // Show message for 2 seconds

  // Set pin modes for sensors
  pinMode(T1TRIG, OUTPUT); pinMode(T1ECHO, INPUT);
  pinMode(T2TRIG, OUTPUT); pinMode(T2ECHO, INPUT);
  pinMode(T3TRIG, OUTPUT); pinMode(T3ECHO, INPUT);
  pinMode(T4TRIG, OUTPUT); pinMode(T4ECHO, INPUT);
  
  pinMode(ENTRYIR, INPUT); // Set the entry IR sensor pin
  pinMode(INTRANSITIR, INPUT); // Set the in-transit IR sensor pin

  // Initialize the servo motor to closed position
  barrierServo.attach(4);
  barrierServo.write(0);  // 0 degrees = barrier closed
}
  
void displayLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  
  if (availableSpaces > 0) {
    lcd.print("Left:");
    if (!isCarParked[0]) lcd.print("1 ");
    if (!isCarParked[1]) lcd.print("2 ");
    if (!isCarParked[2]) lcd.print("3 ");
    if (!isCarParked[3]) lcd.print("4 ");
  } else {
    lcd.print("No Free Slots");
  }
  
  lcd.setCursor(0, 1);
  lcd.print("In Transit: ");
  lcd.print(carsInTransit);
}

long getDistance(int trigPin, int echoPin) {
  long duration, cm;

  // Trigger the sensor by sending a 10-microsecond pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo signal
  duration = pulseIn(echoPin, HIGH);

  // If no signal is received, return -1
  if (duration == 0) return -1;

  // Convert the time into distance in centimeters
  cm = duration / 58.2;
  return cm;
}

bool sensorDetectsCar(float distance) {
  return (distance > 0 && distance < 10);
}

// Function to update parking status
void updateParkingStatus(float distance, int slot) {
  if (sensorDetectsCar(distance) && !isCarParked[slot]) {
    // Car parked
    isCarParked[slot] = true;
    if (availableSpaces > 0) {
      availableSpaces -= 1;
    }    
    if (carsInTransit > 0) {
      carsInTransit -= 1;  // Decrease in-transit count
    }
    Serial.print("Car parked in Slot ");
    Serial.println(slot + 1);
  } 
  else if (!sensorDetectsCar(distance) && isCarParked[slot]) {
    // Car left
    isCarParked[slot] = false;
    if (availableSpaces < TOTALPARKINGSPACES) {
      availableSpaces += 1;
    }    
    Serial.print("Car left Slot ");
    Serial.println(slot + 1);
  }
}

// Update in-transit cars when detected
void updateInTransitStatus(int inTransitIR) {
  // Inverted logic for in-transit sensor
  if (inTransitIR == LOW && carsInTransit < MAXINTRANSIT) {
    carsInTransit += 1;
    Serial.print("Car in transit: ");
    Serial.println(carsInTransit);
  }
}

// Manage boom barrier and in-transit logic
void manageBoomBarrier(int ENTRYIR, int carsInside) {
  // Inverted logic for entry sensor
  if (barrierState == 0 && ENTRYIR == LOW) { // Entry signal LOW opens barrier
    if (carsInside < TOTALPARKINGSPACES) { 
      barrierServo.write(90); // Open barrier
      barrierState = 1;
      Serial.println("Car detected at entry. Opening barrier...");
    } else {
      Serial.println("Car Detected but barrier locked.");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Barrier Locked!");
      lcd.setCursor(0, 1);
      lcd.print("System full");
      delay(2000);
    }
  }
  
  // Close barrier when car has passed
  if (barrierState == 1 && ENTRYIR == HIGH) { // Entry signal HIGH closes barrier
    barrierServo.write(0); // Close barrier
    barrierState = 0;
    Serial.println("Closing barrier...");
  }
}

// Manage exit barrier logic


void loop() {
  // Get sensor readings
  float d1 = getDistance(T1TRIG, T1ECHO);
  float d2 = getDistance(T2TRIG, T2ECHO);
  float d3 = getDistance(T3TRIG, T3ECHO);
  float d4 = getDistance(T4TRIG, T4ECHO);  // Added for T4
  
  int entryIR = digitalRead(ENTRYIR); // IR sensor for entry
  int inTransitIR = digitalRead(INTRANSITIR); // IR sensor for in-transit
  

  // Print sensor readings to Serial Monitor
  Serial.println("Sensor Readings:");
  Serial.print("Parking Sensor 1: "); Serial.print(d1); Serial.println(" cm");
  Serial.print("Parking Sensor 2: "); Serial.print(d2); Serial.println(" cm");
  Serial.print("Parking Sensor 3: "); Serial.print(d3); Serial.println(" cm");
  Serial.print("Parking Sensor 4: "); Serial.print(d4); Serial.println(" cm");
  Serial.print("Entry IR Sensor: "); Serial.println(entryIR == LOW ? "Car Detected" : "No Car");
  Serial.print("In-Transit IR Sensor: "); Serial.println(inTransitIR == LOW ? "Car Detected" : "No Car");
  Serial.println("------------------------------");

  // Update parking status
  updateParkingStatus(d1, 0);
  updateParkingStatus(d2, 1);
  updateParkingStatus(d3, 2);
  updateParkingStatus(d4, 3);  // Added for T4
  updateInTransitStatus(inTransitIR);
  
  // Manage boom barrier for entry
  int carsInside = TOTALPARKINGSPACES - availableSpaces + carsInTransit;
  manageBoomBarrier(entryIR, carsInside);
  
  // Manage exit barrier
 

  // Display on LCD
  displayLCD();
  
  delay(1000); // Refresh rate
}
