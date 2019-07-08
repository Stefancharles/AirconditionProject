#ifndef _pcf8563_h_
#define _pcf8563_h_

#include "stdint.h"

uint8_t PCF8563_Init(void);
void PCF8563_GetTime(uint8_t *ptr);
void PCF8563_WirteTime(uint8_t *ptr);
uint8_t Convert_BcdToHex(uint8_t *bcd, uint8_t *hex);
uint8_t Time_CheckValidity(uint8_t *rtc);

#endif /* _pcf8563_h_ */
