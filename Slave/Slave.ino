#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 6
#define TxD 7
 
#define DEBUG_ENABLED  1
 
SoftwareSerial blueToothSerial(RxD,TxD);
 
int redPin = 11;
int greenPin = 10;
int bluePin = 9;

char recvChar;
String recvBuf;
String recvAddr;
String recvInq;
 
void setup() 
{ 
  Serial.begin(9600);
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  setupBlueToothConnection();
 
 
 //wait 1 sec and flush the serial buffer
  delay(1000);
  Serial.flush();
  blueToothSerial.flush();
  Serial.println(" ");
  Serial.println("restarted....");
  Serial.println("Starting BTScan....");
} 
 
void loop() 
{ 
  btScan();
} 
 
void setupBlueToothConnection()
{
  blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
  blueToothSerial.println("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
  blueToothSerial.println("\r\n+STNA=SeeedBTSlave\r\n"); //set the bluetooth name as "SeeedBTSlave"
  blueToothSerial.println("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  blueToothSerial.println("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
  delay(2000); // This delay is required.
  blueToothSerial.println("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000); // This delay is required.
  blueToothSerial.flush();
}

void setColor(int red, int green, int blue)
{
analogWrite(redPin, 255-red);
analogWrite(greenPin, 255-green);
analogWrite(bluePin, 255-blue);
}

void btScan()  //scan for devices
{
  if (blueToothSerial.available()) {  
    setColor(0, 0, 255);
    Serial.println("AVAIL:");
    recvChar = blueToothSerial.read();
    recvBuf += recvChar;
    if (((recvChar > 47) && (recvChar < 58)) || ((recvChar > 64) && (recvChar < 71)))  // get numbers and A-F only
    {
      recvAddr += recvChar;
      if(recvAddr.length()==12)
      {
        Serial.println(recvAddr);
        recvAddr = "";
        blueToothSerial.flush();
      }
    }
  }
}

