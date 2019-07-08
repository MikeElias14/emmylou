int posPin = 5;
int pos = 0;

void setup(){
  Serial.begin(9600);
}

void loop(){
  pos= analogRead(posPin);
  if(pos){
    Serial.println(pos);
    delay(100);
  }
}
