#include "config.h"
#include "SensorW1.h"

int powerOnDelayNetwork = INITIALIZE_DELAY_NETWORK_MINUTES;
bool doDebug = DO_DEBUG_MESSAGES;

OneWire ds(W1_BUS_PIN);  // on pin x (a 4.7K resistor is necessary to 5V.)
short Nsensors;
boolean initialRun = true;
boolean currentElectricalHeaterStatus = false;

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

    //28 9C A0 1D 7 0 0 3E
    byte sensorAddress[8] = {0x28, 0x9C, 0xA0, 0x1D, 0x07, 0x00, 0x00, 0x3E};
    SensorObjects[0].SensorW1ini(sensorAddress, "hotwatter",100 );
    // HOT WAtter temperature. 
    

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

  // by now we have the temperature. let's check then if we need to start the electrical heater or not
  signed int hotWatterTemp = SensorObjects[0].getValue();

  if (doDebug){
    Serial.print("=== Current Hot Watter Temperature = ");
    Serial.println(hotWatterTemp);
    
    Serial.print("=== Current Electrical Heater Status = ");
    Serial.println(currentElectricalHeaterStatus);

    Serial.print("=== Battle: ");
    Serial.print(hotWatterTemp/100);
    Serial.print("C vs ");
    Serial.print((HOTWATTER_MIN_TEMP)/100);
    Serial.println("C.");
  }
  
  if (currentElectricalHeaterStatus){
    // currently the heater is ON
    if (hotWatterTemp > (HOTWATTER_MIN_TEMP+HOTWATTER_MIN_HYSTERESIS)){
      // Watter is already too hot. we need to turn it OFF
      if (doDebug){
        Serial.print("=== ===> Current Hot Watter Temperature is too hot. let's turn off the Heater: ");
        Serial.print(hotWatterTemp/100);
        Serial.print("C > ");
        Serial.print((HOTWATTER_MIN_TEMP+HOTWATTER_MIN_HYSTERESIS)/100);
        Serial.println("C.");
        
      }
      currentElectricalHeaterStatus = false;
    }
  }else{
    // currently the heater is OFF
    if (hotWatterTemp < (HOTWATTER_MIN_TEMP-HOTWATTER_MIN_HYSTERESIS)){
      // we need to turn it ON
      if (doDebug){
        Serial.print("=== ===> Current Hot Watter Temperature is too cold. let's turn ON the Heater: ");
        Serial.print(hotWatterTemp/100);
        Serial.print("C < ");
        Serial.print((HOTWATTER_MIN_TEMP-HOTWATTER_MIN_HYSTERESIS)/100);
        Serial.println("C.");
        
      }
      currentElectricalHeaterStatus = true;
    }
  }
  digitalWrite(RELAIS_PIN, currentElectricalHeaterStatus);

  
  if (doDebug){
    Serial.println("--- Normal Loop ENDS here.");
  }
  int delayMinutes = 1;
  //delay(delayMinutes*60*1000); // wait x minutes.
  delay(5000);
} // end of loop.





