
#ifndef _Send_ANO_H_
#define	_Send_ANO_H_

#include "stdint.h"

extern float	f32_cache[10];
extern int32_t i32_cache[10];
extern int16_t i16_cache[10];

extern int8_t i8_anoflag[6];
extern int16_t i16_anoval[6];

void SendTO_ANO(uint8_t f32_num, uint8_t i32_num, uint8_t i16_num);
void ano_sendDebug(uint8_t u8_flagnum,uint8_t u8_valnum);
#endif