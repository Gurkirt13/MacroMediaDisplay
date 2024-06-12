#include <GxEPD2_3C.h>
#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
#include <Fonts/FreeMonoBold9pt7b.h>

#define GxEPD2_DRIVER_CLASS GxEPD2_290_Z13c // GDEH029Z13  128x296, UC8151D, (HINK-E029A10-A3 20160809)

#define MAX_DISPLAY_BUFFER_SIZE 800 // 
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=*/ 10, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7));

int16_t tbx, tby; uint16_t tbw, tbh;
String receivedData;
int temp1;
void setup()
{
  //display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
  display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  
  pinMode(4, OUTPUT); //Test LED
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b); 
  //StartSerialPort
  Serial.begin(9600); //

  pinMode(18, INPUT);

}

void loop() {

  //VolumeControl
  int slider1Value = analogRead(18); // Read value from slider 1
  int slider2Value = analogRead(3); // Read value from slider 2

  // Map the slider values to a range from 0 to 100
  int volume1 = map(slider1Value, 0, 1023, 0, 100);
 
  int volume2 = map(slider2Value, 0, 1023, 0, 100);
  if (volume1!=temp1)
  {
    Serial.print("Volume1:"); Serial.println(volume1);
    temp1=volume1;
  }

 

 
  //UpdateSongs
  if (Serial.available() > 0) 
  { // Check if data is available to read
    // Clear the serial buffer to ensure we process the latest data
    
    while (Serial.available() > 0) {
      receivedData = Serial.readStringUntil('\n'); // Read the incoming string
    }

    // Only update the display if the song title has changed
    static String previousSong = "";
    if (receivedData != previousSong) {
      digitalWrite(4, HIGH);
      updateDisplay(receivedData);
      previousSong = receivedData;
    }
  }
}

void updateDisplay(String songTitle) 
{
        display.getTextBounds(songTitle, 0, 0, &tbx, &tby, &tbw, &tbh);
        uint16_t x = ((display.width() - tbw) / 2) - tbx;
        uint16_t y = ((display.height() - tbh) / 2) - tby;
        display.setFullWindow();
        //display.setPartialWindow( 48, 92, 48, 92);
        //display.fillScreen(GxEPD_WHITE);
        display.setTextColor(GxEPD_BLACK);
        display.firstPage();
        do
          {
            display.setCursor(x, y);
            display.print(songTitle);
            display.display(true);
          }
        while (display.nextPage());
        //display.hibernate(); 
        receivedData=songTitle;
}