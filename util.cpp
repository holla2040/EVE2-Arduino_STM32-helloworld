/* pulled from matrix orbital's EVE-School repo https://github.com/MatrixOrbital/EVE-School */


#include "app.h"

SPISettings spiSettings(20000000, MSBFIRST, SPI_MODE0);  

char line[50];
uint16_t FifoWriteLocation;                                                                                                                                                               
void spiSetup() {
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_CS, HIGH);
  pinMode(MOSI, OUTPUT);
  pinMode(MISO, INPUT_PULLUP);
  SPI.begin();
  FifoWriteLocation = 0;
}

void hostCommand(uint8_t command) {
#ifdef DEBUG
  sprintf(line,"%-6d %-8s 0x%02X",millis(),"hostComm",command);
  console.println(line);
#endif
  spiEnable();
  SPI.transfer(command);
  SPI.transfer(0x00);
  SPI.transfer(0x00);
  spiDisable();
}

uint8_t rd8(uint32_t address) {
  uint8_t buf[1];
  spiEnable();
  uint8_t *a = (uint8_t*)&address;
  SPI.write((uint8_t)(a[2]&0x3F));  
  SPI.write((uint8_t)a[1]);      
  SPI.write((uint8_t)a[0]);              
  SPI.write(0x00); // dummy
  SPI.read(buf, 1);
  spiDisable();
  return buf[0];
}

uint16_t rd16(uint32_t address) {
  uint8_t buf[2];
  spiEnable();
  uint8_t *a = (uint8_t*)&address;
  SPI.write((uint8_t)(a[2]&0x3F));  
  SPI.write((uint8_t)a[1]);      
  SPI.write((uint8_t)a[0]);              
  SPI.write(0x00); // dummy
  SPI.read(buf, 2);
  spiDisable();

  uint16_t rv = buf[0] + ((uint16_t)buf[1] << 8);
  return rv;
}

uint32_t rd32(uint32_t address) {
  uint8_t buf[4];
  uint32_t rv;

  spiEnable();
  uint8_t *a = (uint8_t*)&address;
  SPI.write((uint8_t)(a[2]&0x3F));  
  SPI.write((uint8_t)a[1]);      
  SPI.write((uint8_t)a[0]);              
  SPI.write(0x00); // dummy
  SPI.read(buf, 4);
  spiDisable();

  rv = buf[0] + ((uint32_t)buf[1] << 8) + ((uint32_t)buf[2] << 16) + ((uint32_t)buf[3] << 24);
  return rv;
}

void wr8(uint32_t address, uint8_t parameter) { 
#ifdef DEBUG
  sprintf(line,"%-6d %-8s 0x%08X 0x%02X",millis(),"wr8",address,parameter);
  console.println(line);
#endif
  uint8_t *a = (uint8_t*)&address;

  spiEnable();
  SPI.write((uint8_t)(a[2]&0x3F | 0x80));  
  SPI.write((uint8_t)a[1]);      
  SPI.write((uint8_t)a[0]);              
  SPI.write(parameter);
  spiDisable();
}

void wr16(uint32_t address, uint16_t parameter) {
#ifdef DEBUG
  sprintf(line,"%-6d %-8s 0x%08X 0x%04X",millis(),"wr16",address,parameter);
  console.println(line);
#endif
  uint8_t *a = (uint8_t*)&address;
  spiEnable();
  SPI.write((uint8_t)a[2]&0x3F | 0x80); 
  SPI.write((uint8_t)a[1]); 
  SPI.write((uint8_t)a[0]);
  SPI.write(&parameter,2);
  spiDisable();
}

void wr32(uint32_t address, uint32_t parameter) {
#ifdef DEBUG
  sprintf(line,"%-6d %-8s 0x%08X 0x%08X",millis(),"wr32",address,parameter);
  console.println(line);
#endif
  uint8_t *a = (uint8_t*)&address;

  spiEnable();
  SPI.write((uint8_t)(a[2]&0x3F | 0x80));  
  SPI.write((uint8_t)a[1]);     
  SPI.write((uint8_t)a[0]);              
  SPI.write(&parameter,4);
  spiDisable();
}

void spiEnable() {
  SPI.beginTransaction(spiSettings);
  digitalWrite(PIN_CS, LOW);
}

void spiDisable() {
  digitalWrite(PIN_CS, HIGH);
  SPI.endTransaction();
}

uint32_t dlAddress = RAM_DL;
uint16_t *dlp = (uint16_t*)&dlAddress;

void dl(uint32_t v) {
#ifdef DEBUG
  sprintf(line,"%-6d %-8s 0x%08X 0x%08X",millis(),"dl",dlAddress,v);
  console.println(line);
#endif
  SPI.write(v,4);
  dlAddress += FT_CMD_SIZE;
  if (dlAddress > (RAM_DL + FT_DL_SIZE)) dlAddress = RAM_DL;
}

void eveSetup() {
  pinMode(PIN_PDN, OUTPUT);            
  pinMode(PIN_AUDIO, OUTPUT);         

  digitalWrite(PIN_AUDIO, LOW);  

  digitalWrite(PIN_PDN, HIGH);   
  digitalWrite(PIN_PDN, LOW);   
  digitalWrite(PIN_PDN, HIGH);   

  hostCommand(HCMD_CLKEXT);
  hostCommand(HCMD_ACTIVE);

  while (rd8(RAM_REG + REG_ID) != 0x7C) {};

  // turn off screen output during startup
   wr8(RAM_REG + REG_PCLK, 0);              // Pixel Clock Output disable

  // screen config
  wr16(RAM_REG + REG_HCYCLE  , HCYCLE);
  wr16(RAM_REG + REG_HOFFSET , HOFFSET);
  wr16(RAM_REG + REG_HSYNC0  , HSYNC0);
  wr16(RAM_REG + REG_HSYNC1  , HSYNC1);
  wr16(RAM_REG + REG_VCYCLE  , VCYCLE);
  wr16(RAM_REG + REG_VOFFSET , VOFFSET);
  wr16(RAM_REG + REG_VSYNC0  , VSYNC0);
  wr16(RAM_REG + REG_VSYNC1  , VSYNC1);
   wr8(RAM_REG + REG_SWIZZLE , SWIZZLE);
   wr8(RAM_REG + REG_PCLK_POL, PCLK_POL);
  wr16(RAM_REG + REG_HSIZE   , HSIZE);
  wr16(RAM_REG + REG_VSIZE   , VSIZE);
   wr8(RAM_REG + REG_CSPREAD , CSPREAD);
   wr8(RAM_REG + REG_DITHER  , DITHER);

  // touch & audio config
  wr16(RAM_REG + REG_TOUCH_RZTHRESH,  1200);    // set touch resistance threshold
   wr8(RAM_REG + REG_TOUCH_MODE,      0x02);    // set touch on: continous - this is default
   wr8(RAM_REG + REG_TOUCH_ADC_MODE,  0x01);    // set ADC mode: differential - this is default
   wr8(RAM_REG + REG_TOUCH_OVERSAMPLE,15);      // set touch oversampling to max

  wr16(RAM_REG + REG_PWM_HZ  ,  0x00FA);        // Backlight PWM frequency
   wr8(RAM_REG + REG_PWM_DUTY , 0x7F);          // Backlight PWM duty (off)   

   wr8(RAM_REG + REG_GPIO_DIR,0x80 | rd8(RAM_REG + REG_GPIO_DIR));
   wr8(RAM_REG + REG_GPIO,0x080 | rd8(RAM_REG + REG_GPIO));//enable display bit

  // make display list with bluescreen

   wr32(RAM_DL+0, CLEAR_COLOR_RGB(0,100,0));
   wr32(RAM_DL+4, CLEAR(1,1,1));
   wr32(RAM_DL+8, DISPLAY());
   wr8(RAM_REG + REG_DLSWAP, DLSWAP_FRAME);          // swap display lists
   wr8(RAM_REG + REG_PCLK, 5);                       // after this display is visible on the LCD
}

void Send_CMD(uint32_t data) {
  wr32(RAM_CMD + FifoWriteLocation, data); 

  FifoWriteLocation += FT_CMD_SIZE;
  FifoWriteLocation %= FT_CMD_FIFO_SIZE;
}

void UpdateFIFO(void) {
  wr16(RAM_REG + REG_CMD_WRITE, FifoWriteLocation);
}


// *** Draw Text - FT81x Series Programmers Guide Section 5.41 ***************************************************
void Cmd_Text(uint16_t x, uint16_t y, uint16_t font, uint16_t options, const char* str){ 
  uint16_t DataPtr, LoopCount, StrPtr;

  uint16_t length = strlen(str);
  if(!length)
    return;

  uint32_t* data = (uint32_t*) calloc((length / 4) + 1, sizeof(uint32_t)); // Allocate memory for the string expansion

  StrPtr = 0;
  for(DataPtr=0; DataPtr<(length/4); ++DataPtr, StrPtr=StrPtr+4)
    data[DataPtr] = (uint32_t)str[StrPtr+3]<<24 | (uint32_t)str[StrPtr+2]<<16 | (uint32_t)str[StrPtr+1]<<8 | (uint32_t)str[StrPtr];

  for(LoopCount=0; LoopCount<(length%4); ++LoopCount, ++StrPtr)
    data[DataPtr] |= (uint32_t)str[StrPtr] << (LoopCount*8);

  // Set up the command
  Send_CMD(CMD_TEXT);
  Send_CMD( ((uint32_t)y << 16) | x );
  Send_CMD( ((uint32_t)options << 16) | font );

  // Send out the text
  for(LoopCount = 0; LoopCount <= length/4; LoopCount++)
    Send_CMD(data[LoopCount]);  // These text bytes get sucked up 4 at a time and fired at the FIFO

  free(data);
}

void printRAM_DL() {
  char temp[100];
  console.println("\nRAM_DL");
  console.println("-----------------------");
  for (uint32_t a = RAM_DL;a < (RAM_DL + 100);a += 4) {
    sprintf(temp,"0x%08X 0x%08X",a,rd32(a));
    console.println(temp);
  }
  console.println();
}
