
/***************************************************************************
  This program was made by Daniel Chacón Mora, student of Electrical Engineering, Universidad de Costa Rica. It combines SPI and I2C protocols. The main base for SPI was written by Thiago Barros for BlueDot UG (haftungsbeschränkt. This sketch was written for the Bosch Sensor BME280.
  The BME280 is a MEMS device for measuring temperature, humidity and atmospheric pressure. 
  The reason behind this project was a College competition where the measure of the air speed was requiered.
 ***************************************************************************/ 
//#include <TinyGPS.h>
#include <Wire.h>
#include "BlueDot_BME280.h"

BlueDot_BME280 bme1;                                     //Object for Sensor 1
BlueDot_BME280 bme5;
int bme1Detected = 0;                                                       
int bme5Detected = 0;
int LEDROJO1 = 2;
int LEDAZUL2 = 3;
float humedad1;
float humedad2;
float temperatura1; 
float temperatura2;
float presion1; //presion interior
float presion2;//presion exterior
float deltapres;
float altura1;
float altura2;
float presion_vapor;
float presion_aireseco;
float deltaalt; //cambio de las alturas
float long1; //datos de GPS
float lat1; // datos de GPS
float long2; //datos de GPS
float lat2; // datos de GPS
float airspeed;
float groundspeed;
float windspeed;
float density; //densidad del aire


void setup() {
  Serial.begin(9600);
  Serial.println(F("TUCAN II Weather Station"));

  //Choose between Software and Hardware SPI protocol for all sensors.  I2C para sensor 5
    bme1.parameter.communication = 1;                    //Hardware SPI for Sensor 1 (bme1)
    bme5.parameter.communication = 0;
    
  //Each sensor must have a different Chip Select pin.
    bme1.parameter.SPI_cs = 4;                          //Chip Select Pin for Sensor 1
    bme5.parameter.I2CAddress = 0x76;
   
    bme1.parameter.SPI_mosi = 11;                       //The MOSI signal must be connected to the same pin for all sensors
    bme1.parameter.SPI_miso = 12;                       //The MISO signal must be connected to the same pin for all sensors
    bme1.parameter.SPI_sck = 13;                       //The SCK signal must be connected to the same pin for all sensors
   
   
  //Now choose on which mode your device will run
  //0b00:     In sleep mode no measurements are performed, but power consumption is at a minimum
  //0b01:     In forced mode a single measured is performed and the device returns automatically to sleep mode
  //0b11:     In normal mode the sensor measures continually (default value)
  
    bme1.parameter.sensorMode = 0b11;                    //Setup Sensor mode for Sensor 1
    bme5.parameter.sensorMode = 0b11;
     
  //The IIR (Infinite Impulse Response) filter suppresses high frequency fluctuations
  //In short, a high factor value means less noise, but measurements are also less responsive
  //0b000:      factor 0 (filter off)
  //0b001:      factor 2
  //0b010:      factor 4
  //0b011:      factor 8
  //0b100:      factor 16 (default value)

    bme1.parameter.IIRfilter = 0b100;                   //IIR Filter for Sensor 1
    bme5.parameter.IIRfilter = 0b100;
 
  //Next you'll define the oversampling factor for the humidity measurements
 
  //0b000:      factor 0 (Disable humidity measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)

    bme1.parameter.humidOversampling = 0b101;            //Humidity Oversampling for Sensor 1
    bme5.parameter.humidOversampling = 0b101;
    //Humidity Oversampling for Sensor 4
 
  //Now define the oversampling factor for the temperature measurements

  //0b000:      factor 0 (Disable temperature measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)

    bme1.parameter.tempOversampling = 0b101;              //Temperature Oversampling for Sensor 1
    bme5.parameter.tempOversampling = 0b101;
    //Temperature Oversampling for Sensor 4

  //Finally, define the oversampling factor for the pressure measurements
  
  //0b000:      factor 0 (Disable pressure measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)  

    bme1.parameter.pressOversampling = 0b101;             //Pressure Oversampling for Sensor 1
    bme5.parameter.pressOversampling = 0b101;
    //Pressure Oversampling for Sensor 4 
 
  //For precise altitude measurements please put in the current pressure corrected for the sea level
  //On doubt, just leave the standard pressure as default (1013.25 hPa);
  
    bme1.parameter.pressureSeaLevel = 1013.25;            //default value of 1013.25 hPa (Sensor 1)
    bme5.parameter.pressureSeaLevel = 1013.25;
  

  //Also put in the current average temperature outside (yes, really outside!)
  //For slightly less precise altitude measurements, just leave the standard temperature as default (15°C);
  
    bme1.parameter.tempOutsideCelsius = 15;               //default value of 15°C
    bme5.parameter.tempOutsideCelsius = 15;
 
    bme1.parameter.tempOutsideFahrenheit = 59;            //default value of 59°F
    bme5.parameter.tempOutsideFahrenheit = 59;
    //default value of 59°F

    
  //Chequear si los sensores se detectan
  if (bme1.init() != 0x58)
  {    
    Serial.println(F("Ops! First BME280 Sensor not found!"));
    bme1Detected = 0;
  }
  else
  {
    Serial.println(F("First BME280 Sensor detected!"));
    bme1Detected = 1;
  }

  if (bme5.init() != 0x60)
  {    
    Serial.println(F("Ops! Second BME280 Sensor not found!"));
    bme5Detected = 0;
  }
  else
  {
    Serial.println(F("Second BME280 Sensor detected!"));
    bme5Detected = 1;
  }
  if ((bme1Detected == 0)&(bme5Detected == 0))
  {
    Serial.println();
    Serial.println();
    Serial.println(F("Troubleshooting Guide"));
    Serial.println(F("*************************************************************"));
    Serial.println(F("1. Let's check the basics: Are the VCC and GND pins connected correctly? If the BME280 is getting really hot, then the wires are crossed."));
    Serial.println();
    Serial.println(F("2. Did you connect the Chip Select (CS) pin from each sensor to a different digital pin on the Arduino?"));
    Serial.println();
    Serial.println(F("3. Are you using Hardware SPI? In this case, the MOSI, MISO and SCK pins from your Arduino are on the ICSP header."));
    Serial.println();
    Serial.println(F("4. Are you using Software SPI instead? In this case, the MOSI, MISO and SCK pins are regular digital pins choosen by you."));
    Serial.println();
    Serial.println(F("5. Did you connect the SDI pin from each sensor to the same MOSI pin from your Arduino? All sensors must share the same MOSI pin."));
    Serial.println();
    Serial.println(F("6. Did you connect the SDO pin from each BME280 sensor to the same MISO pin from your Arduino? All sensors must share the same MISO pin."));
    Serial.println();
    Serial.println(F("7. And finally, did you connect the SCK pin from each BME280 sensor to the same SCK pin from your Arduino? Yes, all sensors must share the same SCK pin."));
    Serial.println();


    Serial.println();
    
    while(1);
  }
    
  Serial.println();
  Serial.println();

}
//Medicion como tal
void loop() {
 
  
  Serial.print(F("Duration in Seconds:  "));
  Serial.println(float(millis())/1000);
  pinMode(LEDROJO1, OUTPUT);
  pinMode(LEDAZUL2, OUTPUT);

//Read Sensor 1
  if (bme1Detected)
  {
    
    digitalWrite(LEDROJO1, HIGH);
    delay(50);
    digitalWrite(LEDROJO1, LOW);
    delay(50);
    Serial.println(F("------------------------------------------------"));
    Serial.print(F("Temperature Sensor 1 [°C]:\t\t")); 
    temperatura1 = bme1.readTempC();
    Serial.println(temperatura1);
    Serial.print(F("Humidity Sensor 1 [%]:\t\t\t"));
    humedad1 = bme1.readHumidity();
    Serial.println(humedad1);
    Serial.print(F("Pressure Sensor 1 [hPa]:\t\t")); 
    presion1= bme1.readPressure();
    Serial.println(presion1);
    Serial.print(F("Altitude Sensor 1 [m]:\t\t\t")); 
    altura1 = bme1.readAltitudeMeter();
    Serial.println(altura1);
    Serial.println(F("****************************************"));
    digitalWrite(LEDAZUL2, HIGH);
    delay(50);
    digitalWrite(LEDAZUL2, LOW);
    delay(50);    
  }

  else
  {
    Serial.print(F("Temperature Sensor 1 [°C]:\t\t")); 
    Serial.println(F("Null"));
    //Serial.print(F("Temperature Sensor 1 [°F]:\t\t")); 
    //Serial.println(F("Null"));
    Serial.print(F("Humidity Sensor 1 [%]:\t\t\t")); 
    Serial.println(F("Null"));
    Serial.print(F("Pressure Sensor 1 [hPa]:\t\t")); 
    Serial.println(F("Null"));
    Serial.print(F("Altitude Sensor 1 [m]:\t\t\t")); 
    Serial.println(F("Null"));
    //Serial.print(F("Altitude Sensor 1 [ft]:\t\t\t")); 
    //Serial.println(F("Null"));
    Serial.println(F("****************************************"));   
  }

   if (bme5Detected)
  {
     digitalWrite(LEDROJO1, HIGH);
    delay(50);
    digitalWrite(LEDROJO1, LOW);
    delay(50);
    Serial.print(F("Temperature Sensor 2 [°C]:\t\t"));
    temperatura2 = (float(bme5.readTempC())); 
    Serial.println(temperatura2);
    Serial.print(F("Humidity Sensor 2 [%]:\t\t\t")); 
    humedad2 = (float(bme5.readHumidity()));
    Serial.println(humedad2);
    Serial.print(F("Pressure Sensor 2 [hPa]:\t\t")); 
    presion2 = (float(bme5.readPressure()));
    Serial.println(presion2);
    Serial.print(F("Altitude Sensor 2 [m]:\t\t\t")); 
    altura2 = (float(bme5.readAltitudeMeter()));
    Serial.println(altura2);
    Serial.println(F("------------------------------------------------"));
    digitalWrite(LEDAZUL2, HIGH);
    delay(50);
    digitalWrite(LEDAZUL2, LOW);
    delay(50);
        
  }

  else
  {
    Serial.print(F("Temperature Sensor 5 [°C]:\t\t")); 
    Serial.println(F("Null"));
    //Serial.print(F("Temperature Sensor 3 [°F]:\t\t")); 
    //Serial.println(F("Null"));
    Serial.print(F("Humidity Sensor 5 [%]:\t\t\t")); 
    Serial.println(F("Null"));
    Serial.print(F("Pressure Sensor 5 [hPa]:\t\t")); 
    Serial.println(F("Null"));
    Serial.print(F("Altitude Sensor 5 [m]:\t\t\t")); 
    Serial.println(F("Null"));
    //Serial.print(F("Altitude Sensor 3 [ft]:\t\t\t")); 
    //Serial.println(F("Null"));
    Serial.println(F("****************************************"));   
  }
  
   presion_vapor = ((6.1078*pow(10,((7.5*temperatura2)/(temperatura2+237.32))))*(humedad2/100));
   presion_aireseco= (presion2-presion_vapor);
   density = ((presion_aireseco/(287.058*(temperatura2+273.15)))+(presion_vapor/(461.495*(temperatura2+273.15)))); //calculo de la densidad del aire
   deltapres = abs(presion1-presion2);
   airspeed=((sqrt((deltapres)/(density)))-9.54); //Ecuacion de Bernoulli
   Serial.print(F("Airspeed (m/s)=")); //Print no necesario para el código final
   Serial.println(airspeed); //Print no necesario para el código final
   //Recolectar datos de longitud y latitud con GPS
   long1 = 0; 
   lat1 = 0; 
   long2 = 0; 
   lat2 = 0; 
   groundspeed = 0; //TinyGPS::distance_between(lat1,long1,lat2,long2); //formula de groundspeed. Se utiliza la fórmula del semiverseno
   //Este groundspeed no considera el cambio de altura, por lo que se debe de hacer aparte
   deltaalt =abs(altura1-altura2);
   groundspeed = 0; //sqrt(pow(groundspeed,2)+pow(deltaalt,2)); //esto es lo equivalente a calcular la magnitud del vector velocidad
   Serial.print(F("Groundspeed (m/s)="));//Print no necesario para el código final
   Serial.println(groundspeed);//Print no necesario para el código final
   windspeed = (groundspeed - airspeed);
   Serial.print(F("Wind speed (m/s)="));
   Serial.println(windspeed); //esto es lo único que se ocupa retornar de todo el programa
   Serial.println(F("------------------------"));
   Serial.println();
   Serial.println();

   delay(1000);
   
 
}
