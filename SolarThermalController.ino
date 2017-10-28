#include "config.h"
#include "SensorW1.h"

int powerOnDelayNetwork = INITIALIZE_DELAY_NETWORK_MINUTES;
bool doDebug = DO_DEBUG_MESSAGES;

OneWire ds(W1_BUS_PIN);  // on pin x (a 4.7K resistor is necessary to 5V.)
short Nsensors;
boolean initialRun = true;

typedef struct {
  byte sensorAddress[8];
  String sensorLocation;
  float dv_max;
} SensorDictionaryElement;

SensorW1 SensorObjects[1];

void setup() {
  // put your setup code here, to run once:
  // first thing is to stop
  Serial.begin(250000);
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
  delay(powerOnDelayNetwork * 1000 * 60 + 500); // in minutes!
} // end of setup

void loop() {
  // put your main code here, to run repeatedly:
  if (doDebug){
    Serial.println("Loop started");
  }
  if (initialRun){
    if (doDebug){
      Serial.println("+++ Initial RUN ");
    }

    list1WireSensors(ds);
    //28 9C A0 1D 7 0 0 3E
    byte sensorAddress[8] = {0x28, 0x9C, 0xA0, 0x1D, 0x07, 0x00, 0x00, 0x3E};
    
    SensorObjects[0].SensorW1ini(sensorAddress, "hotwatter",100 );
    

    initialRun = false;

  }else{
    if (doDebug){
      Serial.println("--- Normal Loop ");
    }


  }// end of initial run
  ds.reset();
  ds.write(0xCC); //Skip ROM to broadcast to all sensors in bus.
  //delay(100);
  ds.write(0x44,1); //Start value conversion (for all sensors at the same time after Skip ROM
  delay(750); //Delay for value conversion at 12-bit resolution
  
   
  SensorObjects[0].updateValue(ds);
  SensorObjects[0].checkAllValues();
  
  if (doDebug){
    Serial.println("--- Normal Loop ENDS here.");
  }
  delay(1000); // wait 5 seconds.
} // end of loop.






void list1WireSensors(OneWire ds) {
  //Serial.println("In list1WireSensors:  --- START");
  // this function searches for sensors in the 1 Wire network and returns them as an array (of addressess)
  byte iWhile = 0;
  byte iAddr = 0;
  byte addr[8];

  boolean doLoop1WireBus = true;
  while (doLoop1WireBus ) {
    if (DO_DEBUG_MESSAGES){
      Serial.println("--- ---- looping for sensor " + String(iWhile));
    }
    if ( !ds.search(addr)) {
      doLoop1WireBus = false;
      if (DO_DEBUG_MESSAGES ) {
        Serial.println(F("No more addresses."));

        Serial.println();
      }
      ds.reset_search();
      delay(250);
      //return read1WireReturns;
      break; // breaks the while?
    } else {
      // not the last elemnt.. this is a valid one.
      if (DO_DEBUG_MESSAGES ) {
        Serial.print(F("Found a sensor with address ="));
        for ( iAddr = 0; iAddr < 8; iAddr++) {
          Serial.write(' ');
          Serial.print(addr[iAddr], HEX);
        }
        Serial.println();
      }


      if (OneWire::crc8(addr, 7) != addr[7]) {
        Serial.println("CRC is not valid!");
        //return read1WireReturns;
        break; // breaks the while?
      }


    } // end if last element.
    if (DO_DEBUG_MESSAGES){
      Serial.print(" ---> Address: ");
    }
      
      if (DO_DEBUG_MESSAGES){

        Serial.println();
      }

    //listOfSensors[i] = addr;
    iWhile++;


  } // end looping over sensors
  //Serial.print(("returning list1WireSensors"));
  delay(1000);
  return;
}

