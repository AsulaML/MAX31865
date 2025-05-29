#ifndef MAX31865_H
#define	MAX31865_H

#include <xc.h> // include processor files - each processor file is guarded.  


#define MAX31856_R_REF       130.00
#define MAX31856_ADC_RESOLUTION       32768

// TODO Insert declarations
#define MAX31856_CONFIG_REG_READ       0x00
#define MAX31856_CONFIG_REG_WRITE      0x80
#define MAX31856_CONFIG_BIAS           0x80
#define MAX31856_CONFIG_MODEAUTO       0x40
#define MAX31856_CONFIG_MODEOFF        0x00
#define MAX31856_CONFIG_1SHOT          0x20
#define MAX31856_CONFIG_3WIRE          0x10
#define MAX31856_CONFIG_24WIRE         0x00
#define MAX31856_CONFIG_FAULTSTAT      0x02
#define MAX31856_CONFIG_FILT50HZ       0x01
#define MAX31856_CONFIG_FILT60HZ       0x00

#define MAX31856_RTDMSB_REG           0x01
#define MAX31856_RTDLSB_REG           0x02
#define MAX31856_HFAULTMSB_REG        0x03
#define MAX31856_HFAULTLSB_REG        0x04
#define MAX31856_LFAULTMSB_REG        0x05
#define MAX31856_LFAULTLSB_REG        0x06
#define MAX31856_FAULTSTAT_REG        0x07

#define MAX31865_FAULT_HIGHTHRESH     0x80
#define MAX31865_FAULT_LOWTHRESH      0x40
#define MAX31865_FAULT_REFINLOW       0x20
#define MAX31865_FAULT_REFINHIGH      0x10
#define MAX31865_FAULT_RTDINLOW       0x08
#define MAX31865_FAULT_OVUV           0x04


void MAX31865_INIT(uint8_t VALUE, uint8_t ADMOD);
void MAX31865_WRITE_REGISTER(uint8_t NUM_REG, uint8_t VAL, uint8_t ADRESS);
uint8_t MAX31865_READ_REGISTER8(uint8_t ADR_REG,uint8_t ADRMAX);
uint16_t MAX31865_READ_VALPT100(uint8_t ADR_REGRES,uint8_t ADREMAX);
int16_t ComputeTemperatureFromResistanceMeasurement(double RESISTANCE);
void Set_CS(uint8_t ADREMAX);
void Release_CS(void);
void Init_MAX31865(void);
#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

