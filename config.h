#define DO_DEBUG_MESSAGES 1 // Controlls if debugging messages are shown on the serial terminal

#define RELAIS_PIN 7 // the output 5V that controlls the relais

#define CONTROLLER_ID "SolTh01" // the id of the contorller. this will be used to identify this controller

#define USE_SIMULATOR 1 // 1: turn on the simulator, for debugging and development purposses. Normal operation is 0

#define POWER_ON_HOUR 4 // the hour of the day where the electric heater has allowance to operate.
#define POWER_ON_MINUTE 30 // the Minute of the day where the electric heater has allowance to operate.
#define POWER_OFF_HOUR 6 // the hour of the day where the electric heater stops operating.
#define POWER_OFF_MINUTE 30 // the Minute of the day where the electric heater stops operating.

#define INITIALIZE_DELAY_NETWORK_MINUTES 0 // after a power off, wait this amount of minutes for the network to start

#define TYPE_DS18S20 0
#define TYPE_DS18B20 1
#define TYPE_DS18S22 2
#define TYPE_MAX31850 3
#define TYPE_knock 4
#define W1_BUS_PIN 2
