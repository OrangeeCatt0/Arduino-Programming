// HW3-1: Light Stick
// Wiring:
//   RGB module    : V -> 5V, R -> D9, G -> D10, B -> D11
//   Button module : IN -> D2, VCC -> 5V, GND -> GND
// Short click : change color (red, green, blue, yellow, magenta)
// Hold 1 sec  : change mode (constant -> blink -> breathing -> disco)

const int BUTTON_PIN = 2;
const int RED_PIN    = 9;
const int GREEN_PIN  = 10;
const int BLUE_PIN   = 11;

int colors[5][3] = {
  {255, 0, 0},
  {0, 255, 0},
  {0, 0, 255},
  {255, 255, 0},
  {255, 0, 255}
};

int colorIndex = 0;
int mode = 0;
unsigned long modeStart = 0;
unsigned long pressStart = 0;
bool wasPressed = false;
bool longDone = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void loop() {
  checkButton();
  showLight();
  delay(10);
}

void checkButton() {
  bool pressed = (digitalRead(BUTTON_PIN) == LOW);

  if (pressed && !wasPressed) {
    pressStart = millis();
    longDone = false;
  }

  if (pressed && !longDone && millis() - pressStart >= 1000) {
    mode = (mode + 1) % 4;
    modeStart = millis();
    longDone = true;
  }

  if (!pressed && wasPressed && !longDone) {
    colorIndex = (colorIndex + 1) % 5;
  }

  wasPressed = pressed;
}

void showLight() {
  int r = colors[colorIndex][0];
  int g = colors[colorIndex][1];
  int b = colors[colorIndex][2];
  unsigned long t = millis() - modeStart;

  switch (mode) {
    case 0:
      setColor(r, g, b);
      break;

    case 1:
      if ((t / 300) % 2 == 0) setColor(r, g, b);
      else                    setColor(0, 0, 0);
      break;

    case 2: {
      int phase = t % 3000;
      if (phase > 1500) phase = 3000 - phase;
      long level = (long)phase * 255 / 1500;
      setColor(r * level / 255, g * level / 255, b * level / 255);
      break;
    }

    case 3: {
      int c = (t / 200) % 5;
      setColor(colors[c][0], colors[c][1], colors[c][2]);
      break;
    }
  }
}

void setColor(int red, int green, int blue) {
  analogWrite(RED_PIN,   255 - red);
  analogWrite(GREEN_PIN, 255 - green);
  analogWrite(BLUE_PIN,  255 - blue);
}