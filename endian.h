#include<stdio.h>

int changeEndian(int a) { //masking  for changing the endian of an integer and shift
   a = ((a&(0x0000FFFF)) << 16) | ((a&(0xFFFF0000)) >> 16);
   a = ((a&(0x00FF00FF)) << 8) | ((a&(0xFF00FF00)) >>8);
   return a;
}
