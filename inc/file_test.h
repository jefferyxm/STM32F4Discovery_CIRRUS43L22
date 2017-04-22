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
    uint32_t size_8; /* 从下个地址开始到文件尾的总字节数 */
    uint32_t wave; /* = "WAVE" 0x45564157*/

    uint32_t fmt; /* = "fmt " 0x20746d66*/
    uint32_t fmtSize; /* 下一个结构体的大小(一般为 16) */
    uint16_t wFormatTag; /* 编码方式,一般为 1 */
    uint16_t wChannels; /* 通道数，单声道为 1，立体声为 2 */
    uint32_t dwSamplesPerSec; /* 采样率 */
    uint32_t dwAvgBytesPerSec; /* 每秒字节数(= 采样率 × 每个采样点字节数) */
    uint16_t wBlockAlign; /* 每个采样点字节数(=量化比特数/8*通道数) */
    uint16_t wBitsPerSample; /* 量化比特数(每个采样需要的 bit 数) */

    uint32_t data; /* = "data" 0x61746164*/
    uint32_t datasize; /* 纯数据长度 */
} WavHead;


#endif