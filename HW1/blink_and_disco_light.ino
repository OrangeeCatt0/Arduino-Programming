
const int buttonPin = 2;

const int RledPin = 3;
const int GledPin = 4;
const int BledPin = 5;

int buttonState = LOW;
int lastButtonState = LOW;
int ledcolor = 0;

bool ambienceMode = false;

unsigned long lastDebounceTime = 0;
unsigned long lastClickTime = 0;
unsigned long lastAmbientTime = 0;

const unsigned long debounceDelay = 30;
const unsigned long doubleClickDelay = 350;
const unsigned long ambientInterval = 800;

int clickCount = 0;
int ambientColor = 0;

// Active-low LED logic from your original code
void setRGB(bool r, bool g, bool b) {
  digitalWrite(RledPin, r ? LOW : HIGH);
  digitalWrite(GledPin, g ? LOW : HIGH);
  digitalWrite(BledPin, b ? LOW : HIGH);
}

void setManualColor(int color) {
  switch (color) {
    case 0: setRGB(true, true, true); break;    // White
    case 1: setRGB(true, false, false); break;  // Red
    case 2: setRGB(false, true, false); break;  // Green
    case 3: setRGB(false, false, true); break;  // Blue
    case 4: setRGB(true, true, false); break;   // Yellow
    case 5: setRGB(true, false, true); break;   // Purple
    case 6: setRGB(false, true, true); break;   // Cyan
    case 7: setRGB(true, true, true); break;    // White
  }
}

void setup() {
  pinMode(RledPin, OUTPUT);
  pinMode(GledPin, OUTPUT);
  pinMode(BledPin, OUTPUT);

  pinMode(buttonPin, INPUT);

  setManualColor(ledcolor);
}

void loop() {
  checkButton();

  if (ambienceMode) {
    updateAmbience();
  }
}

void checkButton() {
  int reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        unsigned long now = millis();

        if (clickCount == 1 &&
            now - lastClickTime <= doubleClickDelay) {

          clickCount = 0;
          ambienceMode = !ambienceMode;

          if (ambienceMode) {
            ambientColor = 0;
            lastAmbientTime = millis();
            setManualColor(1); // Start with red
          } else {
            setManualColor(ledcolor);
          }

        } else {
          clickCount = 1;
          lastClickTime = now;
        }
      }
    }
  }

  lastButtonState = reading;

  if (clickCount == 1 &&
      millis() - lastClickTime > doubleClickDelay) {

    clickCount = 0;

    if (!ambienceMode) {
      ledcolor++;

      if (ledcolor > 7) {
        ledcolor = 0;
      }

      setManualColor(ledcolor);
    }
  }
}

void updateAmbience() {
  if (millis() - lastAmbientTime >= ambientInterval) {
    lastAmbientTime = millis();

    // Red -> Green -> Blue -> Yellow -> Purple -> Cyan -> White
    ambientColor++;

    if (ambientColor > 6) {
      ambientColor = 0;
    }

    // Map ambience colors to the original color states
    switch (ambientColor) {
      case 0: setRGB(true, false, false); break;  // Red
      case 1: setRGB(false, true, false); break;  // Green
      case 2: setRGB(false, false, true); break;  // Blue
      case 3: setRGB(true, true, false); break;   // Yellow
      case 4: setRGB(true, false, true); break;   // Purple
      case 5: setRGB(false, true, true); break;   // Cyan
      case 6: setRGB(true, true, true); break;    // White
    }
  }
}
