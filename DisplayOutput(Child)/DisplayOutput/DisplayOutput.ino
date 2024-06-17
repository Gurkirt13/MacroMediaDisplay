#include <GxEPD2_3C.h>
#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
#include <Fonts/FreeMonoBold9pt7b.h>

#define GxEPD2_DRIVER_CLASS GxEPD2_290_Z13c // GDEH029Z13  128x296, UC8151D, (HINK-E029A10-A3 20160809)

#define MAX_DISPLAY_BUFFER_SIZE 800 // 
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(/*CS=*/ 10, /*DC=*/ 8, /*RST=*/ 9, /*BUSY=*/ 7));

int16_t tbx, tby; uint16_t tbw, tbh;
String receivedData;
String previousSong;
int temp1;
void setup()
{
  //display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
  display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  display.refresh();
  display.setRotation(1);
  display.setFont(&FreeMonoBold9pt7b); 
  //StartSerialPort
  Serial1.begin(9600); //
}
void loop()
{
  if (Serial1.available() > 0) { // Check if data is available from the master Arduino
    receivedData = Serial1.readStringUntil('\n'); // Read the incoming string

    // Only update the display if the song title has changed
    static String previousSong = "";
    if (receivedData != previousSong) {
      digitalWrite(4, HIGH);
      updateDisplay(receivedData);
      previousSong = receivedData;
      Serial1.println(receivedData);
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
        display.setTextColor(GxEPD_WHITE);
        display.firstPage();
        do
          {
            display.setCursor(x, y);
            display.print(songTitle);
            display.display(false);
          }
        while (display.nextPage());
        //display.hibernate(); 
        receivedData=songTitle;
}