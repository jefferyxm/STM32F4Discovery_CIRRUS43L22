#ifndef __FILE_TEST_H
#define __FILE_TEST_H

#include "ff.h"
#include "diskio.h"
#include "main.h"
#include "stdlib.h"
#include <stdbool.h>

void format_fatfs(char *rootPath);
bool writFile(char *rootPath, char *fileName, char* fileBuff, uint32_t buffSize);
void readFile(char *rootPath, char *fileName, char* fileBuff, uint32_t buffSize);
UINT outStream(const BYTE *p, UINT c);
void read_speedtest(char *rootPath, char *fileName);
#endif