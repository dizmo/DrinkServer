#include <SPI.h>
#include <Wire.h>
#include <Ethernet.h>

#include <Adafruit_MotorShield.h>

// Enabe debug tracing to Serial port.
#define DEBUG true
#define STATICIP true

// Here we define a maximum framelength to 64 bytes. Default is 256.
#define MAX_FRAME_LENGTH 64

#include <WebSocket.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x91, 0x8C };

#if STATICIP
byte ip[] = {192,168,1,61};
#endif

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS0 = Adafruit_MotorShield(0x60); 
Adafruit_MotorShield AFMS1 = Adafruit_MotorShield(0x61); 

Adafruit_DCMotor *myMotor[8] = {AFMS0.getMotor(3),AFMS0.getMotor(4),AFMS0.getMotor(1),AFMS0.getMotor(2),AFMS1.getMotor(1),AFMS1.getMotor(2),AFMS1.getMotor(3),AFMS1.getMotor(4)};
int directions[8] = {BACKWARD,FORWARD,BACKWARD,FORWARD,FORWARD,BACKWARD,FORWARD,BACKWARD};

int duration[8]={0,0,0,0,0,0,0,0};
int counter = 0;
int glassSizeFactor = 50; // 10 seconds
int running = 0;

// Create a Websocket server
WebSocketServer wsServer;

void onConnect(WebSocket &socket) {
  Serial.println("onConnect called");
}

// You must have at least one function with the following signature.
// It will be called by the server when a data frame is received.
void onData(WebSocket &socket, char* dataString, byte frameLength) {
  
#ifdef DEBUG
  Serial.print("Got data: ");
  Serial.write((unsigned char*)dataString, frameLength);
  Serial.println();
#endif

//Serial.println(running);
//Serial.println(frameLength);

// check that none of the motors is running
if ((running==0)||(frameLength==16)) {

int p=0;
int n;
int o;

for (int j=0; j<frameLength/2; j++){

  running=1;

  n=dataString[p]-'0';
  o=dataString[p+1]-'0';

  p=p+2;

  int selectedDuration = o;
  int selectedMotor = n;

  //Serial.println(selectedDuration);

  // stop all the motors by sending 90
  if (selectedDuration == 9) {
         for (int i=0; i < 8; i++){
             duration[i] = 0;
         }
  } else {
         duration[selectedMotor] = selectedDuration * glassSizeFactor;
         Serial.println(duration[selectedMotor]);
  }

  socket.send("ACK",3);

  }
}
  // Just echo back data for fun.
  //socket.send(dataString, strlen(dataString));
}


void onDisconnect(WebSocket &socket) {
  Serial.println("onDisconnect called");
}

void setup() {
#ifdef DEBUG  
  Serial.begin(9600);
#endif

#if STATICIP
  Ethernet.begin(mac, ip);
#else
  if (Ethernet.begin(mac) == 0) {
#if DEBUG
    Serial.println("Unable to set server IP address using DHCP");
#endif
    for(;;)
      ;
  }
#if DEBUG
  // report the dhcp IP address:
  Serial.println(Ethernet.localIP());
#endif
#endif

  wsServer.registerConnectCallback(&onConnect);
  wsServer.registerDataCallback(&onData);
  wsServer.registerDisconnectCallback(&onDisconnect);  
  wsServer.begin();
  
  delay(100); // Give Ethernet time to get ready

  AFMS0.begin();  // create with the default frequency 1.6KHz
  AFMS1.begin();  // create with the default frequency 1.6KHz
 
}

void loop() {
  
  // Should be called for each loop.
  wsServer.listen();

  for (int i=0; i < 8; i++){
    if (duration[i] > 0) {
      myMotor[i]->setSpeed(255);   // RUN SPEED
      myMotor[i]->run(directions[i]); // RUN DIRECTION
      duration[i]--;
    } else {
      myMotor[i]->run(RELEASE); // STOP 
    }
  }

  //counter++;
  //if (counter > 9) {
    Serial.print("'");
    Serial.print(duration[0]);
    Serial.print("','");
    Serial.print(duration[1]);
    Serial.print("','");
    Serial.print(duration[2]);
    Serial.print("','");
    Serial.print(duration[3]);
    Serial.print("','");
    Serial.print(duration[4]);
    Serial.print("','");
    Serial.print(duration[5]);
    Serial.print("','");
    Serial.print(duration[6]);
    Serial.print("','");
    Serial.print(duration[7]);
    Serial.println("'");
    counter = 0;

    if (running==1) {
      int check=0;
      for (int k=0; k<8; k++) {
        if (duration[k]>0) check=1;
      }
      Serial.println(check);
      if (check==0) {
        Serial.println("done");
        wsServer.send("DONE",4);
        running=0;
      }
    }
    
  //} 
 
  delay(100);
}
