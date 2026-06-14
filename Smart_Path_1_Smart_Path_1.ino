#include <SoftwareSerial.h>
#include <DFRobot_BMI160.h>

SoftwareSerial BTserial(2, 3); // RX | TX
// Connect the HC-06 TX to the Arduino RX on pin 2. 
// Connect the HC-06 RX to the Arduino TX on pin 3 through a voltage divider.
// 
DFRobot_BMI160 bmi160; 


int echoPin = 12; // Echo Pin
int trigPin = 13; // Trigger Pin
int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance

const int VibPin = 4;
const int8_t i2c_addr = 0x69;
bool readStep = false;


#if defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_MEGA2560 || defined ARDUINO_AVR_PRO
  //interrupt number of uno and mega2560 is 0
  int pbIn = 2;
#elif ARDUINO_AVR_LEONARDO
  //interrupt number of uno and leonardo is 0
  int pbIn = 3;
#else
  int pbIn = 13;
#endif
/*the bmi160 have two interrput interfaces*/
int int1 = 1;
int int2 = 2;

void setup() {
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
pinMode(VibPin,OUTPUT);
Serial.begin(9600);
BTserial.begin(9600); // BT

  delay(500);

// Επαναφορά αισθητήρα 
  if (bmi160.softReset() != BMI160_OK) {
    Serial.println("Reset failed");
    while(1);
  }

  //set and init the bmi160 i2c address
  while (bmi160.I2cInit(i2c_addr) != BMI160_OK){
    Serial.println("i2c init fail");
    delay(500);
  }

  //set interrput number to int1 or int2
  if (bmi160.setInt(int1) != BMI160_OK){
    Serial.println("set interrput fail");
    while(1);
  }

  //set the bmi160 mode to step counter
  if (bmi160.setStepCounter() != BMI160_OK){
    Serial.println("set step fail");
    while(1);
  }
#if defined ARDUINO_AVR_UNO || defined ARDUINO_AVR_MEGA2560 || defined ARDUINO_AVR_LEONARDO || defined ARDUINO_AVR_PRO
  //set the pin in the board to connect to int1 or int2 of bmi160
  attachInterrupt(digitalPinToInterrupt(pbIn), stepChange, FALLING);
#else
  attachInterrupt(pbIn, stepChange, FALLING);
#endif
}



void loop() {

 


digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration = pulseIn(echoPin, HIGH);
//Υπολογισμός απόστασης (σε cm) βασιζόμενοι στην ταχύτητα του ήχου.
distance = duration/58.2;

if (distance >= maximumRange || distance <= minimumRange){
Serial.println("Εκτός ορίων");

delay(500);
}
else {
Serial.println(distance);
if (distance<100) {
  digitalWrite(VibPin,HIGH);
  delay(200);
  digitalWrite(VibPin,LOW);
  }

delay(500);
};

  if (readStep){
    uint16_t stepCounter = 0;
    //read step counter from hardware bmi160
    if (bmi160.readStepCounter(&stepCounter)==BMI160_OK){
      Serial.println("step counter = ");Serial.println(stepCounter);
    }
    readStep = false;
  }

  //  όλες οι τιμές του επιταχυνσιομέτρου 
  int16_t data[6] = {0}; // [0..2] Γυροσκόπιο, [3..5] Επιταχυνσιόμετρο
  
  // Λήψη όλων των δεδομένων 
  if (bmi160.getAccelGyroData(data) == 0) {
    // Εκτύπωση Γυροσκοπίου (X, Y, Z) σε deg/s (μετατροπή αν χρειάζεται)
    Serial.print("Gyro: ");
    for(int i=0; i<3; i++) {
      Serial.print(data[i] * 3.14 / 180.0); Serial.print("\t");
    }
    
    // Εκτύπωση Επιταχυνσιομέτρου (X, Y, Z) σε g 
    Serial.print("Accel: ");
    for(int i=3; i<6; i++) {
      Serial.print(data[i] / 16384.0); Serial.print("\t");
    }
    Serial.println();
  }
  delay(500); // Delay for readability (500ms)
}





void stepChange()
{
  //once the step conter is changed, the value can be read
  readStep = true;
}




