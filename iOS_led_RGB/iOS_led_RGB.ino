int statusLed = 12;
int powerLed = 13;
int ez1Analog = 0;

int aByte;
int flag = 0;

void setup() {
   pinMode(statusLed,OUTPUT);
   pinMode(ez1Analog,INPUT);
   Serial.begin(115200);

   pinMode(powerLed,OUTPUT);
   blinkLed( statusLed, 500 );
}

void loop() {

   if (Serial.available() > 0) {
          aByte = Serial.read();
          if ( flag == 0 ) {
            flag = 1;
            digitalWrite(powerLed, HIGH);
          } else {
            flag = 0;
            digitalWrite(powerLed, LOW);
          }
    }

   if ( flag == 1 ) {
     int val = analogRead(ez1Analog);

     if ( val > 0 ) {
         val = val / 2;
         float cm = float(val)*2.54;
         Serial.println( int(cm) ); // cm
         blinkLed( statusLed, 100);
      }
   }

}

void blinkLed(int pin, int ms) {
   digitalWrite(pin,LOW);
   digitalWrite(pin,HIGH);
   delay(ms);
   digitalWrite(pin,LOW);
   delay(ms);
}
