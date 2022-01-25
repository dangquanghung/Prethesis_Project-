#include "mq2.h"

/*library for measuring mq2 gas values in this function
 * 
 * this library includes  function:
 *              1)inits()----> for serial begin or anything you want to describe
 *              
 *              2)calibrate()------> for calibrating sensor RL resistor inside the MQ-9 should be on for 24-48Hours
 *              
 *              3)getValue(bool printing,char gasName, double *output) ------> get values of sensor and measure gas in ppm this function have 3 input 1)bool printing: for printing values
 *                                                                                                                                                    2)char gasname : the gas name value which want get values 
 *                                                                                                                                                    it includes: 
 *                                                                                                                                                    'L' ---> LPG
 *                                                                                                                                                    'P' ---> Propane
 *                                                                                                                                                    'H' ---> H2
 *                                                                                                                                                    'M' ---> Methane
 *                                                                                                                                                    'A' ---> Alchol
 *                                                                                                                                                    3)double output : values return inside it should be in size of 5
 *              4)thrValue(char gasName,int threshold)  ------> this function is defined for thresholding define threshold for specific gas name and it returns true or false ****gas name are same with number 2-2
 *              
 *              5)printData(float data)  ------> this function is used for printing data it $%$ if your compiler is not arduino change as u want
 *              
 *              6)readADC(int pin)   ------> this function is used for reading adc values $%$ if your compiler is not arduino change as u want
 *              
 *              7)thrNvalue(int threshold)------> just use adc value as threshold(fast mode)
 * 
 * 
 * the chart inside MQ-x sensors are used 
 * this chart is log chart and values are captured from them 
 * 
 * *****************Please visit below website for more information**************************
 * 
 *              https://thestempedia.com/tutorials/interfacing-mq-2-gas-sensor-with-evive/
 * ******************************************************************************************
 */

void * mq2::inits() {

Serial.begin(9600);
adcBits = 1024;//use for defining number of bits to calculate values of MQ sensor
adcVoltage =5.0;//adc refrence voltage
    


}
void * mq2::calibrate() {
  float sensor_volt;  
  float RS_air; //  Rs in clean air 
  
  float sensorValue;
  for(int x = 0 ; x < 1000 ; x++) 
  { 
    sensorValue = sensorValue + readADC(A0); 
  }   
  sensorValue = sensorValue/1000.0;
  Serial.println(sensorValue);
  sensor_volt = (sensorValue/adcBits)*adcVoltage; 
  RS_air = (adcVoltage-sensor_volt)/sensor_volt; // Depend on RL on yor module 
  _R0 = RS_air/9.9; // According to MQ9 datasheet table 
   
 
  printData(_R0); 
  delay(1000);   
}

double * mq2::getValue(bool printing,char gasName, double *output) {

  float sensor_volt; 
  float RS_gas; 
  float ratio; 
  float ppm;
 
  int sensorValue = readADC(A0); 
  sensor_volt = ((float)sensorValue / adcBits) * adcVoltage; 
  RS_gas = (adcVoltage - sensor_volt) / sensor_volt; // Depend on RL on yor module 
 
 
  ratio = RS_gas / _R0; // ratio = RS/R0 
  switch (gasName){
  case 'L'://lpg gas
  _M0 = -0.454838059;
  _B0 = 1.25063406;
  ppm = pow(10,(log(ratio)- _B0)/_M0);
  if(printing){printData(ppm);}
  output[1]=ppm;
  break;
  case 'P'://propane gas
  _M0 = -0.461038681;
  _B0 = 1.290828982;
  ppm = pow(10,(log(ratio)- _B0)/_M0);
  if(printing){printData(ppm);}
  output[1]=ppm;
  break;
  case 'H'://h2 gas
  _M0 = -0.47305447;
  _B0 = 1.412572126;
  ppm = pow(10,(log(ratio)- _B0)/_M0);
  //Serial.println(ppm);
  if(printing){printData(ppm);}
  output[1]=ppm;
  break;
  case 'M'://methane
  _M0 = -0.372003751;
  _B0 = 1.349158571;
  ppm = pow(10,(log(ratio)- _B0)/_M0);
  //Serial.println(ppm);
  if(printing){printData(ppm);}
  output[1]=ppm;
  break;
  case 'A'://Alchol
  _M0 = -0.373311285;
  _B0 = 1.310286169;
  ppm = pow(10,(log(ratio)- _B0)/_M0);
  //Serial.println(ppm);
  if(printing){printData(ppm);}
  output[1]=ppm;
  break;
  default://all values
  ppm = pow(10,(log(ratio)- 1.25063406)/-0.454838059);//LPG
  output[1]=ppm;
  ppm = pow(10,(log(ratio)- (-0.461038681))/1.290828982);//propane
  output[2]=ppm;
  ppm = pow(10,(log(ratio)- (-0.47305447))/-0.454838059);//H2 gas
  output[3]=ppm;
  ppm = pow(10,(log(ratio)- (-0.372003751))/1.349158571);//Methane gas
  output[4]=ppm;
  ppm = pow(10,(log(ratio)- (-0.373311285))/1.310286169);//Alchol gas
  output[5]=ppm;
  break;
}

 
  
 
 
  delay(1000);


    
}
bool  mq2::thrValue(char gasName,int threshold){

float sensor_volt; 
  float RS_gas; 
  float ratio; 
  float ppm;
 
  int sensorValue = readADC(A0); 
  sensor_volt = ((float)sensorValue / 1024) * 3.3; 
  RS_gas = (3.3 - sensor_volt) / sensor_volt; // Depend on RL on yor module 
 
 
  ratio = RS_gas / _R0; // ratio = RS/R0 
  switch (gasName){
  case 'L'://lpg gas
  _M0 = -0.454838059;
  _B0 = 1.25063406;
  ppm = pow(10,(log(ratio)- _B0)/_M0);
  
  break;
  case 'P'://propane gas
  _M0 = -0.461038681;
  _B0 = 1.290828982;
  ppm = pow(10,(log(ratio)- _B0)/_M0);
  
  
  break;
  case 'H'://h2 gas
  _M0 = -0.47305447;
  _B0 = 1.412572126;
  ppm = pow(10,(log(ratio)- _B0)/_M0);
  

  break;
  case 'M'://methane
  _M0 = -0.372003751;
  _B0 = 1.349158571;
  ppm = pow(10,(log(ratio)- _B0)/_M0);
  
  
  break;
  case 'A'://Alchol
  _M0 = -0.373311285;
  _B0 = 1.310286169;
  ppm = pow(10,(log(ratio)- _B0)/_M0);
  
  break;
 
}


if(ppm>=threshold){
    return(true);}
  else{return(false);}


  
}
  
bool  mq2::thrNvalue(int threshold){



if(readADC(0)>=threshold){
    return(true);}
  else{return(false);}


  
}
int mq2::readADC(int pin){

return(analogRead(pin));


  
}
void mq2::printData(float data){


 Serial.print("data is:");
 Serial.println(data,2);
}
