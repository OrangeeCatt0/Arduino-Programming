void setup() {
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
}

void loop() {

  // Turn on Pin 3
  digitalWrite(3, LOW);
  delay(30);
  digitalWrite(3, HIGH);
  delay(30);

  // Turn on Pin 4
  digitalWrite(4, LOW);
  delay(30);
  digitalWrite(4, HIGH);
  delay(30);

  // Turn on Pin 5
  digitalWrite(5, LOW);
  delay(30);
  digitalWrite(5, HIGH);
  delay(30);
}