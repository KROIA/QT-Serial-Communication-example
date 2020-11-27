const byte xPoti = A0;
const byte yPoti = A1;

void setup() {
  Serial.begin(115200);
  pinMode(xPoti,INPUT);
  pinMode(yPoti,INPUT);
}

void loop() {
  Serial.print(analogRead(xPoti));
  Serial.print(" ");
    
  Serial.print(analogRead(yPoti));
  Serial.print(" ");

  Serial.print(0);
  Serial.print("\n");
}
