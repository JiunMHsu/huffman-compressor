
#ifndef _TARR_BITWRITER_
#define _TARR_BITWRITER_

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "../funciones/strings.hpp"

using namespace std;

typedef unsigned char uchar;

struct BitWriter
{
   FILE *file;
   string byte;
};

BitWriter bitWriter(FILE *f)
{
   return {f, ""};
}

void bitWriterWrite(BitWriter &bw, int bit)
{
   bw.byte += charToString(intToChar(bit));

   if (length(bw.byte) == 8)
   {
      int byte = stringToInt(bw.byte, 2);
      uchar ucByte = (uchar)byte;

      write<uchar>(bw.file, ucByte);
      
      bw.byte = "";
   }
}

void bitWriterFlush(BitWriter &bw)
{
   if (length(bw.byte) < 8 && length(bw.byte) > 0)
   {
      bw.byte = rpad(bw.byte, 7, '0');
      bitWriterWrite(bw, 0);
   }
}

#endif
