#include <CapacitiveSensor.h>

CapacitiveSensor   string = CapacitiveSensor(4, 2); // pin 2 is sensor pin

void setup()                    
{
   Serial.begin(9600);
}

void loop()                    
{
    long total1 =  string.capacitiveSensor(30);

    Serial.println(total1);                  // print sensor output 1

    delay(100);                             // arbitrary delay to limit data to serial port 
}
