int SliderPINS[] = {21,20,19,18,10,9,8};

int SliderValue[7];

int TotalSliders = 7;

void setup()
{
  //StartSerialPort
  Serial.begin(9600); //
  Serial1.begin(9600);
  delay(3000);
  for (int thisPin = 0; thisPin < TotalSliders; thisPin++) 
  {
      pinMode(SliderPINS[thisPin], INPUT);
      //Serial.print(SliderPINS[thisPin]);
  }
}

void loop() {

  for (int thisPin = 0; thisPin < TotalSliders; thisPin++) 
    {
      int temp = map(analogRead(SliderPINS[thisPin]), 0, 1023, 0, 100);
      if (temp!=SliderValue[thisPin])
        {
          Serial.println("Volume" + String(thisPin) + String(": ") + String(temp));// Serial.println(temp);
          SliderValue[thisPin]=temp;
        }
    }
 
//send data to other arduino
if (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('\n');
    Serial1.println(receivedData);
  }

//check for the arduino recived data for debuging purposes
  if (Serial1.available() > 0) {
    String slaveData = Serial1.readStringUntil('\n');
    Serial.print("Slave:"); Serial.println(slaveData);
  }
  delay(500);
}