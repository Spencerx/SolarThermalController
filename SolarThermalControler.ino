#include "config.h"

int powerOnDelayNetwork = INITIALIZE_DELAY_NETWORK_MINUTES;
bool doDebug = DO_DEBUG_MESSAGES;

void setup() {
  // put your setup code here, to run once:
  // first thing is to stop
  Serial.begin(57600);
  pinMode(RELAIS_PIN, OUTPUT);
  if (doDebug){
    Serial.println("Arduino started. ");
    if(powerOnDelayNetwork>0){
        Serial.print("Initializing delay of ");
        Serial.print(powerOnDelayNetwork);
        Serial.println(" Minutes to wait for network.");
    }else{
      Serial.println("Not waiting for network");
    }
  }// end of doDebug


  delay(powerOnDelayNetwork * 1000 * 60); // in minutes!
}

void loop() {
  // put your main code here, to run repeatedly:

}
