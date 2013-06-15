  /*
* Test Arduino Manager for iPad / iPhone / Mac
 *
 * A simple test program to show the Arduino Manager
 * features.
 *
 * Author: Fabrizio Boco - fabboco@gmail.com
 *
 * Version: 1.9.1
 *
 * 01/09/2013
 *
 * All rights reserved
 *
 */

/*
*
 * IOSController library, IOSControllerWiFi library (“The Software”) and the related documentation (“The Documentation”) are supplied to you 
 * by the Author in consideration of your agreement to the following terms, and your use or installation of The Software and the use of The Documentation 
 * constitutes acceptance of these terms.  
 * If you do not agree with these terms, please do not use or install The Software.
 * The Author grants you a personal, non-exclusive license, under author's copyrights in this original software, to use The Software. 
 * Except as expressly stated in this notice, no other rights or licenses, express or implied, are granted by the Author, including but not limited to any 
 * patent rights that may be infringed by your derivative works or by other works in which The Software may be incorporated.
 * The Software and the Documentation are provided by the Author on an "AS IS" basis.  THE AUTHOR MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT 
 * LIMITATION THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING THE SOFTWARE OR ITS USE AND OPERATION 
 * ALONE OR IN COMBINATION WITH YOUR PRODUCTS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, 
 * REPRODUCTION AND MODIFICATION OF THE SOFTWARE AND OR OF THE DOCUMENTATION, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE), 
 * STRICT LIABILITY OR OTHERWISE, EVEN IF THE AUTHOR HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <Servo.h>
#include <IOSController.h>
//#include <avr/wdt.h>

/*
 *
 * Ethernet Library configuration
 *
 */
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // MAC Address assigned to the board

/*
*
 * IP info
 * 
 * Using DHCP these parameters are not needed
 */
IPAddress ip(192,168,1, 220);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

/*
 *
 * Initialize the Ethernet server library
 */
EthernetServer server(80);   // Messages received on port 80

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
#define CHIPSELECT 4
/**
 *
 * Other initializations
 *
 */
#define YELLOWLEDPIN 8
int yellowLed = HIGH;

#define TEMPERATUREPIN 0
float temperature;

#define LIGHTPIN 1
int light;

#define SERVOPIN 9
Servo servo;
int servoPos;


#define CONNECTIONPIN 7
int connectionLed = LOW;

#define POTENTIOMETERPIN 2
int pot;

/*
 *
 * Prototypes of IOSController’s callbacks
 *
 *
 */
void doWork();
void doSync(char *variable);
void processIncomingMessages(char *variable, char *value);
void processOutgoingMessages();
void processAlarms(char *variable);
void deviceConnected();
void deviceDisconnected();


/*
 *
 * IOSController Library initialization
 *
 */
#ifdef ALARMS_SUPPORT 
  IOSController iosController(&server,&doWork,&doSync,&processIncomingMessages,&processOutgoingMessages,&processAlarms,&deviceConnected,&deviceDisconnected);
#else
  IOSController iosController(&server,&doWork,&doSync,&processIncomingMessages,&processOutgoingMessages,&deviceConnected,&deviceDisconnected);
#endif

void setup()
{
  Serial.begin(9600);

  /*
   * SD Card inizialization
   *
   *
   * On the Ethernet Shield, CS is pin 4. It's set as an output by default.
   * Note that even if it's not used as the CS pin, the hardware SS pin 
   * (10 on most Arduino boards, 53 on the Mega) must be left as an output 
   * or the SD library functions will not work. 
  */
  pinMode(10, OUTPUT);     // change this to 53 on a mega
  digitalWrite(10, HIGH);  // but turn off the W5100 chip!
  
  if (!SD.begin(CHIPSELECT))  {
    
    Serial.println("SD init failed.");
  } else {
    
    Serial.println("Card present."); 
  }

  /*
   * Start the Ethernet connection and the server
   * ATTENTION: Ethernet Library provided with Arduino 1.0 has the subnet and gateway swapped respect to the documentation
   *
   */
  Ethernet.begin(mac, ip, subnet, gateway);  

  server.begin();
  
  /**
   *
   * Other initializations
   *
   */
   //iosController.setNTPServerAddress(IPAddress(64,147,116,229));
   

  // Yellow LED on
  pinMode(YELLOWLEDPIN,OUTPUT);
  digitalWrite(YELLOWLEDPIN,yellowLed);


  // Servo position at 90 degrees 
  servo.attach(SERVOPIN);
  servoPos = 90;
  servo.write(servoPos);

  // Red LED OFF
  pinMode(CONNECTIONPIN,OUTPUT);
  digitalWrite(CONNECTIONPIN,connectionLed);
}


/**
 * 
 * Standard loop function
 *
 */
void loop()
{
  //iosController.loop();
  iosController.loop(500);
}

/**
*
*
* This function is called periodically and its equivalent to the standard loop() function
*
*/
void doWork() {

  temperature = getVoltage(TEMPERATUREPIN);  //getting the voltage reading from the temperature sensor
  temperature = (temperature - 0.5) * 100;  // converting from 10 mv per degree with 500 mV offset
                                            // to degrees ((voltage – 500mV) times 100  
  digitalWrite(YELLOWLEDPIN,yellowLed);                                            

  servo.write(servoPos);

  light = analogRead(LIGHTPIN);
  
  pot = analogRead(POTENTIOMETERPIN);
}

/**
*
*
* This function is called when the ios device connects and needs to initialize the position of switches and knobs
*
*/
void doSync (char *variable) {

  if (strcmp(variable,"Knob1")==0) {

    iosController.writeMessage(variable,map(servo.read(),0,180,0,1023));
  }

  if (strcmp(variable,"S1")==0) {

    iosController.writeMessage(variable,digitalRead(YELLOWLEDPIN));
  }
    
  if (strcmp(variable,"Msg")==0) {
    
    iosController.writeTxtMessage("Msg","Hello, I'm your Arduino board");
  } 
  
}

/**
*
*
* This function is called when a new message is received from the iOS device
*
*/
void processIncomingMessages(char *variable, char *value) {

  if (strcmp(variable,"S1")==0) {

    yellowLed = atoi(value);
  }

  if (strcmp(variable,"Knob1")==0) {

      servoPos = atoi(value);  
      servoPos = map(servoPos, 0, 1023, 0, 180);      
  }

  if (strcmp(variable,"Push1") == 0) {
    
    iosController.temporaryDigitalWrite(CONNECTIONPIN,LOW,500);
  }

#ifdef  SD_SUPPORT   
  if (strcmp(variable,"SD")==0) {
    
    iosController.sendFileList();
  }

  if (strcmp(variable,"SDDL")==0) {
    
    iosController.sendFile(value);
  }
#endif

  if (strcmp(variable,"Cmd")==0) {
    
    Serial.print("Command ");
    Serial.println(value);
  } 

}

/**
*
*
* This function is called periodically and messages can be sent to the iOS device
*
*/
void processOutgoingMessages() {

  iosController.writeMessage("T",temperature);

  iosController.writeMessage("L",light);

  iosController.writeMessage("Led13",yellowLed);
  
  iosController.writeMessage("Pot",pot);
}

/**
*
*
* This function is called when a Alarm is fired
*
*/
void processAlarms(char *alarm) {
  
  Serial.print(alarm);
  Serial.println(" fired");
  
  servoPos = 0;
}

/**
*
*
* This function is called when the iOS device connects
*
*/
void deviceConnected () {

  digitalWrite(CONNECTIONPIN,HIGH);
  
  iosController.writeTxtMessage("Msg","Hello, I'm your Arduino board");
  
  Serial.println("Device connected");
}

/**
*
*
* This function is called when the iOS device disconnects
*
*/
void deviceDisconnected () {

  digitalWrite(CONNECTIONPIN,LOW);
}

/**
*  
* Auxiliary functions
*
*/

/*
 * getVoltage() – returns the voltage on the analog input defined by pin
 * 
 */
float getVoltage(int pin) {

  return (analogRead(pin) * .004882814);  // converting from a 0 to 1023 digital range
                                          // to 0 to 5 volts (each 1 reading equals ~ 5 millivolts
}
