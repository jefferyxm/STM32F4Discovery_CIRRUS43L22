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
#endif