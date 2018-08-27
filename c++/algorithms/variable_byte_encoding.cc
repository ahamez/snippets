#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

size_t
compress_unsigned_int_1(unsigned int n, char* data)
{
  size_t size = 0;
  while (n  > 127)
  {
    ++size;
    *data++ = (n & 127)|128;
    n >>= 7;
  }
  *data++ = n;
  return ++size;
}

size_t
compress_unsigned_int_2(unsigned int n, char *data)
{
    if (n < (1U << 14)) {
        if (n < (1U << 7)) {
            data[0] = n;
            return 1;
        } else {
            data[0] = (n & 127) | 128;
            data[1] = ((n >> 7) & 127);
            return 2;
        }
    } else if (n < (1U << 28)) {
        if (n < (1U << 21)) {
            data[0] = (n & 127) | 128;
            data[1] = ((n >> 7) & 127) | 128;
            data[2] = (n >> 14) & 127;
            return 3;
        } else {
            data[0] = (n & 127) | 128;
            data[1] = ((n >> 7) & 127) | 128;
            data[2] = ((n >> 14) & 127) | 128;
            data[3] = (n >> 21) & 127;
            return 4;
        }
    } else {
        data[0] = (n & 127) | 128;
        data[1] = ((n >> 7) & 127) | 128;
        data[2] = ((n >> 14) & 127) | 128;
        data[3] = ((n >> 21) & 127) | 128;
        data[4] = (n >> 28) & 127;
        return 5;
    }
}

size_t
compress_unsigned_int_3(unsigned int n, char* data)
{
  size_t size;

  if (n < 0x00000080U) {
    size = 1;
    goto b1;
  }
  if (n < 0x00004000U) {
    size = 2;
    goto b2;
  }
  if (n < 0x00200000U) {
    size = 3;
    goto b3;
  }
  if (n < 0x10000000U) {
    size = 4;
    goto b4;
  }
  size = 5;

  *data++ = (n & 0x7f) | 0x80;
  n >>= 7;
b4:
  *data++ = (n & 0x7f) | 0x80;
  n >>= 7;
b3:
  *data++ = (n & 0x7f) | 0x80;
  n >>= 7;
b2:
  *data++ = (n & 0x7f) | 0x80;
  n >>= 7;
b1:
  *data = n;
  return size;
}

size_t
compress_unsigned_int_4(unsigned int n, char* data)
{
  const char* orig = data;

  // '__builtin_clz' => count leading zeros
  // switch(__builtin_clz(n + 1))
  switch( (31 - __builtin_clz(n)) / 7 )
  {
    case 4: *data++ = (n & 0x7f) | 0x80; n >>= 7;
    case 3: *data++ = (n & 0x7f) | 0x80; n >>= 7;
    case 2: *data++ = (n & 0x7f) | 0x80; n >>= 7;
    case 1: *data++ = (n & 0x7f) | 0x80; n >>= 7;
    case 0: *data = n;
  }

  return data - orig;
}

int
main(int argc, char** argv)
{
  srandomdev();
  char data[16];
  char data_1[16];
  char data_2[16];

  size_t i = 0;
  size_t j = 0;

  for (i = 0; i < UINT_MAX; ++i)
  {
    memset(data, 0, 16);
    // memset(data_1, 0, 16);
    // memset(data_2, 0, 16);

    const int r = random();
    // const int r = i;
    // compress_unsigned_int_1(r, data);
    compress_unsigned_int_2(r, data);
    // compress_unsigned_int_3(r, data);
    // compress_unsigned_int_4(r, data);

    // if(memcmp(data_1, data_2, 16))
    // {
    //   printf("i = %lu\n",i);
    //   for (j = 0; j < 16; ++j)
    //   {
    //     printf("%hhu|",data_1[j]);
    //   }
    //   printf("\n\n");
    //   for (j = 0; j < 16; ++j)
    //   {
    //     printf("%hhu|",data_2[j]);
    //   }
    //   printf("\n");
    //   exit(1);
    // }
  }
  
  return 0;
}