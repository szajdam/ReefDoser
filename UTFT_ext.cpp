#include "UTFT_ext.h"

void UTFT_ext::lcdOff()
{
	cbi(P_CS, B_CS);
	//turn OFF
	LCD_Write_COM_DATA(0x07,0x0000);
	LCD_Write_COM_DATA(0x00,0x0000);
	LCD_Write_COM_DATA(0x10,0x0001);
		
	//only standby mode ON : no visible difference (?)
	//LCD_Write_COM_DATA(0x10,0x0001);
	
	sbi(P_CS, B_CS);
}

void UTFT_ext::lcdOn()
{
	cbi(P_CS, B_CS);
	LCD_Write_COM_DATA(0x07,0x0021);
	LCD_Write_COM_DATA(0x00,0x0001);
	LCD_Write_COM_DATA(0x07,0x0023);
	LCD_Write_COM_DATA(0x10,0x0000);
	delay(30);
	LCD_Write_COM_DATA(0x07,0x0033);
	LCD_Write_COM_DATA(0x11,0x60B0);
	LCD_Write_COM_DATA(0x02,0x0600);
	LCD_Write_COM(0x22);
		
	//only standby mode OFF : no visible difference (?)
	//LCD_Write_COM_DATA(0x10,0x0000);
	
	sbi(P_CS, B_CS);
}



