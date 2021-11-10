#include "LCD_4bit.h"
#include "delay.h"
#include <stdint.h>

uint8_t lcd_read_status(void)
{
  uint8_t status;

  SET_LCD_DATA_DIR_IN
  SET_LCD_RS(0)
  SET_LCD_RW(1)
  Delay(1);
  SET_LCD_E(1)
  Delay(1);
  status  = GET_LCD_DATA_IN << 4;
  SET_LCD_E(0)
  Delay(1);
  SET_LCD_E(1)
  Delay(1);
  status |= GET_LCD_DATA_IN;
  SET_LCD_E(0)
  SET_LCD_DATA_DIR_OUT
  return(status);
}

void wait_while_busy(void)
{
	for( ; lcd_read_status() & LCD_BUSY_FLAG_MASK; )
		;
}

void lcd_write_4bit(uint8_t c)
{
  SET_LCD_RW(0)
  SET_LCD_E(1)
  SET_LCD_DATA_OUT(c&0x0F)
  Delay(1);
  SET_LCD_E(0)
  Delay(1);
	//0000 1111
	//0000 0011
}

void lcd_write_cmd(uint8_t c)
{
  wait_while_busy();

  SET_LCD_RS(0)
  lcd_write_4bit(c>>4);
  lcd_write_4bit(c);
}

static void lcd_write_data(uint8_t c)
{
  wait_while_busy();

  SET_LCD_RS(1)
  lcd_write_4bit(c>>4);
  lcd_write_4bit(c);
}

void lcd_putchar(char c)
{ 
  lcd_write_data(c);
}

void lcd_init_port(void) {
	/* Enable clocks for peripherals        */
  ENABLE_LCD_PORT_CLOCKS                          

	/* Set Pin Mux to GPIO */
	PIN_DATA_PORT->PCR[PIN_DATA_SHIFT] = PORT_PCR_MUX(1);
	PIN_DATA_PORT->PCR[PIN_DATA_SHIFT+1] = PORT_PCR_MUX(1);
	PIN_DATA_PORT->PCR[PIN_DATA_SHIFT+2] = PORT_PCR_MUX(1);
	PIN_DATA_PORT->PCR[PIN_DATA_SHIFT+3] = PORT_PCR_MUX(1);
	PIN_E_PORT->PCR[PIN_E_SHIFT] = PORT_PCR_MUX(1);
	PIN_RW_PORT->PCR[PIN_RW_SHIFT] = PORT_PCR_MUX(1);
	PIN_RS_PORT->PCR[PIN_RS_SHIFT] = PORT_PCR_MUX(1);
}

void Init_LCD(void)
{ 
	/* initialize port(s) for LCD */
	lcd_init_port();
	
  /* Set all pins for LCD as outputs */
  SET_LCD_ALL_DIR_OUT
  Delay(100);
  SET_LCD_RS(0)
  lcd_write_4bit(0x3);                 
  Delay(100);
  lcd_write_4bit(0x3);
  Delay(10);
  lcd_write_4bit(0x3); //set to 8 bit bus mode? 3 times?
  lcd_write_4bit(0x2); //set to 4bit bus mode
  lcd_write_cmd(0x28);//0010 1000 calling Function set 4 bit bus mode, 2 lines of display, 5x8 dots per char
  lcd_write_cmd(0x0C);//1100      calling 'Display ON/OFF Control' with display on, cursor and blinking cursor off
  lcd_write_cmd(0x06);//0110      calling 'Entry Mode Set', cursor moves right and DDRAM+1, dont shift display
  lcd_write_cmd(0x80);//1000 0000 calling 'Set DDRAM Address' to return DDRAM address to 0
}

void Set_Cursor(uint8_t column, uint8_t row)
{
  uint8_t address;

  address =(row * 0x40) + column;
	address |= 0x80;
  lcd_write_cmd(address);               
}

void Clear_LCD(void)
{
  lcd_write_cmd(0x01); //                
  Set_Cursor(0, 0);
}

void Print_LCD(char *string)
{
  while(*string)  {
    lcd_putchar(*string++);
  }
}
