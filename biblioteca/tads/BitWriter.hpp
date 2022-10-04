
#ifndef _TARR_BITWRITER_
#define _TARR_BITWRITER_

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "../funciones/strings.hpp"

using namespace std;

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
      u_char ucByte = (unsigned char)byte;

      write<u_char>(bw.file, ucByte);
      
      bw.byte = "";
   }
}

void bitWriterFlush(BitWriter &bw)
{
   bw.byte = rpad(bw.byte, 7, '0');
   bitWriterWrite(bw, 0);
}

#endif
