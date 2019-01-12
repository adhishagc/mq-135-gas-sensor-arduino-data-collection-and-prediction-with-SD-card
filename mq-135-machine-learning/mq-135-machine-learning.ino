//Importing libraries
#include <SD.h> //SD Card Header
#include <SPI.h> //SPI Protocol for communication
#include <MQ135.h> //Gas Sensor Header


int LED_sensor_indicator = A1; //Sensor indicator LED
int LED_SD_indicator = 48; //SD Card indicator LED
int sensor_activate_button = 44; //Push button to activate the Sensor
int PowerPin = 13; //5V Power Pin to the Sensor BreadBoard
int ModeInput = 7; //Initialize Pin 7 to get the Input from the Switch to decide the Mode.
int CS_PIN = 53; // SPI Pin on the board
int previous_push_input = 0; //Used to get only reading from the Sensor
File file; //For file reading purposes

//Creating an instance of the Gas Sensor
MQ135 gasSensor = MQ135(A0);




void setup()
{

  //Initializing Serial Communication
  Serial.begin(9600);
  //Initializing Ports
  pinMode(LED_sensor_indicator,OUTPUT);
  pinMode(LED_SD_indicator,OUTPUT);
  pinMode(PowerPin,OUTPUT);
  pinMode(ModeInput,INPUT);
  pinMode(sensor_activate_button,INPUT);
  analogWrite(PowerPin,255);
  digitalWrite(LED_SD_indicator,HIGH);
  
  //Initialize SD Card
  initializeSD();
  
  //createFile("dataset.txt");
  //writeToFile("10.0,76.63,116.6020682,2.769034857,0.00035,0.02718,1.39538,0.0018,397.13");
  //closeFile();
  
  /*openFile("dataset.txt");
  readOpenFile();
  closeFile();*/

  //Update Sensor Parameters
  loadParameters();
  

  //Calculations take place
  float rzero = gasSensor.getRZero();
 
}

void loadParameters(){
  //Indicate Busy
  SD_indicator_busy(true);
  //open data file from SD card
  openFile("data.txt");
  //Read and update the data
  readLine();
  //close opened file
  closeFile();
  SD_indicator_busy(false);
}

void SD_indicator_busy(boolean state){
  if(state == true){
    //On
    digitalWrite(LED_SD_indicator,HIGH);
  }
  else{
    //Off
    digitalWrite(LED_SD_indicator,LOW);
  }
}

/*void readOpenFile(){
  String received = "";
  char ch;
  
  while (file.available())
  {
    ch = file.read();
    
    Serial.println(ch);
  }
}*/

void readLine(){
  String received = "";
  char ch;
  float paramList[9];
  int count = 0;
  
  while (file.available())
  {
    ch = file.read();
    
    if (ch == ',')
    {
      paramList[count] = received.toFloat();
      
      
      received = "";
      //return String(received);
      count++;
    }
    else if (ch =='\n'){
      
    }
    else
    {
      received += ch;
    }
  }

  Serial.println("Parameters are currently being loaded from file");
  
  //Code to access the SD Card and read the parameter push_inputues

  float RLOAD = paramList[0];
  /// Calibration resistance at atmospheric CO2 level
  float RZERO = paramList[1];
  /// Parameters for calculating ppm of CO2 from sensor resistance
  float PARA =  paramList[2];
  float PARB = paramList[3];
  
  /// Parameters to model temperature and humidity dependence
  float CORA = paramList[4];
  float CORB = paramList[5];
  float CORC = paramList[6];
  float CORD = paramList[7];
  
  /// Atmospheric CO2 level for calibration purposes
  float ATMOCO2 = paramList[8];

  //updating the parameters
  gasSensor.updateParameters(RLOAD,RZERO,PARA,PARB,CORA,CORB,CORC,CORD,ATMOCO2); 
}

void loop() {
  // put your main code here, to run repeatedly:
  //int push_input = digitalRead(ModeInput);
  //Serial.println(push_input);
  //float ppm = gasSensor.getPPM();
  //sensorReadBlink();
  //Serial.println(ppm*100);

  // Check state
  int mode = digitalRead(ModeInput);
  int push_input = digitalRead(sensor_activate_button);
  
  //If mode == 0, it switches to prediction mode, otherwise training mode.
   if(mode ==0){
    //It is in Prediction mode
    /*
      The following series of nested if is to make sure that only once the
      make prediction is run. Normally when a human presses push button the 
      loop cycle rate makes the makePrediction function run multiple times.
      Therefore the following if statements are used to avoid it.

      Using the previous push input checks whether the digital input is 1 twice.
      If so prediction is not made.
    */
    if(push_input == 1 && previous_push_input == 0){
      //Prediction takes place
      makePrediction();
      //Set the previous Push input to 1
      previous_push_input = 1;
    }
    else if(push_input ==1 && previous_push_input==1){
      //Do nothing
    }
    else if(push_input == 0 && previous_push_input == 1){
      //Reset the push input
      previous_push_input = 0;
    }
    else if(push_input == 0 && previous_push_input == 0){
      //Keep Blinking the lights to indicate the user to Press the Push
      pushButtonWaiting();
    }

    
  }
  else{
    //It is in Training mode
    //Serial.println("Training mode");
    //previous_push_input = 0;
    //push_input = 0;

    if(push_input == 1 && previous_push_input == 0){
      //Prediction takes place
      trainSample();
      //Set the previous Push input to 1
      previous_push_input = 1;
    }
    else if(push_input ==1 && previous_push_input==1){
      //Do nothing
    }
    else if(push_input == 0 && previous_push_input == 1){
      //Reset the push input
      previous_push_input = 0;
    }
    else if(push_input == 0 && previous_push_input == 0){
      //Keep Blinking the lights to indicate the user to Press the Push
      pushButtonWaiting();
    }
  }

  //Serial.println(mode); 
}

void pushButtonWaiting(){
  //Initialize pin
  int led_pin_push = 45;
  pinMode(led_pin_push,OUTPUT);
  digitalWrite(led_pin_push,HIGH);
  delay(500);
  digitalWrite(led_pin_push,LOW);
  delay(500);
}

void makePrediction(){

  //Get PPM push_inputue
  float ppm = gasSensor.getPPM();
  //Send to Machine Learning Model
  String output = sendToMLModel(ppm);
  //Blink Lights on the sensor
  sensorBusyIndicator();
  //Printing the output
  Serial.println("Predicted result " + output);
  
}

void sensorBusyIndicator(){
  //This blinks the Sensor leds faster
  for(int times=0;times<2;times++){
    digitalWrite(LED_sensor_indicator,HIGH);
    delay(90);
    digitalWrite(LED_sensor_indicator,LOW);
    delay(90);  
  }
  
}

void trainSample(){
  //Get PPM push_inputue
  float ppm = gasSensor.getPPM();
  //Send to Machine Learning Model
  String output = sendToMLTrainingModel(ppm);
  //Lights Blink when the sensor is on
  sensorBusyIndicator();
  //Printing the output
  Serial.println("Status " + output);
}

String sendToMLTrainingModel(float ppm){
  //Function to send the collected data to the Machine learning model
  openFile("dataset.txt");
  String ppm_string = String(ppm);
  writeToFile(ppm_string);
  closeFile();
  //Code
  Serial.println(ppm);
  String status = "Training Completed";
  return status;
}

String sendToMLModel(float ppm){
  //Function to send the collected data to the Machine learning model
  //Code
  Serial.println(ppm);
  String prediction = "Positive";
  return prediction;
}

void sensorReadBlink(){
  digitalWrite(LED_sensor_indicator,HIGH);
  delay(90);
  digitalWrite(LED_sensor_indicator,LOW);
  delay(90);
  }

void initializeSD(){
  digitalWrite(LED_SD_indicator,HIGH);
  Serial.println("Initializing SD card...");
  delay(1000);
  pinMode(CS_PIN, OUTPUT);

  if (SD.begin(CS_PIN))
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
  digitalWrite(LED_SD_indicator,LOW);
}

int createFile(char filename[]){
  file = SD.open(filename, FILE_WRITE);

  if (file)
  {
    Serial.println("File created successfully.");
    return 1;
  } else
  {
    Serial.println("Error while creating file.");
    return 0;
  }
}

int writeToFile(String text){
  if (file)
  {
    file.println(text);
    Serial.println("Writing to file: ");
    Serial.println(text);
    return 1;
  } else
  {
    Serial.println("Couldn't write to file");
    return 0;
  }
}

void closeFile(){
  if (file)
  {
    file.close();
    Serial.println("File closed");
  }
}

int openFile(char filename[]){
  file = SD.open(filename);
  if (file)
  {
    Serial.println("File opened with success!");
    return 1;
  } else
  {
    Serial.println("Error opening file...");
    return 0;
  }
}


