#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/spi2.h"
#include "mcc_generated_files/spi1.h"
#include "mcc_generated_files/pin_manager.h"
#include "MAX31865.h"
#include <math.h>

PT100Sensor PT100_S1 = { .cs_id = 0x00, .spi = SPI_IF1 };
PT100Sensor PT100_S2 = { .cs_id = 0x01, .spi = SPI_IF1 };
PT100Sensor PT100_BF = { .cs_id = 0x05, .spi = SPI_IF2 };

/**
 * \fn MAX31865_Init
 * \brief Fonction permet d'initialiser l'instance du composant.
 * \param    PT100Sensor sensor
 * \return   void
 */
void MAX31865_Init(PT100Sensor sensor)
{
    MAX31865_Write_Register(sensor, MAX31856_CONFIG_REG_WRITE, 0xC3);        
    __delay_ms(100);                                                 
}


/**
 * \fn MAX31865_Read_Resistance
 * \brief Fonction qui permet de lire la valeur de la résistance.
 * \param    PT100Sensor sensor
 * \return   uint16_t
 */
uint16_t MAX31865_Read_Resistance(PT100Sensor sensor)
{
    uint8_t cmd = MAX31856_RTDMSB_READ | 0x80;
    uint8_t val[2];

    MAX31865_CS_Select(sensor.cs_id);
    if (sensor.spi == SPI_IF1) 
    {
        SPI1_ExchangeByte(cmd);
        SPI1_ReadBlock(val, 2);
    } 
    else 
    {
        SPI2_ExchangeByte(cmd);
        SPI2_ReadBlock(val, 2);
    }
    MAX31865_CS_Deselect();
    uint16_t raw = ((uint16_t)val[0] << 8) | val[1];
    raw >>= 1;

    sensor.Resistance = (double)raw * MAX31856_R_REF / MAX31856_ADC_RESOLUTION;

    return  (uint16_t)(sensor.Resistance*100.0); 
} 

/**
 * \fn ComputeResistanceToTemperature
 * \brief Fonction qui convertit une résistance en température.
 *  à tester pour la mesure de température négative
 * \param    double Rt : résistance mesurée 
 * \return   int16_t : status_reg
 */
int16_t ComputeResistanceToTemperature(double Rt)
{
    const double R0    = 100.0;

    double a     = 3.9083E-03;
    double b     = -5.7750E-07;
    double c     = -4.183E-12;

    double CalculatedTempInCent = 0;

    double T = -200.0;
    double Rcalc;
    
    if (Rt >= R0) 
    {
        // T >= 0°C : résolution directe
        double discriminant = a*a - 4*b*(1 - Rt / R0);
        if (discriminant < 0) return -999.9; // erreur
        T = (-a + sqrt(discriminant)) / (2*b);
    } 
    else 
    {
        // T < 0°C : résolution par itération (Newton-Raphson ou binaire)
        double Tmin = -200.0, Tmax = 0.0;
        while ((Tmax - Tmin) > 0.01) 
        {
            T = (Tmax + Tmin) / 2.0;
            Rcalc = R0 * (1 + a*T + b*T*T + c*(T - 100)*T*T*T);
            if (Rcalc < Rt)
                Tmax = T;
            else
                Tmin = T;
        }
    }
    CalculatedTempInCent = T * 100.0;

    return((int16_t)(CalculatedTempInCent));
}


/**
 * \fn MAX31865_Write_Register
 * \brief Fonction permet d'écrire dans un registre du composant.
 * \param    PT100Sensor sensor
 * \param    uint8_t reg
 * \param    uint8_t value
 * \return   void
 */
void MAX31865_Write_Register(PT100Sensor sensor, uint8_t reg, uint8_t value)
{
    uint8_t data[2] = { reg & 0x7F, value };

    MAX31865_CS_Select(sensor.cs_id);
    if (sensor.spi == SPI_IF1)
        SPI1_WriteBlock(data, 2);
    else
        SPI2_WriteBlock(data, 2);
    MAX31865_CS_Deselect();
}

/**
 * \fn MAX31865_Read_Register
 * \brief Fonction qui permet de lire la valeur d'un registre.
 * \param    PT100Sensor sensor
 * \return   uint8_t
 */
uint8_t MAX31865_Read_Register(PT100Sensor sensor)
{
    uint8_t val;

    MAX31865_CS_Select(sensor.cs_id);
    if (sensor.spi == SPI_IF1) 
    {
        SPI1_ExchangeByte(cmd);
        SPI1_ReadBlock(val, 1);
    } 
    else 
    {
        SPI2_ExchangeByte(cmd);
        SPI2_ReadBlock(val, 1);
    }
    MAX31865_CS_Deselect();

    return(val);
}

/**
 * \fn MAX31865_CS_Select
 * \brief Fonction qui permet de sélectionner le bon CS.
 * \param    PT100Sensor sensor
 * \return   void
 */
void MAX31865_CS_Select(PT100Sensor sensor)
{
    switch(sensor.cs_id)
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

/**
 * \fn MAX31865_CS_Deselect
 * \brief Fonction qui permet de désélectionner les CS.
 * \param    void
 * \return   void
 */
void MAX31865_CS_Deselect(void)
{
    CS_MES1_SetHigh();
    CS_MES2_SetHigh();
    CS_SREG_SetHigh();
}