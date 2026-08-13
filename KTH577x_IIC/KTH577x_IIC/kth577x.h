#ifndef __KTH577X_H__
#define __KTH577X_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define Kth577xI2CReadXYData          0xB0
    
#define STA_IIC_ACK   0
#define STA_IIC_NACK  1
    
extern uint8_t  KTH577X_IIC_ADDRESS;
    
void KTH577XGpioInit(void);
    
uint8_t KTH577XReadMeasureDataIIC(uint8_t* outdata, uint16_t outdataaddr);
uint8_t KTH577XSendCommandIIC(uint8_t *command,uint8_t length);
uint8_t KTH577XReadDataIIC(uint8_t *data,uint8_t num, uint32_t delaytime);

void Kth577xDelayUs(uint32_t t);
void Kth577xDelayMs(uint32_t t);

#ifdef __cplusplus
}
#endif

#endif /* __KEY_H__ */

