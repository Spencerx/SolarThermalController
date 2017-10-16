#include "config.h"
#include "SensorW1.h"

int powerOnDelayNetwork = INITIALIZE_DELAY_NETWORK_MINUTES;
bool doDebug = DO_DEBUG_MESSAGES;

OneWire ds(W1_BUS_PIN);  // on pin 5 (a 4.7K resistor is necessary)
short Nsensors;
boolean initialRun = true;

typedef struct {
  byte sensorAddress[8];
  String sensorLocation;
  float dv_max;
} SensorDictionaryElement;

SensorW1 SensorObjects[2];

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
    SensorDictionaryElement mySensor0={{0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}, "xx_sensor_0",  100};
    SensorDictionaryElement mySensor1={{0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}, "xx_sensor_1",  100};
    SensorDictionaryElement MySensorDictionary[]={mySensor0,mySensor1};
    for (short iSensor = 0; iSensor <= 1; iSensor++) {
      SensorObjects[iSensor].SensorW1ini(MySensorDictionary[iSensor].sensorAddress, MySensorDictionary[iSensor].sensorLocation,MySensorDictionary[iSensor].dv_max );
      SensorObjects[iSensor].checkAllValues();
    }

    Nsensors = findNumberOfSensorsInBus(ds);
    Nsensors =  Nsensors +1;
    if (doDebug){
      Serial.print("+++ --- We found  ");
      Serial.print(Nsensors);
      Serial.println(" sensors in the bus");
    }

    SensorDictionaryElement SensorDictionary[Nsensors]; // here we can initialize this variable knowing it's size.
    byte listOfSensors[Nsensors][8];
    list1WireSensors(listOfSensors, ds);


    for (short iSensor = 0; iSensor < Nsensors; iSensor++) {

      if (doDebug){
        Serial.print(F("--- ---> Sensor Nr. "));
        Serial.print(iSensor);
        Serial.print(F(" with Address: "));
        for (short iAddr = 0; iAddr < 8; iAddr++) {
          Serial.print(listOfSensors[iSensor][iAddr], HEX);
          Serial.print(F(" "));
        }
        Serial.println();
      }
      // ok, now we need to update SensorDictionary with the elements that we have here.

      //SensorDictionaryElement thisSensorDictionaryElement;
      //SensorDictionary[iSensor] = thisSensorDictionaryElement;
      for (short iAddr = 0; iAddr < 8; iAddr++) {
        SensorDictionary[iSensor].sensorAddress[iAddr] = listOfSensors[iSensor][iAddr];
      }
      // now let's match against our library of sensors.
      boolean foundMatch= false;
      for (short iSensorDict = 0; iSensorDict < Nsensors; iSensorDict ++) {
        // loop over MySensorDictionary and see which matches.
        short NmatchAddres = 0;
        // now loop over addr and se if it maches.
        for (short iAddr = 0; iAddr < 8; iAddr++) {
          if (MySensorDictionary[iSensorDict].sensorAddress[iAddr] == SensorDictionary[iSensor].sensorAddress[iAddr]){
            NmatchAddres  ++;
          }
        }
        // now if the sensor iSensor had a match with the library iSensorDict, then NmatchAddres  should be 8!
        if (NmatchAddres  == 8){
          //serialWriter.sendStatus("{\"message\":\"Got a match with Dic Element\", \"sensorID\": \"" + MySensorDictionary[iSensorDict].sensorLocation + "\", \"sensor\"}}");
          //Serial.print(F("--- ---- ---> Got a match with Dictionary element Nr. "));
          //Serial.println(iSensorDict);
          SensorDictionary[iSensor].dv_max = MySensorDictionary[iSensorDict].dv_max;
          SensorDictionary[iSensor].sensorLocation = MySensorDictionary[iSensorDict].sensorLocation;
          foundMatch = true;
        }else{
          // nothing wrong, continue.
        }

      }// end looping over dictionary.
      if (!foundMatch){
        Serial.print(F("--- --- ---> Did not found anything for Sensor"));
        break;// stop runtime
      }
      // at this point, if the arduino continues, is because we matched all sensors with the library. this is good :-)

      delay(500);
      //SensorObjects[iSensor].checkAllValues(); // 2DO: Why doesnt this here print anything interesting?
    } // end looping over the results.

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
  for (short iSensor = 0; iSensor < Nsensors; iSensor++) {

   // Serial.println(Nsensors);    // Chekear cantidad de sensores que se van a revisar
    SensorObjects[iSensor].updateValue(ds);
    SensorObjects[iSensor].checkAllValues();
  }
  if (doDebug){
    Serial.println("--- Normal Loop ENDS here.");
  }
  delay(1000); // wait 5 seconds.
} // end of loop.












/*
* -------------------------------------------------------------------------------------- START
* DECLARE read1WireBus function and it's return structure
*
* This function has the goal of reading through the 1 Wire bus and returning an array with
* all values for all sensors found in the array. The code has been taken from
* https://github.com/adafruit/MAX31850_OneWire/tree/master/examples/MAX31850_Vemperature
*/

byte findNumberOfSensorsInBus(OneWire ds) {
  byte Nsensors_ = 0;
  byte addr[8];
  boolean doLoop1WireBus = true;
  while (doLoop1WireBus ) {
    if ( !ds.search(addr)) {
      if (DO_DEBUG_MESSAGES ) {
        //Serial.println(F("No more addresses."));
        doLoop1WireBus = false;
        //Serial.println();
      }
      ds.reset_search();
      delay(250);
      break; // breaks the while?
    } else {
      // not the last elemnt.. this is a valid one.
      if (OneWire::crc8(addr, 7) != addr[7]) {
        break; // breaks the while?
      }
    } // end if last element.
    Nsensors_ ++;
  }
  //Serial.println("-- Inside findNumberOfSensorsInBus. Got " + String(Nsensors_) + " sensors");
  return Nsensors_;
}


void list1WireSensors(byte listOfSensors[][8], OneWire ds) {
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
      for ( iAddr = 0; iAddr < 8; iAddr ++) {
        listOfSensors[iWhile][iAddr] = addr[iAddr];
        if (DO_DEBUG_MESSAGES){
          Serial.print(listOfSensors[iWhile][iAddr], HEX);
          Serial.print(" ");
        }
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
