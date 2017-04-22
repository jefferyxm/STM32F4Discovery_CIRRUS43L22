#ifndef __FILE_TEST_H
#define __FILE_TEST_H

#include "ff.h"
#include "diskio.h"
#include "main.h"
#include "stdlib.h"

void mount_fatfs(void);
void writFile();
void readFile();
UINT outStream(const BYTE *p, UINT c);
void read_speedtest();


typedef __packed struct 
{
    uint32_t riff; /* = "RIFF" 0x46464952*/
    uint32_t size_8; /* ���¸���ַ��ʼ���ļ�β�����ֽ��� */
    uint32_t wave; /* = "WAVE" 0x45564157*/

    uint32_t fmt; /* = "fmt " 0x20746d66*/
    uint32_t fmtSize; /* ��һ���ṹ��Ĵ�С(һ��Ϊ 16) */
    uint16_t wFormatTag; /* ���뷽ʽ,һ��Ϊ 1 */
    uint16_t wChannels; /* ͨ������������Ϊ 1��������Ϊ 2 */
    uint32_t dwSamplesPerSec; /* ������ */
    uint32_t dwAvgBytesPerSec; /* ÿ���ֽ���(= ������ �� ÿ���������ֽ���) */
    uint16_t wBlockAlign; /* ÿ���������ֽ���(=����������/8*ͨ����) */
    uint16_t wBitsPerSample; /* ����������(ÿ��������Ҫ�� bit ��) */

    uint32_t data; /* = "data" 0x61746164*/
    uint32_t datasize; /* �����ݳ��� */
} WavHead;


#endif