/**************************************************************************************************************
*
*
*
*
*
*
*
**************************************************************************************************************/
#ifndef _HAL_LED_H_
#define _HAL_LED_H_

#include "74hc595.h"

#define LED_0   0
#define LED_1   1
#define LED_2   2
#define LED_3   3
#define LED_4   4
#define LED_5   5
#define LED_6   6
#define LED_7   7
#define LED_8   8
#define LED_9   9
#define LED_a   0x0a
#define LED_b   0x0b
#define LED_c   0x0c
#define LED_d   0x0d
#define LED_E   0x0e
#define LED_F   0x0f
#define LED_H   0x10
#define LED_h   0x11
#define LED_L   0x12
#define LED_n   0x13
#define LED_N   0x14
#define LED_o   0x15
#define LED_p   0x16
#define LED_q   0x17
#define LED_r   0x18
#define LED_t   0x19
#define LED_U   0x1A
#define LED_y   0x1B
#define LED__   0x1C
#define LED_SPACE    0x1D

#define SEG_H    ~0x80
#define B_dot    0x80
#define MAX_LEDCH 0X1D

void led_out(uint8_t dat, uint8_t loca);
void set_led_dispbuf(uint8_t *buf, uint8_t pos,uint8_t len);
void clr_led_dispbuf(void);
void Bcd2disp(uint8_t *bcd, uint8_t *buf,uint8_t len);

extern void led_scan(void);

#endif
