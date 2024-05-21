#define BLYNK_TEMPLATE_ID "TMPL3qeyDalVN"
#define BLYNK_TEMPLATE_NAME "led"
#define BLYNK_AUTH_TOKEN "xT5LKHx6W7fuAoQSFoB5fhYqco5V5oef"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

const int ldrPin1 = 34;   // GPIO pin connected to the first LDR
const int ledPin1 = 4;    // GPIO pin connected to the first LED
const int irPin1 = 35;    // GPIO pin connected to the first IR sensor

const int ldrPin2 = 32;   // GPIO pin connected to the second LDR
const int ledPin2 = 2;    // GPIO pin connected to the second LED
const int irPin2 = 33;    // GPIO pin connected to the second IR sensor

bool automaticMode = true; // Automatic mode is on by default

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Resh";
char pass[] = "244466666";

BlynkTimer timer;

void setup() {
  pinMode(ldrPin1, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(irPin1, INPUT_PULLUP); // Using INPUT_PULLUP to avoid floating pin

  pinMode(ldrPin2, INPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(irPin2, INPUT_PULLUP); // Using INPUT_PULLUP to avoid floating pin

  Serial.begin(115200);
  delay(1000);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, readSensors); // Set interval to read sensors every second
}

void loop() {
  Blynk.run();
  timer.run();
}

void readSensors() {
  // Read and process the first set of sensors and LED
  int ldrValue1 = analogRead(ldrPin1); // Read the first LDR value
  int scaledLdrValue1 = map(ldrValue1, 0, 4095, 0, 500); // Scale the first LDR value to 0-500 range for the gauge
  Blynk.virtualWrite(V5, scaledLdrValue1); // Update Gauge with scaled LDR value
  
  int irValue1 = digitalRead(irPin1); // Read the first IR sensor value

  if (automaticMode) {
    if (ldrValue1 < 3000) {
      // If light intensity is low, turn off the first LED
      analogWrite(ledPin1, LOW); // Turn off the first LED
    } else {
      // If light intensity is high, control the first LED based on the IR sensor
      if (irValue1 == LOW) {
        // If the first IR sensor detects motion, increase the first LED brightness
        analogWrite(ledPin1, 255); // Set the first LED brightness to maximum
      } else {
        // If the first IR sensor does not detect motion, set the first LED to dim brightness
        analogWrite(ledPin1, 50); // Set the first LED brightness to dim
      }
    }
  }

  // Read and process the second set of sensors and LED
  int ldrValue2 = analogRead(ldrPin2); // Read the second LDR value
  int scaledLdrValue2 = map(ldrValue2, 0, 4095, 0, 500); // Scale the second LDR value to 0-500 range for the gauge
  Blynk.virtualWrite(V6, scaledLdrValue2); // Update Gauge with scaled LDR value
  
  int irValue2 = digitalRead(irPin2); // Read the second IR sensor value

  if (automaticMode) {
    if (ldrValue2 < 3000) {
      // If light intensity is low, turn off the second LED
      analogWrite(ledPin2, LOW); // Turn off the second LED
    } else {
      // If light intensity is high, control the second LED based on the IR sensor
      if (irValue2 == LOW) {
        // If the second IR sensor detects motion, increase the second LED brightness
        analogWrite(ledPin2, 255); // Set the second LED brightness to maximum
      } else {
        // If the second IR sensor does not detect motion, set the second LED to dim brightness
        analogWrite(ledPin2, 50); // Set the second LED brightness to dim
      }
    }
  }
}

BLYNK_WRITE(D4) // Switch widget on D4 for the first LED
{
  int switchState = param.asInt(); // Get value from the switch (0 or 1)
  if (!automaticMode) { // Only use switch value in manual mode
    if (switchState == 1) {
      analogWrite(ledPin1, 255); // Turn on the first LED
    } else {
      analogWrite(ledPin1, LOW); // Turn off the first LED
    }
  }
}


BLYNK_WRITE(D2) // Switch widget on D2 for the second LED
{
  int switchState = param.asInt(); // Get value from the switch (0 or 1)
  if (!automaticMode) { // Only use switch value in manual mode
    if (switchState == 1) {
      analogWrite(ledPin2, 255); // Turn on the second LED
    } else {
      analogWrite(ledPin2, LOW); // Turn off the second LED
    }
  }
}

BLYNK_WRITE(D3) // Switch widget on D3 for automatic/manual mode
{
  int switchState = param.asInt(); // Get value from the switch (0 or 1)
  automaticMode = (switchState == 1); // Set automaticMode based on switch state
  if (automaticMode) {
    // If switching to automatic mode, ensure the LEDs are controlled by the sensors
    readSensors();
  } else {
    // If switching to manual mode, turn off both LEDs initially to await switch control
    analogWrite(ledPin1, LOW);
    analogWrite(ledPin2, LOW);
  }
}