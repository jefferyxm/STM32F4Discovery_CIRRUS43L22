#include "main.h"
#include "stdint.h"
#include "sendAno.h"
#include "perip_func.h"

float	f32_cache[10], *f32;
int32_t i32_cache[10], *i32;
int16_t i16_cache[10], *i16;
extern int32_t temp;


void SendTO_ANO(uint8_t f32_num, uint8_t i32_num, uint8_t i16_num)
{
	union fi32
	{
		float u_f32;
		int32_t u_i32;
		int8_t fi32_i8[4];
	}fi32_un;

	union i16
	{
		int16_t u_i16;
		int8_t i16_i8[2];
	}i16_un;

	int8_t buff[64], check = 0;
	uint8_t first = 3;

	f32 = f32_cache;	i32 = i32_cache;	i16 = i16_cache;

	buff[0] = 0x88;
	buff[1] = 0xa1;
	buff[2] = f32_num * 4 + i32_num * 4 + i16_num * 2;			//֡ͷ�Ĵ���
	check = 0x88 + 0xa1 + buff[2];

	while (f32_num--)										//���������ݴ��뷢�ͻ�����
	{															//first�����˿̴���buff��λ��
		fi32_un.u_f32 = *f32++;								//���������ĸ������ݴ���f32_un������

		//��f32_un������ת���ַ����ж�ȡ
		buff[first] = fi32_un.fi32_i8[3];	check += buff[first++];
		buff[first] = fi32_un.fi32_i8[2];	check += buff[first++];
		buff[first] = fi32_un.fi32_i8[1];	check += buff[first++];
		buff[first] = fi32_un.fi32_i8[0];	check += buff[first++];
	}

	while (i32_num--)
	{
		fi32_un.u_i32 = *i32++;

		buff[first] = fi32_un.fi32_i8[3];	check += buff[first++];
		buff[first] = fi32_un.fi32_i8[2];	check += buff[first++];
		buff[first] = fi32_un.fi32_i8[1];	check += buff[first++];
		buff[first] = fi32_un.fi32_i8[0];	check += buff[first++];
	}

	while (i16_num--)
	{
		i16_un.u_i16 = *i16++;

		buff[first] = i16_un.i16_i8[1];	check += buff[first++];
		buff[first] = i16_un.i16_i8[0];	check += buff[first++];
	}

	buff[first++] = check;
	
	usart_sendMessage(USART2, (uint8_t *)buff, first);

}


int8_t i8_anoflag[6]; 
int16_t i16_anoval[6], *p_anoval;

void ano_sendDebug(uint8_t u8_flagnum,uint8_t u8_valnum)
{
    int8_t buffer[7] ={0};
    int8_t check=0;
    int8_t flagcnt=0; 
    int8_t valcnt=0;

    union UNION_i16
    {
        int16_t i16;
        int8_t i8[2];
    }uinon_i16;

    buffer[0] = 0x88;
    buffer[1] = 0xAD;
    while(u8_flagnum--)
    {
        buffer[2] = 0x02;
        buffer[3] = ++flagcnt;
        buffer[4] = *(i8_anoflag + flagcnt-1);
        check = 0x88+0xad+0x02+buffer[3]+buffer[4];
        buffer[5] = check;
        usart_sendMessage(USART2,(uint8_t *)buffer,6);
    }

    p_anoval = i16_anoval;   //上位机有bug，第六个数据显示不出来
    while(u8_valnum--)
    {
        buffer[2] = 0x03;
        buffer[3] = ++valcnt+6;
        uinon_i16.i16 = *(p_anoval++);
        buffer[4] = uinon_i16.i8[1];
        buffer[5] = uinon_i16.i8[0];
        check = 0x88+0xad+0x03+buffer[3]+buffer[4]+buffer[5];
        buffer[6]  = check;
        usart_sendMessage(USART2,(uint8_t *)buffer,7);
    }
}