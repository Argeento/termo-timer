#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin definitions
const int ONE_WIRE_BUS = A0;
const int SWITCH_PIN = 5;
const int RELAY_PIN = 6;
const int BUTTON_INC = 3;
const int BUTTON_DEC = 4;
const int BUTTON_START = 2;

// Constants
const int READ_TEMP_INTERVAL = 5;
const int TIME_STEP = 15 * 60;
const int MAX_TIME = 12 * 60 * 60;
const int MIN_TIME = 15 * 60;
const int TEMP_STEP = 5;
const int MAX_TEMP = 120;
const int MIN_TEMP = 40;
const int DEBOUNCE_DELAY = 200;
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
const int LCD_ADDR = 0x27;

// Mode definitions
enum Mode {
  TEMP_MODE = LOW,
  TIME_MODE = HIGH
};

// Libraries
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Variables
Mode mode = TIME_MODE;
unsigned long countdownTime = MIN_TIME;
unsigned long initialCountdownTime = countdownTime;
unsigned long remainingTime = 0;
unsigned long startTime;
bool timerRunning = false;
int targetTemp = MIN_TEMP;
int currentTemp = MIN_TEMP;

void setup() {
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_INC, INPUT_PULLUP);
  pinMode(BUTTON_DEC, INPUT_PULLUP);
  pinMode(BUTTON_START, INPUT_PULLUP);
  digitalWrite(RELAY_PIN, HIGH);

  lcd.init();
  lcd.backlight();
  sensors.begin();
}

void loop() {
  readTemperature();
  handleModeSwitch();
  handleButtons();
  updateRemainingTime();
  updateLCD();
  controlRelay();
}

void updateRemainingTime() {
  if (timerRunning) {
    remainingTime = initialCountdownTime - (millis() / 1000 - startTime);
    if (remainingTime <= 0) {
      timerRunning = false;
      remainingTime = 0;
    }
  }
}

void readTemperature() {
  static unsigned long lastTempReadTime = 0;
  if (millis() - lastTempReadTime > READ_TEMP_INTERVAL * 1000) {
    sensors.requestTemperatures();
    currentTemp = (int)sensors.getTempCByIndex(0);
    lastTempReadTime = millis();
  }
}

void handleModeSwitch() {
  mode = (Mode)digitalRead(SWITCH_PIN);
}

void handleButtons() {
  if (buttonPressed(BUTTON_INC)) handleButtonInc();
  if (buttonPressed(BUTTON_DEC)) handleButtonDec();
  if (buttonPressed(BUTTON_START)) handleButtonStart();
}

void handleButtonInc() {
  if (mode == TIME_MODE) {
    countdownTime = min(MAX_TIME, countdownTime + TIME_STEP);
  }
  else {
    targetTemp = min(MAX_TEMP, targetTemp + TEMP_STEP);
  }
}

void handleButtonDec() {
  if (mode == TIME_MODE) {
    countdownTime = max(MIN_TIME, countdownTime - TIME_STEP);
  }
  else {
    targetTemp = max(MIN_TEMP, targetTemp - TEMP_STEP);
  }
}

void handleButtonStart() {
  initialCountdownTime = countdownTime;
  startTime = millis() / 1000;
  timerRunning = true;
  remainingTime = initialCountdownTime;
}

void updateLCD() {
  displayTemperature();
  displayRemainingTime();
  displayCountdownTime();
  displayHeaterStatus();
}

void displayTemperature() {
  lcd.setCursor(0, 0);
  lcd.print(currentTemp);
  lcd.print('/');
  lcd.print(targetTemp);
  lcd.write(0b11011111);
  lcd.print("C  ");
}

void displayRemainingTime() {
  lcd.setCursor(0, 1);
  if (timerRunning) {
    const int remainingHours = remainingTime / 3600;
    const int remainingMinutes = (remainingTime % 3600) / 60;
    const int remainingSeconds = remainingTime % 60;
    lcd.print(formatTime(remainingHours));
    lcd.print(':');
    lcd.print(formatTime(remainingMinutes));
    lcd.print(':');
    lcd.print(formatTime(remainingSeconds));
  }
  else {
    lcd.print("00:00:00");
  }
}

void displayCountdownTime() {
  lcd.setCursor(9, 1);
  lcd.write(0b01111111);
  const int hours = countdownTime / 3600;
  const int minutes = (countdownTime % 3600) / 60;
  if (hours < 10) lcd.print(' ');
  lcd.print(hours);
  lcd.print('h');
  lcd.print(formatTime(minutes));
  lcd.print('m');
}

void displayHeaterStatus() {
  lcd.setCursor(13, 0);
  lcd.print(digitalRead(RELAY_PIN) == LOW ? " ON " : "OFF");
}

String formatTime(int timeValue) {
  if (timeValue < 10) return "0" + String(timeValue);
  return String(timeValue);
}

void controlRelay() {
  bool tempCondition = currentTemp <= targetTemp;
  digitalWrite(RELAY_PIN, tempCondition && timerRunning ? LOW : HIGH);
}

bool buttonPressed(int pin) {
  static unsigned long lastDebounceTime = 0;
  if (digitalRead(pin) == LOW && (millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    lastDebounceTime = millis();
    return true;
  }
  return false;
}