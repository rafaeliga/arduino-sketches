#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

AndroidAccessory acc("Manufacturer",
  "Model",
  "Description",
  "1.0",
  "http://example.com",
                "0000000012345678");
                
int redPin = 11;
int greenPin = 10;
int bluePin = 9;

void setup()
{
  // set communiation speed
  Serial.begin(115200);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  acc.powerOn();
}
 
void loop()
{
  byte msg[2];
  
  if (acc.isConnected()) {
    int len = acc.read(msg, sizeof(msg), 3); // read data into msg variable
    if (len > 0) {
      setColor(msg[0], msg[1], msg[2]);
    }
  }
  else
  {
    setColor(0, 0, 0); // OFF
  }
}

void setColor(int red, int green, int blue)
{
analogWrite(redPin, 255-red);
analogWrite(greenPin, 255-green);
analogWrite(bluePin, 255-blue);
}

