#include <reg52.h>
#include "LCDh.h"
#include <stdio.h>
#include <intrins.h>

sbit DS1302_CE = P3^5; // DS1302 chip enable pin
sbit DS1302_CLK = P3^6; // DS1302 clock pin
sbit DS1302_IO = P3^4; // DS1302 data I/O pin

// DS1302 commands
#define DS1302_READ_BURST 0xBF
#define DS1302_WRITE_BURST 0xBE
#define DS1302_SECONDS 0x80
#define DS1302_SECONDS_read 0x81
#define DS1302_MINUTES 0x82
#define DS1302_MINUTES_read 0x83
#define DS1302_HOURS   0x84
#define DS1302_HOURS_read  0x85
#define DS1302_DATE    0x86
#define DS1302_MONTH   0x88
#define DS1302_DAY     0x8A
#define DS1302_YEAR    0x8C
#define DS1302_CONTROL 0x8E
#define DS1302_CHARGER 0x90
#define DS1302_RAMBURST_READ 0xFF
#define DS1302_RAMBURST_WRITE 0xFE
#define CLK_DELAY 10



void Write_Byte(unsigned int Value);
unsigned char Read_Byte(unsigned char cmd);
void DS1302_SetTime(unsigned char hour,  unsigned char minute, unsigned char second);
void DS1302_SetDate(unsigned char day,  unsigned char month,  unsigned char year);
void DS1302_GetTime(void);
void DS1302_GetDate(void);
void Send_Bit(unsigned int bitt);
void DS1302_init();
void DS1302_ReadByte();

    unsigned char hour, minute, second;
    unsigned char day, month, year;
		unsigned char timeString[9];
    unsigned int read_bytes[12];

void main()
{

	  lcd_init();         // Initialize the LCD
	  DS1302_init();        // Initialize ds1302
	
	   // Set initial time and date
     DS1302_SetTime(0x23, 0x59, 0x50); 
     DS1302_SetDate(0x30, 0x09, 0x23); 

    while (1)
    {	  
				DS1302_CE = 1;
			  DS1302_ReadByte();
				DS1302_CE = 0;

        DS1302_GetTime();
			  DS1302_GetDate();

    }
}

void SCLK_Pulse()
{
    DS1302_CLK = 1;
    DS1302_CLK = 0;
}

void Send_Bit(unsigned int bitt)// Function to send a bit to the DS1302
{
		if(bitt != 0)
			{
		
			DS1302_IO = 1;
		}
		else
			{
			DS1302_IO = 0;
		}
}


void write_byte(unsigned int byte)
{
    unsigned int i,Bit;
    
    for (i = 0; i < 8; i++)
    {
        Bit = byte & (1 << i); // Get each bit
        Send_Bit(Bit);         // Send the bit
        SCLK_Pulse();        // Generate clock pulse
    }
}

unsigned char read_byte(unsigned char cmd)
{
	  unsigned int j,dat = 0;
	  write_byte(cmd);

     for (j = 0; j < 8; j++)
      {
        dat >>= 1;
        if (DS1302_IO)
        {
            dat |= 0x80;     // Set the corresponding bit in R
        }
        else
        {
            dat &= 0x7F;    // Clear the corresponding bit in R
        }

        SCLK_Pulse();       // Generate clock pulse
			}
    return dat;
}
			

// Function to write a byte to the DS1302unsigned char cmd,
void DS1302_WriteByte(unsigned char cmd, unsigned char Value)
{
   
	  DS1302_CE = 1;
    write_byte(cmd);
	  write_byte(Value);
    DS1302_CE = 0;
}


// Function to read a byte from the DS1302
void DS1302_ReadByte()
{
    unsigned int dat=0, j,i;
	
    DS1302_CE = 1;
	  write_byte(DS1302_READ_BURST);

	for( i = 0; i<8; i++){
		
		dat = 0;
		
    for (j = 0; j < 8; j++)          // Receive the data byte
    {
        dat >>= 1;
        if (DS1302_IO)
        {
            dat |= 0x80;     // Set the corresponding bit in R
        }
        else
        {
            dat &= 0x7F;    // Clear the corresponding bit in R
        }

        SCLK_Pulse();       // Generate clock pulse
    }
		
		read_bytes[i] = dat;
		
	}
    DS1302_CE = 0;

}

void DS1302_init()
{
	unsigned char r;
	
	    DS1302_CE = 0;
	    DS1302_CLK = 0;
			DS1302_WriteByte(DS1302_CONTROL,0x00);
	    DS1302_WriteByte(DS1302_CHARGER,0xAB);
	
	r = read_byte(DS1302_SECONDS_read);
	
	if ((r & DS1302_SECONDS_read) != 0)
	DS1302_WriteByte(DS1302_SECONDS,0);
			
}
void DS1302_SetTime(unsigned char hour, unsigned char minute, unsigned char second)
{

	  DS1302_WriteByte(DS1302_SECONDS,second);
    DS1302_WriteByte(DS1302_MINUTES,minute);
    DS1302_WriteByte(DS1302_HOURS,hour);
 
}

void LCD_display(unsigned char Number)
{
	lcd_write_data(((Number>>4)&0x0F) + 0x30);
	lcd_write_data((Number&0x0F) + 0x30);
}

void DS1302_GetTime(void)
{
	
	      lcd_set_cursor(0, 0);
	      lcd_write_string("Time:");
	
        lcd_set_cursor(0, 6);
		    LCD_display(read_bytes[2]);	
      	lcd_write_data(':');
	      delay_ms(10);
	
        lcd_set_cursor(0, 9);
		    LCD_display(read_bytes[1]);
		    lcd_write_data(':');
      	delay_ms(10);

	      lcd_set_cursor(0, 12);
	      LCD_display(read_bytes[0]);
  		  delay_ms(10);	
}

// Function to set the date in DS1302
void DS1302_SetDate(unsigned char day, unsigned char month, unsigned char year)
{
	
    DS1302_WriteByte(DS1302_DATE,day);
    DS1302_WriteByte(DS1302_MONTH,month);
    DS1302_WriteByte(DS1302_YEAR,year);
}


 //Function to read the date from DS1302
void DS1302_GetDate()
{
        lcd_set_cursor(1, 0);
	      lcd_write_string("Date:");
	
        lcd_set_cursor(1, 6);
		    LCD_display(read_bytes[3]);	
      	lcd_write_data(':');
	      delay_ms(10);
	
        lcd_set_cursor(1, 9);
		    LCD_display(read_bytes[4]);
		    lcd_write_data(':');
      	delay_ms(10);

	      lcd_set_cursor(1, 12);
		    LCD_display(read_bytes[6]);
		    delay_ms(10);
}
