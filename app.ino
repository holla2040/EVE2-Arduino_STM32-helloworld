#include <SPI.h>
#include "app.h"

uint32_t timeoutBlink;
uint32_t timeoutDim;
int DotSize = 20;
char i = '0';

#define USE_RAMDL

void setup() {
  console.begin(115200);
  console.println("\neve2 helloworld setup");
#ifdef USE_RAMDL
  console.println("RAMDL");
#else
  console.println("FIFO");
#endif

  pinMode(LED_BUILTIN,OUTPUT);
  spiSetup();
  eveSetup();

  console.println("setup done");
  timeoutBlink = 0;
  timeoutDim = millis() + 15000;
}


void loop() {
  uint8_t tag = 0;
  if (millis() > timeoutBlink) {
    digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN));
    timeoutBlink = millis() + 1000;

#ifdef USE_RAMDL
    wr32(RAM_DL + 0, CLEAR(1, 1, 1));                // clear screen (but not calibration)
    wr32(RAM_DL + 4, BEGIN(BITMAPS));                // start drawing bitmaps
    wr32(RAM_DL + 8, VERTEX2II(120, 110, 31, 'F'));  // ascii F in font 31
    wr32(RAM_DL + 12, VERTEX2II(144, 110, 31, 'T')); // ascii T
    wr32(RAM_DL + 16, VERTEX2II(172, 110, 31, 'D')); // ascii D
    wr32(RAM_DL + 20, VERTEX2II(199, 110, 31, 'I')); // ascii I
    wr32(RAM_DL + 24, VERTEX2II(237, 180, 31, i++)); 
    wr32(RAM_DL + 28, END());                        // end placing bitmaps
    wr32(RAM_DL + 32, COLOR_RGB(192, 26, 26));       // change colour to reddish
    wr32(RAM_DL + 36, POINT_SIZE(DotSize * 16));     // set point size to DotSize pixels. Points = (pixels x 16)
    wr32(RAM_DL + 40, BEGIN(POINTS));                // start drawing points
    wr32(RAM_DL + 44, TAG(1));                       // Tag the red dot with a touch ID
    wr32(RAM_DL + 48, VERTEX2II(92, 133, 0, 0));    // place red point
    wr32(RAM_DL + 52, COLOR_RGB(26, 26, 192));       // change colour to blue
    wr32(RAM_DL + 56, TAG(2));                       // Tag the red dot with a touch ID
    wr32(RAM_DL + 60, VERTEX2II(250, 133, 0, 0));    // place blue point
    wr32(RAM_DL + 64, END());                        // end placing points
    wr32(RAM_DL + 68 , DISPLAY());                   // display the image
    wr8(REG_DLSWAP + RAM_REG, DLSWAP_FRAME);         // swap display lists
#else
    Send_CMD(CMD_DLSTART);
    Send_CMD(CLEAR(1, 1, 1));                // clear screen (but not calibration)
    Send_CMD(BEGIN(BITMAPS));                // start drawing bitmaps
    Send_CMD(VERTEX2II(120, 110, 31, 'F'));  // ascii F in font 31
    Send_CMD(VERTEX2II(144, 110, 31, 'T'));  // ascii T
    Send_CMD(VERTEX2II(172, 110, 31, 'D'));  // ascii D
    Send_CMD(VERTEX2II(199, 110, 31, 'I'));  // ascii I
    Send_CMD(VERTEX2II(237, 180, 31, i++));   
    Send_CMD(END());                         // end placing bitmaps
    Send_CMD(COLOR_RGB(192, 26, 26));        // change colour to reddish
    Send_CMD(POINT_SIZE(DotSize * 16));      // set point size to DotSize pixels. Points = (pixels x 16)
    Send_CMD(BEGIN(POINTS));                 // start drawing points
    Send_CMD(TAG(2));                        // Tag the red dot with a touch ID
    Send_CMD(VERTEX2II(92, 133, 0, 0));     // place red point
    Send_CMD(COLOR_RGB(26, 26, 192));       // change colour to blue
    Send_CMD(TAG(2));                       // Tag the red dot with a touch ID
    Send_CMD(VERTEX2II(250, 133, 0, 0));    // place blue point
    Send_CMD(END());                         // end placing points
    Send_CMD(COLOR_RGB(255, 255, 255));
    Cmd_Text(10,10,31,0,"helloworld");
    Send_CMD(DISPLAY());                     // display the image
    Send_CMD(CMD_SWAP);                      // Make this screen active
    UpdateFIFO();                            // Trigger the CoProcessor to start processing the FIFO
#endif

    if (i > '9') i = '0';

  }
  tag = rd8(REG_TOUCH_TAG + RAM_REG);
  if (tag) {
    wr8(RAM_REG + REG_PWM_DUTY , 128);
    console.print(millis());
    console.print(" ");
    switch (tag) {
      case 1:
        console.println("dot red");
        break;
      case 2:
        console.println("dot blue");
        break;
    }
    delay(20);
    timeoutDim = millis() + 15000;
  }
  if (millis() > timeoutDim) {
    wr8(RAM_REG + REG_PWM_DUTY , 2);
    timeoutDim = millis() + 15000;
  }
}

