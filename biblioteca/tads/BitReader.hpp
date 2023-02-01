
#ifndef _TARR_BITREADER_
#define _TARR_BITREADER_

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "../funciones/strings.hpp"

using namespace std;

typedef unsigned char uchar;

struct BitReader
{
   FILE *file;
   string byte;
};

BitReader bitReader(FILE *f)
{
   return {f, ""};
}

string binToString(unsigned char c)
{
   string s = "";
   for (int i = 0; i < 8; i++)
   {
      uchar mask = (char)pow(2, i);
      uchar res = c & mask;
      s = (res == 0) ? '0' + s : '1' + s;
   }
   return s;
}

/**
 * @param BitReader (objeto bitReader) 
 * @return int (1 o 0 implican bit leido // -1 implica final del archivo)
 */
int bitReaderRead(BitReader &br)
{
   // early return en caso de fin de archivo
   if (feof(br.file)) return -1;

   if (isEmpty(br.byte))
   {
      uchar ucByte = read<uchar>(br.file);
      br.byte = binToString(ucByte);
   }
   char bit = br.byte[0];
   br.byte = removeAt(br.byte, 0);
   return (bit == '0') ? 0 : 1;
}

#endif
