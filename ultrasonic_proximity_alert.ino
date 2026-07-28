#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================================
// PIN SETTINGS
// =====================================================

#define OLED_SDA_PIN 8
#define OLED_SCL_PIN 9

#define TRIG_PIN 4
#define ECHO_PIN 5
#define BUZZER_PIN 6

// =====================================================
// OLED SETTINGS
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// =====================================================
// DISTANCE SETTINGS
// =====================================================

// Buzzer is silent above this distance.
const float WARNING_DISTANCE_CM = 40.0;

// Faster warning below this distance.
const float NEAR_DISTANCE_CM = 20.0;

// Continuous alarm below this distance.
const float DANGER_DISTANCE_CM = 10.0;

// Used only for the OLED distance bar.
const float BAR_MAX_DISTANCE_CM = 100.0;

// =====================================================
// TIMING
// =====================================================

const unsigned long MEASUREMENT_INTERVAL_MS = 150;

unsigned long lastMeasurementTime = 0;
unsigned long lastBuzzerToggleTime = 0;

bool buzzerState = false;
bool firstValidReading = true;

float displayedDistance = 0.0;

// =====================================================
// SETUP
// =====================================================

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(TRIG_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin(OLED_SDA_PIN, OLED_SCL_PIN);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS
      )) {
    Serial.println("OLED initialization failed.");

    while (true) {
      digitalWrite(BUZZER_PIN, LOW);
      delay(1000);
    }
  }

  showStartupScreen();

  Serial.println();
  Serial.println("ESP32 Ultrasonic Proximity Monitor");
  Serial.println("System started.");

  delay(1500);
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {
  unsigned long currentTime = millis();

  if (
    currentTime - lastMeasurementTime
    >= MEASUREMENT_INTERVAL_MS
  ) {
    lastMeasurementTime = currentTime;

    float rawDistance = measureDistanceCm();

    if (rawDistance > 0) {
      displayedDistance = smoothDistance(rawDistance);

      printDistanceToSerial(displayedDistance);
      updateDisplay(displayedDistance);
    } else {
      showSensorError();
    }
  }

  updateBuzzer(displayedDistance);
}

// =====================================================
// ULTRASONIC MEASUREMENT
// =====================================================

float measureDistanceCm() {
  // Ensure a clean trigger pulse.
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  // Wait up to 30 ms for the echo.
  unsigned long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    30000UL
  );

  // No echo received.
  if (duration == 0) {
    return -1.0;
  }

  // Speed of sound is approximately 0.0343 cm/us.
  // Divide by 2 because sound travels to the object and back.
  float distance = duration * 0.0343F / 2.0F;

  // Practical HC-SR04 range validation.
  if (distance < 2.0 || distance > 400.0) {
    return -1.0;
  }

  return distance;
}

// Reduces rapid jumping in the displayed reading.
float smoothDistance(float newDistance) {
  if (firstValidReading) {
    firstValidReading = false;
    return newDistance;
  }

  return displayedDistance * 0.65F
       + newDistance * 0.35F;
}

// =====================================================
// DISTANCE STATUS
// =====================================================

const char* getDistanceStatus(float distance) {
  if (distance < DANGER_DISTANCE_CM) {
    return "TOO CLOSE";
  }

  if (distance < NEAR_DISTANCE_CM) {
    return "VERY NEAR";
  }

  if (distance < WARNING_DISTANCE_CM) {
    return "NEAR";
  }

  return "CLEAR";
}

// =====================================================
// OLED
// =====================================================

void showStartupScreen() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(14, 12);
  display.println("ULTRASONIC OBJECT");

  display.setCursor(28, 25);
  display.println("DETECTOR");

  display.drawRect(
    19,
    43,
    90,
    10,
    SSD1306_WHITE
  );

  display.fillRect(
    22,
    46,
    55,
    4,
    SSD1306_WHITE
  );

  display.display();
}

void updateDisplay(float distance) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Header
  display.setTextSize(1);
  display.setCursor(12, 0);
  display.println("OBJECT DISTANCE");

  display.drawLine(
    0,
    10,
    SCREEN_WIDTH - 1,
    10,
    SSD1306_WHITE
  );

  // Large distance value
  display.setTextSize(2);
  display.setCursor(7, 16);

  if (distance < 10.0) {
    display.print(" ");
  }

  display.print(distance, 1);
  display.print(" cm");

  // Status
  display.setTextSize(1);
  display.setCursor(0, 39);
  display.print("Status: ");
  display.println(getDistanceStatus(distance));

  // Bar outline
  display.drawRect(
    0,
    52,
    SCREEN_WIDTH,
    12,
    SSD1306_WHITE
  );

  // Closer object = longer bar.
  float limitedDistance = constrain(
    distance,
    0.0F,
    BAR_MAX_DISTANCE_CM
  );

  int barWidth = map(
    (int)limitedDistance,
    0,
    (int)BAR_MAX_DISTANCE_CM,
    124,
    0
  );

  barWidth = constrain(barWidth, 0, 124);

  display.fillRect(
    2,
    54,
    barWidth,
    8,
    SSD1306_WHITE
  );

  display.display();
}

void showSensorError() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(9, 10);
  display.println("ULTRASONIC SENSOR");

  display.setTextSize(2);
  display.setCursor(27, 30);
  display.println("ERROR");

  display.setTextSize(1);
  display.setCursor(15, 53);
  display.println("Check wiring");

  display.display();

  digitalWrite(BUZZER_PIN, LOW);
  buzzerState = false;

  Serial.println("No valid distance reading.");
}

// =====================================================
// BUZZER
// =====================================================

void updateBuzzer(float distance) {
  if (
    distance <= 0 ||
    firstValidReading
  ) {
    turnBuzzerOff();
    return;
  }

  unsigned long currentTime = millis();

  // Continuous alarm below 10 cm.
  if (distance < DANGER_DISTANCE_CM) {
    digitalWrite(BUZZER_PIN, HIGH);
    buzzerState = true;
    return;
  }

  // Fast beeping from 10 to 20 cm.
  if (distance < NEAR_DISTANCE_CM) {
    toggleBuzzerAtInterval(
      currentTime,
      120
    );
    return;
  }

  // Slow beeping from 20 to 40 cm.
  if (distance < WARNING_DISTANCE_CM) {
    toggleBuzzerAtInterval(
      currentTime,
      400
    );
    return;
  }

  // Silent above 40 cm.
  turnBuzzerOff();
}

void toggleBuzzerAtInterval(
  unsigned long currentTime,
  unsigned long interval
) {
  if (
    currentTime - lastBuzzerToggleTime
    >= interval
  ) {
    lastBuzzerToggleTime = currentTime;
    buzzerState = !buzzerState;

    digitalWrite(
      BUZZER_PIN,
      buzzerState ? HIGH : LOW
    );
  }
}

void turnBuzzerOff() {
  digitalWrite(BUZZER_PIN, LOW);
  buzzerState = false;
}

// =====================================================
// SERIAL MONITOR
// =====================================================

void printDistanceToSerial(float distance) {
  Serial.print("Distance: ");
  Serial.print(distance, 1);
  Serial.print(" cm | Status: ");
  Serial.println(getDistanceStatus(distance));
}