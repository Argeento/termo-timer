#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin definitions
#define ONE_WIRE_BUS 2  // DS18B20 on Arduino pin 2
#define SWITCH_PIN 3    // Switch on pin 3
#define RELAY_PIN 4     // Relay on pin 4
#define BUTTON_INC 5    // Increase button on pin 5
#define BUTTON_DEC 6    // Decrease button on pin 6
#define BUTTON_START 7  // Start countdown button on pin 7

// Mode definitions
#define TEMP_MODE HIGH
#define TIME_MODE LOW

// Initialize the library with the I2C address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Setup a oneWire instance
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Variables
const int MAX_TIME = 600;
const int MIN_TIME = 15;
const int MAX_TEMP = 120;
const int MIN_TEMP = 40;

int targetTemp = 25;      // Initial target temperature
int countdownTime = 15;   // Initial countdown time in minutes
int remainingTime = 0;    // Remaining time
bool mode = TIME_MODE;    // Initial mode
unsigned long startTime;  // Timer start
bool timerRunning = false;

void setup() {
  pinMode(SWITCH_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_INC, INPUT);
  pinMode(BUTTON_DEC, INPUT);
  pinMode(BUTTON_START, INPUT);

  lcd.init();       // initialize the lcd
  lcd.backlight();  // turn on backlight
  sensors.begin();  // Start temperature sensor
}

void loop() {
  sensors.requestTemperatures();
  float currentTemp = sensors.getTempCByIndex(0);

  mode = digitalRead(SWITCH_PIN);

  if (digitalRead(BUTTON_INC) == HIGH) {
    if (mode == TIME_MODE) {
      countdownTime = min(MAX_TIME, countdownTime + 15);
    }
    else {
      targetTemp = min(MAX_TEMP, targetTemp + 5);
    }
    delay(200);  // Debounce delay
  }

  if (digitalRead(BUTTON_DEC) == HIGH) {
    if (mode == TIME_MODE) {
      countdownTime = max(MIN_TIME, countdownTime - 15);
    }
    else {
      targetTemp = max(MIN_TEMP, targetTemp - 5);
    }
    delay(200);  // Debounce delay
  }

  if (digitalRead(BUTTON_START) == HIGH) {
    startTime = millis();
    timerRunning = true;
    remainingTime = countdownTime;
    delay(200);  // Debounce delay
  }

  if (timerRunning) {
    remainingTime = countdownTime - (millis() - startTime) / 60000;
    if (remainingTime <= 0) {
      timerRunning = false;
      remainingTime = 0;
    }
  }

  // Update LCD display
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(currentTemp);
  lcd.print("C ");
  lcd.setCursor(8, 0);
  lcd.print("Set:");
  lcd.print(targetTemp);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Time:");
  lcd.print(remainingTime);
  lcd.print("m ");
  lcd.setCursor(8, 1);
  lcd.print("Set:");
  lcd.print(countdownTime);
  lcd.print("m");

  // Control relay
  bool tempCondition = currentTemp >= targetTemp;
  bool timeCondition = !timerRunning;

  if (tempCondition && timeCondition) {
    digitalWrite(RELAY_PIN, LOW);  // Turn off relay
  }
  else {
    digitalWrite(RELAY_PIN, HIGH);  // Turn on relay
  }
}