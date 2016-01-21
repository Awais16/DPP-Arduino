#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
 
// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 10   //temp
#define I0 A0      //accelro x
#define I1 A1     //accelro y
#define I2 A2    //carbon mono
 
// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.

float temperature;
const int analogInPin1 = I0; // Analog input pin that the Accelerometer's first pin is attached to
const int analogInPin2 = I1;
const int analogInPin3 = I2;

int xAcceleration=0;
int yAcceleration=0;

int carbonmonoxide=0;
int carbondioxide=0;
 
void setup(void)
{
  // start serial port
  //Serial.begin(9600);
  
  // Start up the library
  sensors.begin(); //wire one: for temp sensor
  Wire.begin();        // join i2c bus (address optional for master)
}
 
 
void loop(void)
{
  
  getTemperature();
  getAcceleration(); 
  getCarbonMonoxide();
  getCarbonDiOxide();
  sendAll();
  
}

void sendAll(){
  
  
  String comm="a";
  comm=comm+xAcceleration;
  comm=comm+",";
  comm=comm+yAcceleration;
  comm=comm+"e";
  
  Serial.begin(115200);
  Serial.print(comm);
  Serial.end();
  delay(100);
    
  
  comm="s";
  comm=comm+temperature;
  comm=comm+",";
  comm=comm+carbonmonoxide;
  comm=comm+",";
  comm=comm+carbondioxide;
  comm=comm+";";
  Serial.begin(115200);
  Serial.print(comm);
  Serial.end();

}

boolean i2cFound=false;
byte I2cDeviceAddress;

void getCarbonDiOxide(){
  
  if(i2cFound){
   //Serial.println(I2cDeviceAddress,HEX);
  
  //i2c command from specification she
  byte error;
  /*Wire.write("Start");
  //Wire.beginTransmission(byte(0x68)); // transmit to device #44 (0x2c)
  Wire.write(byte(0xD0));                            // device address is specified in datasheet
  Wire.write(byte(0x22));            // sends instruction byte  
  Wire.write(byte(0x00));
  Wire.write(byte(0x08));
  Wire.write(byte(0x2A));
  Wire.write("Stop");
  //error=Wire.endTransmission();     // stop transmitting
  */
  
  Wire.beginTransmission(byte(0x68)); // transmit to device #4
  //Wire.write("Start");
  //Wire.write(byte(0xD0));                            // device address is specified in datasheet
  Wire.write(byte(0x22));            // sends instruction byte  
  Wire.write(byte(0x00));
  Wire.write(byte(0x08));
  Wire.write(byte(0x2A));
  //Wire.write("Stop");
  // sends one byte  
  error=Wire.endTransmission(); 
  
  
  //Serial.println("transmitted");
   if (error == 0)
   {
    //Serial.println("Success");
   }else if(error==1){
   	//Serial.println("data too long to fit in transmit buffer");
         carbondioxide=-1;
   }else if (error=2){
   	//Serial.println("received NACK on transmit of address");
        carbondioxide=-2;
   }else if (error=3){
//   	Serial.println("received NACK on transmit of data");
        carbondioxide=-3;
   }else if (error=4){
//   	Serial.println("Other error");
         carbondioxide=-4;
   }
   	
  
  /*Wire.requestFrom(2, 6);    // request 6 bytes from slave device #2
  while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }*/
  
  //delay(20); // wait 20ms for sensor to reply
  //scanI2C();
  
  
  delay(20);
  //Serial.println(Wire.requestFrom(104, 4));
  Wire.requestFrom(104, 4);
  /*
  while(Wire.available())    // slave may send less than requested
  { 
  	byte c = Wire.read();    //operation status, where bit 0 tells us if the read command was successfully executed.
  	Serial.println(c,HEX); 
//  	int value =Wire.read(); //
//    Serial.println(value);         //shuold be value of co2 from sensor
//    c = Wire.read();
//    Serial.println(value,HEX);         //check sum of above bytes98
    
  }*/
  if(Wire.available()){
    byte c = Wire.read();    //operation status, where bit 0 tells us if the read command was successfully executed.
//    Serial.println(c,BIN);
    
    byte high = Wire.read(); //The 2:nd and 3:rd byte will contain CO2 value hi byte and CO2 value low byte.
    byte low=Wire.read();//low byte
    //Serial.println(high,HEX);
    //Serial.println(low,HEX);
    
    byte checksum=Wire.read();//The 4:th byte contains checksum
    //Serial.println(low,HEX);
    carbondioxide=bitShiftCombine(high,low);
    
  }
  delay (20); //atleast
   
  }else{
    scanI2C();
  }
    
}

int bitShiftCombine( byte x_high, byte x_low)
{
  int combined; 
  combined = x_high;              //send x_high to rightmost 8 bits
  combined = combined<<8;         //shift x_high over to leftmost 8 bits
  combined |= x_low;              //logical OR keeps x_high intact in combined and fills in                                                             //rightmost 8 bits
  return combined;
}

void scanI2C(){
  byte error, address;
  int nDevices;
 
  //Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      //Serial.print("I2C device found at address 0x");
      if (address<16){
        //Serial.print("0");
      }
//      Serial.print(address,HEX);
//      Serial.println("  !");
      nDevices++;
      I2cDeviceAddress=address;
      break;
    }
    else if (error==4)
    {
//      Serial.print("Unknow error at address 0x");
//      if (address<16){
//        Serial.print("0");
//      }
//      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0){
//    Serial.println("No I2C devices found\n");
    i2cFound=false; //will scan again;
    delay(5000); //wait 5 seconds for next scan
  }else{
//    Serial.println("done\n");
    i2cFound=true;
  }
	
}




void getCarbonMonoxide(){
  
  carbonmonoxide= analogRead(analogInPin3);
  delay(100); //TODO: takecare of this.
}

void getAcceleration(){
  int sensorValue1 = analogRead(analogInPin1);
  int sensorValue2 = analogRead(analogInPin2);
  
  // map it to the range of the analog out:
  xAcceleration= map(sensorValue1, 0, 1023, 0, 255);
  yAcceleration = map(sensorValue2, 0, 1023, 0, 255); 
  
}

void getTemperature(){
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  //Serial.print(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Serial.println("DONE");
  
  //Serial.print("Temperature:"); // for Device 1 is: 
  temperature=sensors.getTempCByIndex(0);
  //Serial.println(temperature); // Why "byIndex"? 
    // You can have more than one IC on the same bus. 
    // 0 refers to the first IC on the wire
}

