#ifndef _APP_H
#define _APP_H

#include <SPI.h>

// #define DEBUG


// change these to make your board and display
#define EVE2_43
const uint8_t   PIN_CS                     = PA4;
const uint8_t   PIN_PDN                    = PA3;
const uint8_t   PIN_AUDIO                  = PA2;
#define console Serial1

#include "eve2.h" // has all the addresses defined

void spiSetup();
void hostCommand(uint8_t command);
uint8_t rd8(uint32_t address);
uint16_t rd16(uint32_t address);
uint32_t rd32(uint32_t address);

void wr8(uint32_t address, uint8_t parameter);
void wr16(uint32_t address, uint16_t parameter);
void wr32(uint32_t address, uint32_t parameter);
void spiEnable();
void spiDisable();

void eveSetup();

void cmd_text(int16_t x,int16_t y,int16_t font,uint16_t options,char* s);

void Send_CMD(uint32_t data);
void UpdateFIFO(void); 
void Cmd_Text(uint16_t x, uint16_t y, uint16_t font, uint16_t options, const char* str);

#endif
