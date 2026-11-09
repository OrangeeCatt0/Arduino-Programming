const int buttonPin = 2;
const int ledPin = 3;

const int blinkDelay[4] = {0, 1000, 500, 150};

int state = 0;
bool ledOn = true;
bool wasPressed = false;
unsigned long lastChange = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  digitalWrite(ledPin, HIGH);
}

void loop() {
  bool pressed = (digitalRead(buttonPin) == LOW);

  if (pressed && !wasPressed) {
    state = (state + 1) % 4;
    ledOn = true;
    lastChange = millis();
  }
  wasPressed = pressed;

  if (state == 0) {
    ledOn = true;
  } else if (millis() - lastChange >= blinkDelay[state]) {
    ledOn = !ledOn;
    lastChange = millis();
  }

  digitalWrite(ledPin, ledOn);
  delay(10);
}