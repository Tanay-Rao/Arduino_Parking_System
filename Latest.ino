#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Initialize LCDs with the correct I2C addresses
LiquidCrystal_I2C lcd16x2(0x27, 16, 2); // 16x2 display
LiquidCrystal_I2C lcd20x4(0x26, 16, 2); // 20x4 display

// Initialize the servo motor
Servo myservo;

// Pin definitions
const int ENTRYIR = 12; // IR sensor for entry
const int INTRANSITIR = 13; // IR sensor for in-transit
const int EXITIR = 2; // IR sensor for exit

// Parking spot ultrasonic sensors
const int T1TRIG = 5;
const int T1ECHO = 6;            
const int T2TRIG = 7; 
const int T2ECHO = 8;           
const int T3TRIG = 9;
const int T3ECHO = 10; 
const int T4TRIG = 11;
const int T4ECHO = 4; // Added for the 4th parking spot

const int BUZZER = 1;  // Pin for buzzer

const int TOTALPARKINGSPACES = 4; // Total parking available
const int MAXINTRANSIT = TOTALPARKINGSPACES; // In-transit cars can't exceed 4

const int DELAYTIME = 2000;

// Variables to track parking
int availableSpaces = TOTALPARKINGSPACES;
bool isCarParked[4] = {false, false, false, false}; // Status of parking spots
int carsInTransit = 0; // Total cars in transit

// Barrier state
int barrierState = 0;  // 0 = closed, 1 = open, 2 = locked

// Buffer to store the latest serial monitor message
String serialMonitorMessage = "";

// Array to hold parking distances for display
float parkingDistances[4] = {0, 0, 0, 0};

void setup() {
  Serial.begin(9600);
  
  // Initialize both LCDs
  lcd16x2.init();
  lcd16x2.backlight();
  lcd16x2.clear();
  lcd16x2.print("Parking System");
  
  lcd20x4.init();
  lcd20x4.backlight();
  lcd20x4.clear();
  lcd20x4.print("Log Monitor");

  delay(2000); // Show message for 2 seconds

  // Set pin modes for sensors
  pinMode(T1TRIG, OUTPUT); pinMode(T1ECHO, INPUT);
  pinMode(T2TRIG, OUTPUT); pinMode(T2ECHO, INPUT);
  pinMode(T3TRIG, OUTPUT); pinMode(T3ECHO, INPUT);
  pinMode(T4TRIG, OUTPUT); pinMode(T4ECHO, INPUT); // 4th parking spot

  // IR sensors and buzzer
  pinMode(ENTRYIR, INPUT);
  pinMode(INTRANSITIR, INPUT);
  pinMode(EXITIR, INPUT);  // Exit IR sensor
  pinMode(BUZZER, OUTPUT);  // Set buzzer pin as output
  digitalWrite(BUZZER, LOW);  // Initially buzzer is off

  // Initialize the servo motor to closed position
  myservo.attach(3);
  myservo.write(0);  // 0 degrees = barrier closed

  // Test sensors on startup

}

void logToLCD20x4() {
  // Row 1: Barrier state & stuff
  lcd20x4.setCursor(0, 0);
  lcd20x4.print("Barrier:");
  if (barrierState == 0) {
    lcd20x4.print("C");
  } else if (barrierState == 1) {
    lcd20x4.print("O");
  } else if (barrierState == 2) {
    lcd20x4.print("L");
  }

  lcd20x4.setCursor(11, 0); // Display if car exits via exit IR row 1
  lcd20x4.print("L:");
  if (digitalRead(EXITIR) == HIGH) {
    lcd20x4.print("Yes");
  } else {
    lcd20x4.print("No");
  }

  // Row 3 and 4: Serial monitor messages
  lcd20x4.setCursor(0, 1);
  lcd20x4.print(serialMonitorMessage);
}

void displayLCD16x2() {
  lcd16x2.clear();
  lcd16x2.setCursor(0, 0);
  
  if (availableSpaces > 0) {
    lcd16x2.print("Slot Left: ");
    if (!isCarParked[0]) lcd16x2.print("1 ");
    if (!isCarParked[1]) lcd16x2.print("2 ");
    if (!isCarParked[2]) lcd16x2.print("3 ");
    if (!isCarParked[3]) lcd16x2.print("4 ");
  } else {
    lcd16x2.print("No Free Slots   ");
  }
  
  lcd16x2.setCursor(0, 1);
  lcd16x2.print("In Transit: ");
  lcd16x2.print(carsInTransit);
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
void updateParkingStatus(float distance, int slotIndex) {
  parkingDistances[slotIndex] = distance; // Update distance for display
  
  if (sensorDetectsCar(distance) && !isCarParked[slotIndex]) {
    // Car parked
    isCarParked[slotIndex] = true;
    availableSpaces--;
    serialMonitorMessage = "Car parked @" + String(slotIndex + 1);
    logToLCD20x4();
  } else if (!sensorDetectsCar(distance) && isCarParked[slotIndex]) {
    // Car left
    isCarParked[slotIndex] = false;
    availableSpaces++;
    serialMonitorMessage = "Car left from  " + String(slotIndex + 1);
    logToLCD20x4();
  }
}

// Function to update in-transit status
void updateInTransitStatus(int inTransitIR) {
  if (inTransitIR == LOW) {
    carsInTransit++;
    serialMonitorMessage = "Car in transit";
    logToLCD20x4();
  }
}

// Function to manage boom barrier based on parking availability and in-transit status
void manageBoomBarrier(int entryIR, int carsInside) {
  if (barrierState == 0 && entryIR == LOW && carsInside < TOTALPARKINGSPACES) {
    // Open the barrier
    myservo.write(90);  // Open barrier
    barrierState = 1;  // Mark barrier as open
    serialMonitorMessage = "Welocme. Opening";
    logToLCD20x4();
    delay(DELAYTIME);  // Keep barrier open for some time
  } else if (barrierState == 1 && entryIR == HIGH) {
    // Close the barrier after car passes
    myservo.write(0);  // Close barrier
    barrierState = 0;  // Mark barrier as closed
    serialMonitorMessage = "Closing barrier...";
    logToLCD20x4();
  }
  
  if (availableSpaces == 0) {
    // Lock the barrier if parking is full
    myservo.write(0);  // Ensure barrier is closed
    barrierState = 2;  // Mark barrier as locked
  }
}

void loop() {
  // Get sensor readings for parking spots (ultrasonic)
  float d1 = getDistance(T1TRIG, T1ECHO);
  float d2 = getDistance(T2TRIG, T2ECHO);
  float d3 = getDistance(T3TRIG, T3ECHO);
  float d4 = getDistance(T4TRIG, T4ECHO); // 4th parking spot

  // Read IR sensor states for entry and in-transit
  int entryIR = digitalRead(ENTRYIR);
  int inTransitIR = digitalRead(INTRANSITIR);

  // Update parking status
  updateParkingStatus(d1, 0);
  updateParkingStatus(d2, 1);
  updateParkingStatus(d3, 2);
  updateParkingStatus(d4, 3); // 4th parking spot status
  updateInTransitStatus(inTransitIR);

  // Manage boom barrier
  int carsInside = TOTALPARKINGSPACES - availableSpaces + carsInTransit;
  manageBoomBarrier(entryIR, carsInside);

  // Display on LCDs
  displayLCD16x2();
  logToLCD20x4();

  delay(1000);  // Refresh rate
}
