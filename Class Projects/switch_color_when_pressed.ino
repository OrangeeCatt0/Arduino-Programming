const int buttonPin = 2;
// the number of the pushbutton pin
const int RledPin = 3;
// the number of the R pin
const int GledPin = 4;
// the number of the G pin
const int BledPin = 5;
// the number of the B pin
// variables will change:
int buttonState = 0; // variable for reading the pushbutton status
int ledcolor = 0;
void setup() {
// initialize the LED pin as an output:
pinMode (RledPin, OUTPUT);
pinMode(GledPin, OUTPUT);
pinMode(BledPin, OUTPUT);
// initialize the pushbutton pin as an input:
pinMode(buttonPin, INPUT);
}

void loop() {
// read the state of the pushbutton value:
buttonState = digitalRead(buttonPin);
if (buttonState == HIGH) {
ledcolor = ledcolor + 1;
delay(100);
}

if (ledcolor == 0){
digitalWrite(RledPin, HIGH);
digitalWrite(GledPin, HIGH);
digitalWrite(BledPin, HIGH);
}
else if (ledcolor == 1) {
// RED
digitalWrite(RledPin, LOW);
digitalWrite(GledPin, HIGH);
digitalWrite(BledPin, HIGH);
}
else if (ledcolor == 2) {
//Green
digitalWrite(RledPin, HIGH);
digitalWrite(GledPin, LOW);
digitalWrite(BledPin, HIGH);
}
//BLUE
else if (ledcolor == 3){
digitalWrite(RledPin, HIGH);
digitalWrite(GledPin, HIGH);
digitalWrite(BledPin, LOW);
}

else if(ledcolor == 4) {
digitalWrite(RledPin, LOW);
digitalWrite(GledPin, LOW);
digitalWrite(BledPin, HIGH);
}
//purple
else if (ledcolor == 5){
digitalWrite(RledPin, LOW);
digitalWrite(GledPin, HIGH);
digitalWrite(BledPin, LOW);
}
//cyan
else if(ledcolor == 6){
digitalWrite(RledPin, HIGH);
digitalWrite(GledPin, LOW);
digitalWrite(BledPin, LOW);
}
//white
else if(ledcolor == 7){
digitalWrite(RledPin, LOW);
digitalWrite(GledPin, LOW);
digitalWrite(BledPin, LOW);
}
else if(ledcolor == 8){
ledcolor = 0;
}
}