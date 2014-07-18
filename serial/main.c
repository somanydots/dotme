/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <string.h>

#include "uart.h"
#include "font.h"

#define CLOCKPULSE 1000  // u s
#define CLOCKPIN  (1<<PB4)  // portD
#define DATAPIN (1<<PB2)  // portD
#define RESETPIN (1<<PB3)  // portD

#define BLANKING 250 // ms

#define DELAY 3000

static void initDOTS(void);
static void clock(void);
static void writeCol(uint8_t col);
static void writeChar(uint8_t ch);
static void writeArray(char* to_write);

char to_write[12];

static void initLED(void) {
    DDRB|=(1<<PB5);
}

int main(void)
{
    initLED();
    initDOTS();
    uart_init(UART_BAUD_SELECT(38400, F_CPU));
    sei();
    uart_flush();
    _delay_ms(100);


    uint8_t i=0;

    char c;
    memset(to_write, 0x00, sizeof(to_write));

    PORTD &=~RESETPIN;
    _delay_ms(1);
    PORTD |= RESETPIN;

    for (uint8_t i=0; i<200; i++) {
      writeCol(0xFF);
    }

    for (;;) {
      while(uart_available() == 0) {
        PORTB|=(1<<PB5);
        _delay_ms(100);
        PORTB&=~(1<<PB5);
        _delay_ms(100);
      }
      c = uart_getc();
      char c_lo = c;
      uart_putc(c);
      if (c_lo >= 0x20) {
        to_write[i++] = c_lo;
      }
      if ((c_lo < 0x20) || (i >= 12)) {
        writeArray(to_write);
        i=0;
      }
    }
    return 0;   /* never reached */
}

static void writeArray(char* to_write) {
  uint8_t i = 0;
  writeCol(0xFF);    writeCol(0xFF);
  while (i<12 && to_write[i] >= 0x20) {
    writeChar(to_write[i]);
    i++;
  }
  for(i*=6; i<98; i++){
    writeCol(0xFF);
  }
  PORTD &=~RESETPIN;
  _delay_ms(1);
  PORTD |= RESETPIN;
  memset(to_write, 0x00, 12);
}

static void writeChar(uint8_t ch) {
    uint8_t chars[6]={0, 0, 0, 0, 0, 0};
    memcpy(chars, &font[ch-0x20], 5);
    for (int i = 0; i<6;i++) {
        writeCol(~(chars[i]));
    }
}

static void initDOTS(void) {
  DDRD|=(1<<PD2);
  DDRD|=(1<<PD3);
  DDRD|=(1<<PD4);

  _delay_ms(10);
}

static void writeCol(uint8_t col) {
  for (int i = 0; i<8; i++) {
    if (col & 1<<i) {
      PORTD |= DATAPIN;
    } else {
      PORTD &= ~DATAPIN;
    }
    clock();
  }
  _delay_us(BLANKING);
}

static void clock(void) {
  PORTD |= CLOCKPIN;
  _delay_us(100);
  PORTD &= ~CLOCKPIN;
  _delay_us(CLOCKPULSE);
}
