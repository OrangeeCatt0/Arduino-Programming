
#include <math.h>

// ===================== PIN SETTINGS =====================
const uint8_t PIN_R   = 9;
const uint8_t PIN_G   = 10;
const uint8_t PIN_B   = 11;
const uint8_t PIN_BTN = 2;

// Updated to match the RGB LED test that worked
const bool COMMON_ANODE = true;

// ===================== MOOD SETTINGS =====================
const float PET_GAIN = 8.0;
const float DECAY_AMOUNT = 3.0;

const unsigned long IDLE_BEFORE_DECAY = 5000;
const unsigned long DECAY_INTERVAL = 1000;
const unsigned long HOLD_TO_START = 1500;

// ===================== GAME SETTINGS =====================
const unsigned long COUNTDOWN_MS = 3000;
const unsigned long GAME_MS = 30000;

const unsigned long GREEN_MIN = 1500;
const unsigned long GREEN_MAX = 4000;

const unsigned long RED_MIN = 1200;
const unsigned long RED_MAX = 3000;

const unsigned long RED_GRACE = 300;
const unsigned long PENALTY_REPEAT = 700;

const int PENALTY_POINTS = 3;
const int WIN_SCORE = 15;

const unsigned long RESULT_MS = 5000;
const unsigned long DEBOUNCE_MS = 30;

// ===================== PROGRAM STATES =====================
enum State {
  MOOD,
  COUNTDOWN,
  GREEN,
  RED,
  RESULT
};

State state = MOOD;

// ===================== BUTTON VARIABLES =====================
bool btnPressed = false;
bool btnJustPressed = false;
bool btnRaw = false;
bool btnLastRaw = false;

unsigned long btnChangeTime = 0;
unsigned long pressStart = 0;

// ===================== MOOD VARIABLES =====================
float mood = 50.0;

unsigned long lastTouch = 0;
unsigned long lastDecay = 0;

bool holdHandled = false;

// ===================== GAME VARIABLES =====================
unsigned long stateStart = 0;
unsigned long phaseLen = 0;
unsigned long gameStart = 0;
unsigned long lastPenalty = 0;

int score = 0;
bool won = false;

// ===================== FLASH VARIABLES =====================
unsigned long flashUntil = 0;

uint8_t flashR = 255;
uint8_t flashG = 255;
uint8_t flashB = 255;

// ===================== LED CONTROL =====================
void setColor(uint8_t r, uint8_t g, uint8_t b) {
  if (COMMON_ANODE) {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
  }

  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
}

void showColor(uint8_t r, uint8_t g, uint8_t b,
               float brightness) {
  if (brightness < 0) brightness = 0;
  if (brightness > 1) brightness = 1;

  setColor(
    (uint8_t)(r * brightness),
    (uint8_t)(g * brightness),
    (uint8_t)(b * brightness)
  );
}

// ===================== BREATHING EFFECT =====================
float breathe(unsigned long periodMs) {
  float phase =
    (float)(millis() % periodMs) / (float)periodMs;

  float s =
    (sin(phase * 2.0 * PI - HALF_PI) + 1.0) / 2.0;

  return 0.15 + 0.85 * s;
}

// ===================== RAINBOW EFFECT =====================
void wheel(uint8_t pos, uint8_t &r,
           uint8_t &g, uint8_t &b) {
  if (pos < 85) {
    r = 255 - pos * 3;
    g = pos * 3;
    b = 0;
  }
  else if (pos < 170) {
    pos -= 85;
    r = 0;
    g = 255 - pos * 3;
    b = pos * 3;
  }
  else {
    pos -= 170;
    r = pos * 3;
    g = 0;
    b = 255 - pos * 3;
  }
}

// ===================== FLASH FEEDBACK =====================
void startFlash(unsigned long durationMs,
                uint8_t r, uint8_t g, uint8_t b) {
  flashUntil = millis() + durationMs;
  flashR = r;
  flashG = g;
  flashB = b;
}

bool drawFlash() {
  if ((long)(flashUntil - millis()) > 0) {
    if ((millis() / 70) % 2 == 0) {
      setColor(flashR, flashG, flashB);
    }
    else {
      setColor(0, 0, 0);
    }
    return true;
  }

  return false;
}

// ===================== BUTTON READING =====================
void updateButton() {
  unsigned long now = millis();
  btnJustPressed = false;

  // INPUT_PULLUP: LOW means pressed
  btnRaw = (digitalRead(PIN_BTN) == LOW);

  if (btnRaw != btnLastRaw) {
    btnChangeTime = now;
    btnLastRaw = btnRaw;
  }

  if ((now - btnChangeTime) > DEBOUNCE_MS &&
      btnRaw != btnPressed) {
    btnPressed = btnRaw;

    if (btnPressed) {
      btnJustPressed = true;
      pressStart = now;
    }
  }
}

// ===================== START GAME =====================
void startCountdown() {
  state = COUNTDOWN;
  stateStart = millis();

  Serial.println("--- GAME START: Get ready! ---");
}

// ===================== GREEN PHASE =====================
void enterGreen() {
  state = GREEN;
  stateStart = millis();
  phaseLen = random(GREEN_MIN, GREEN_MAX + 1);

  Serial.println("GREEN - Squeeze the doll!");
}

// ===================== RED PHASE =====================
void enterRed() {
  state = RED;
  stateStart = millis();
  phaseLen = random(RED_MIN, RED_MAX + 1);
  lastPenalty = 0;

  Serial.println("RED - Don't squeeze!");
}

// ===================== GAME TIME =====================
bool gameTimeUp() {
  return (millis() - gameStart) >= GAME_MS;
}

// ===================== END GAME =====================
void endGame() {
  state = RESULT;
  stateStart = millis();

  won = (score >= WIN_SCORE);
  mood = won ? 100.0 : 20.0;

  Serial.print("GAME OVER. Score = ");
  Serial.print(score);

  if (won) {
    Serial.println(" -> YOU WIN! Rainbow animation.");
  }
  else {
    Serial.println(" -> YOU LOSE! Red breathing light.");
  }
}

// ===================== NORMAL / MOOD MODE =====================
void updateMood() {
  unsigned long now = millis();

  // Squeeze increases mood
  if (btnJustPressed) {
    mood += PET_GAIN;
    if (mood > 100) mood = 100;

    lastTouch = now;
    lastDecay = now;
    holdHandled = false;

    startFlash(90, 255, 255, 255);

    Serial.print("Pet! Mood = ");
    Serial.println(mood);
  }

  // Hold for 1.5 seconds to start the game
  if (btnPressed) {
    lastTouch = now;

    if (!holdHandled &&
        (now - pressStart) >= HOLD_TO_START) {
      holdHandled = true;
      startCountdown();
      return;
    }
  }

  // Mood decays while the doll is ignored
  if ((now - lastTouch) > IDLE_BEFORE_DECAY &&
      (now - lastDecay) >= DECAY_INTERVAL) {
    lastDecay = now;
    mood -= DECAY_AMOUNT;

    if (mood < 0) mood = 0;

    Serial.print("Lonely... Mood = ");
    Serial.println(mood);
  }

  // Brief white flash feedback
  if (drawFlash()) return;

  // NORMAL MODE: white breathing light
  showColor(255, 255, 255, breathe(2000));
}

// ===================== COUNTDOWN =====================
void updateCountdown() {
  unsigned long elapsed = millis() - stateStart;

  if (elapsed >= COUNTDOWN_MS) {
    score = 0;
    gameStart = millis();
    enterGreen();
    return;
  }

  // Blink white every 500 ms
  if ((elapsed / 500) % 2 == 0) {
    setColor(255, 255, 255);
  }
  else {
    setColor(0, 0, 0);
  }
}

// ===================== GREEN LIGHT =====================
void updateGreen() {
  unsigned long now = millis();

  if (btnJustPressed) {
    score++;

    Serial.print("+1! Score = ");
    Serial.println(score);

    startFlash(60, 255, 255, 255);
  }

  if (gameTimeUp()) {
    endGame();
    return;
  }

  if ((now - stateStart) >= phaseLen) {
    enterRed();
    return;
  }

  if (drawFlash()) return;

  setColor(0, 255, 0);
}

// ===================== RED LIGHT =====================
void updateRed() {
  unsigned long now = millis();
  unsigned long elapsed = now - stateStart;

  // Penalize holding the doll during red
  if (btnPressed &&
      elapsed > RED_GRACE &&
      (lastPenalty == 0 ||
       (now - lastPenalty) > PENALTY_REPEAT)) {
    lastPenalty = now;

    score -= PENALTY_POINTS;
    if (score < 0) score = 0;

    Serial.print("Oops! -");
    Serial.print(PENALTY_POINTS);
    Serial.print(" points. Score = ");
    Serial.println(score);

    startFlash(500, 255, 255, 255);
  }

  if (gameTimeUp()) {
    endGame();
    return;
  }

  if (elapsed >= phaseLen) {
    enterGreen();
    return;
  }

  if (drawFlash()) return;

  setColor(255, 0, 0);
}

// ===================== RESULT ANIMATION =====================
void updateResult() {
  unsigned long elapsed = millis() - stateStart;

  // Return to normal mode after 5 seconds
  if (elapsed >= RESULT_MS) {
    state = MOOD;
    lastTouch = millis();
    lastDecay = millis();
    holdHandled = true;
    return;
  }

  if (won) {
    // WIN: rainbow animation
    uint8_t r, g, b;
    wheel((uint8_t)((millis() / 6) % 256), r, g, b);
    setColor(r, g, b);
  }
  else {
    // LOSE: red breathing light
    showColor(255, 0, 0, breathe(1500));
  }
}

// ===================== SETUP =====================
void setup() {
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_BTN, INPUT_PULLUP);

  Serial.begin(9600);
  randomSeed(analogRead(A0));

  lastTouch = millis();
  lastDecay = millis();

  // Begin with LED off; normal mode will breathe white
  setColor(0, 0, 0);

  Serial.println(
    "Interactive Toy ready! Pet me! "
    "Hold for 1.5 seconds to start Red Light, Green Light."
  );
}

// ===================== MAIN LOOP =====================
void loop() {
  updateButton();

  switch (state) {
    case MOOD:
      updateMood();
      break;

    case COUNTDOWN:
      updateCountdown();
      break;

    case GREEN:
      updateGreen();
      break;

    case RED:
      updateRed();
      break;

    case RESULT:
      updateResult();
      break;
  }
}
