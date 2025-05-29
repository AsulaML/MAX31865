/************************************************************************************/
/*      Sous Programme de gestion du MAX31865 Circuit de mesure de PT100            */
/*                                                          14/XII/2023             */
/************************************************************************************/
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/spi2.h"
#include "mcc_generated_files/spi1.h"
#include "mcc_generated_files/pin_manager.h"
#include "MAX31865.h"
#include <math.h>

void Init_MAX31865(void)
{
    if(SPI2_Open(SPI2_DEFAULT)& SPI1_Open(SPI1_DEFAULT))
    {
        __delay_ms(500);
        MAX31865_WRITE_REGISTER(MAX31856_CONFIG_REG_WRITE,0xC3,1);
        __delay_ms(200);
        MAX31865_WRITE_REGISTER(MAX31856_CONFIG_REG_WRITE,0xC3,0);
        __delay_ms(200);
        MAX31865_WRITE_REGISTER(MAX31856_CONFIG_REG_WRITE,0xC3,5);
        __delay_ms(200);
    }
}

void MAX31865_WRITE_REGISTER(uint8_t NUM_REG, uint8_t VAL, uint8_t ADRESS)
{
    uint8_t DATASPI[2];
    
    DATASPI[0] = NUM_REG;
    DATASPI[1] = VAL;
    
    
    Set_CS(ADRESS);

    __delay_us(10);
    
    if(ADRESS <= 1)
    {    
        SPI1_WriteBlock(&DATASPI, 2);
    }    
    else
    {    
        SPI2_WriteBlock(&DATASPI, 2);
    }
    __delay_us(10);
    Release_CS();
}


void MAX31865_INIT(uint8_t VALUE, uint8_t ADMOD)
{
    
    MAX31865_WRITE_REGISTER(MAX31856_CONFIG_REG_WRITE,0xC3,ADMOD);        //BIAS, AUTO, 50Hz
    __delay_ms(63);                                                 //minimum 62.5mSec before a result can be read
}


uint8_t MAX31865_READ_REGISTER8(uint8_t ADR_REG,uint8_t ADRMAX)
{
    uint8_t VALREG;
    
    Set_CS(ADRMAX);

    
    __delay_us(10);
    if(ADRMAX <= 1)
    {    
        VALREG = SPI1_ExchangeByte(ADR_REG);
        SPI1_ReadBlock(&VALREG, 1);
    }    
    else
    {    
        VALREG = SPI2_ExchangeByte(ADR_REG);
        SPI2_ReadBlock(&VALREG, 1);
    }
 
    __delay_us(10);
    
    Release_CS();
    
    return(VALREG);
}

uint16_t MAX31865_READ_VALPT100(uint8_t ADR_REGRES,uint8_t ADREMAX)
{
    uint16_t VALUPT100;
    uint8_t VALRES[2],DUMMY;
    double VALRESPT100;
    
    Set_CS(ADREMAX);
    
    
    __delay_us(10);
    if (ADREMAX <= 1)
    {    
        DUMMY = SPI1_ExchangeByte(ADR_REGRES);
        SPI1_ReadBlock(&VALRES, 2);
    }
    else
    {
        DUMMY = SPI2_ExchangeByte(ADR_REGRES);
        SPI2_ReadBlock(&VALRES, 2);
    }    
    __delay_us(10);    
    
    Release_CS();
    
    VALUPT100 = ((uint16_t)(VALRES[0])*256)+VALRES[1];
    
    VALUPT100 = VALUPT100 >> 1;
    VALRESPT100 = (VALUPT100)*130.00;
    VALRESPT100 = VALRESPT100 / (double)(32768);
    VALRESPT100 = VALRESPT100 * 100.0;
    
    return (uint16_t)(VALRESPT100);
} 

int16_t ComputeTemperatureFromResistanceMeasurement(double MeasuredResistance)
{
    double a     = 3.9083E-03;
    double b     = -5.7750E-07;
    double R0    = 100.0;
    double CalculatedTempInCent = 0;
    double tmp_1 = 0;
    double tmp_2 = 0;
    double tmp_3 = 0;
    
    if(MeasuredResistance >= R0)
    {
        tmp_1 = 1.0 - (MeasuredResistance/R0);
        tmp_1 *= (4.0*b);
        tmp_2 = (a*a);
        
        tmp_3 = sqrt(tmp_2 - tmp_1);
        tmp_3 -= a; 
        tmp_3 /= (2.0*b);
        
        CalculatedTempInCent = (tmp_3*100.00);
    }    
    return((int16_t)(CalculatedTempInCent));
}  

void Set_CS(uint8_t ADREMAX)
{
    switch(ADREMAX)
    {
        case 0x00:                  /* PT100 J5 @0 sonde 1*/
            CS_MES1_SetLow();
            
            CS_MES2_SetHigh();
            CS_SREG_SetHigh();
        break;
        
        case 0x01:                  /* PT100 J4 @1  BF1 */
            CS_MES1_SetHigh();
            
            CS_MES2_SetLow();
            CS_SREG_SetHigh();
        break;
        
        case 0x05:                  /* PT 100 J6 Mesure chocolat */
            CS_MES1_SetHigh();
            
            CS_MES2_SetHigh();
            CS_SREG_SetLow();
        break;    
    }
}

void Release_CS(void)
{
    CS_MES1_SetHigh();
    CS_MES2_SetHigh();
    CS_SREG_SetHigh();
}