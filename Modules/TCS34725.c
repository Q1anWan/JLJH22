/****************************
	½®À´¼Æ»®2022
*****************************	
	Name	:TCS34725.c
	Function:RGB Sensor API
	Author	:qianwan
*****************************/
#include "TCS34725.h"
#define false 0
#define true  1
uint16_t TIME_OUT = 1000;

/* Write a 1-Byte long data */
void TCS_Write8(uint8_t reg, uint32_t value)
{
	uint8_t buf[2];
	buf[0] = (TCS34725_WRITE_BIT | reg);
	buf[1] = (uint8_t)(value & 0xFF);
	
	HAL_I2C_Master_Transmit(&I2C_Port, TCS34725_ADDRESS, buf, 2, TIME_OUT);
}

/* Read a 1-Byte long data */
uint8_t TCS_Read8(uint8_t reg)
{
	uint8_t buf;
	buf = (TCS34725_WRITE_BIT | reg);
	
	HAL_I2C_Master_Transmit(&I2C_Port, TCS34725_ADDRESS, &buf, 1, TIME_OUT);
	HAL_I2C_Master_Receive(&I2C_Port, TCS34725_ADDRESS, &buf, 1, TIME_OUT);
	
	return buf;
}

/* Read a 2-Byte long data */
uint16_t TCS_Read16(uint8_t reg)
{
	uint16_t data;
	uint8_t buf[2];
	buf[0] = (TCS34725_WRITE_BIT | reg);
	
	HAL_I2C_Master_Transmit(&I2C_Port, TCS34725_ADDRESS, buf, 1, TIME_OUT);
	HAL_I2C_Master_Receive(&I2C_Port, TCS34725_ADDRESS, buf, 2, TIME_OUT);
	
	data  = buf[0];
	data <<= 8;
	data |= buf[1];
	
	return data;
}

/* Turn the device on to save power */
void TCS_Enable(void)
{
	TCS_Write8(TCS34725_ENABLE, TCS34725_ENABLE_PON);
	TCS_Write8(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);  
}

/* Turn the device off to save power */
void TCS_Disable(void)
{ 
  uint8_t reg = 0;
  reg = TCS_Read8(TCS34725_ENABLE);
  TCS_Write8(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}

/*Set Integration Time*/
void TCS_Set_I(TCS_t *TCS, TCS_I_TIME TIME)
{
  if (!TCS->Initialised) TCS_Init(TCS);

  /* Update the timing register */
  TCS_Write8(TCS34725_ATIME, TIME);

  /* Update value placeholders */
  TCS->I_Time = TIME;
}

/*Set Integration Gain*/
void TCS_Set_Gain(TCS_t *TCS, TCS_GAIN_TIME Gain)
{
  if (!TCS->Initialised) TCS_Init(TCS);

  /* Update the timing register */
  TCS_Write8(TCS34725_CONTROL, Gain);

  /* Update value placeholders */
  TCS->Gain = Gain;
}

/*Get Raw Data from Senser*/
void TCS_GetRawData(TCS_t *TCS)
{
  if (!TCS->Initialised) TCS_Init(TCS);

  TCS->Data_RAW.C_rawdata = TCS_Read16(TCS34725_CDATAL);
  TCS->Data_RAW.R_rawdata = TCS_Read16(TCS34725_RDATAL);
  TCS->Data_RAW.G_rawdata = TCS_Read16(TCS34725_GDATAL);
  TCS->Data_RAW.B_rawdata = TCS_Read16(TCS34725_BDATAL);
}

/*Calculate RGB Value*/
void TCS_CalRGB(TCS_t *TCS)
{
  TCS->Color.R = TCS->Data_RAW.R_rawdata * TCS->Data_RAW.rawtoRGB_factor;
  TCS->Color.G = TCS->Data_RAW.G_rawdata * TCS->Data_RAW.rawtoRGB_factor;
  TCS->Color.B = TCS->Data_RAW.B_rawdata * TCS->Data_RAW.rawtoRGB_factor;	
}

/*Init Sensor */
uint8_t TCS_Init(TCS_t *TCS)
{
	TCS->Statue = TCS_Read8(TCS34725_ID);
	
	if (!((TCS->Statue == 0x44)||(TCS->Statue == 0x4D)))
	{
		return false;
	}
	TCS->Initialised = true;
	
	TCS_Set_I(TCS,TCS34725_INTEGRATIONTIME_24MS);
	TCS->Data_RAW.rawtoRGB_factor = TCS34725_toRGB_700MS;
	TCS_Set_Gain(TCS,TCS34725_GAIN_4X );

	/* Note: by default, the device is in power down mode on bootup */
	TCS_Enable();
	return true;
}