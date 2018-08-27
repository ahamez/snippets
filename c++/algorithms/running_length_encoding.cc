#include <assert.h>
#include <limits.h>
#include <stdio.h>

// size_t
// rle_int(size_t size, int* src, char* dest)
// {
//   const char* orig = dest;
//   size_t i = 0;
//   for( i = 0 ; i < size ; ++i )
//   {
//     unsigned char run_length = 1;
//     while (i+1 < size && src[i] == src[i+1] && run_length < UCHAR_MAX)
//     {
//       ++run_length;
//       ++i;
//     }
// 
//     *dest = run_length;
//     dest += sizeof(unsigned char);
//     *(int*)dest = src[i];
//     dest += sizeof(int);
//   }
//   return dest - orig;
// }
// 
// size_t
// unrle_int(size_t size, char* src, int* dest)
// {
//   const char* orig = src;
//   while ((src - orig) < size)
//   {
//     const unsigned char run_length = *src++;
//     size_t j = 0;
//     for(j = 0; j < run_length; ++j )
//     {
//       *dest++ = *(int*)src;
//     }
//     src += sizeof(int);
//   }
//   return src - orig;
// }

// Works char by char
size_t
rle(size_t size, char* src, char* dest)
{
  const char* orig = dest;
  size_t i = 0;
  for( i = 0 ; i < size ; ++i )
  {
    unsigned char run_length = 1;
    while( i+1 < size && src[i] == src[i+1] && run_length < UCHAR_MAX)
    {
      ++run_length;
      ++i;
    }

    *dest++ = run_length;
    *dest++ = src[i];
  }
  return dest - orig;
}

typedef char v16qi __attribute__((vector_size(16)));
typedef union
{
  v16qi v;
  unsigned char e[16];
}  vec16;

size_t
rle_vec(size_t size, char* src, char* dest)
{
  const char* orig = dest;
  size_t i = 0;
  vec16 v;
  const size_t s = size / 16;
  for(i = 0; i < s; ++i)
  {
    size_t j = 0;
    for(j = 0; j < 16; ++j)
    {
      v.e[j] = src[j];
    }
    src += 16;
    
    for( j = 0; j < 16; ++j)
    {
      unsigned char run_length = 1;      
      while (j+1 < 16 && v.e[j] == v.e[j+1])
      {
        ++run_length;
        ++j;
      }
      *dest++ = run_length;
      *dest++ = v.e[j];
    }
    
  }
  return dest - orig;
}

size_t
unrle(size_t size, char* src, char* dest)
{
  const char* orig = src;
  while ((src - orig) < size)
  {
    const unsigned char run_length = *src++;
    size_t j = 0;
    for(j = 0; j < run_length; ++j )
    {
      *dest++ = *src;
    }
    ++src;
  }
  return src - orig;
}

int
main(int argc, char** argv)
{
  size_t i = 0;
  char buffer[2048*1024];
  char tab[1024];
  // char res[1024];

  for(i = 0; i < 32; ++i)
  {
    tab[i] = 42;
  }

  for(i = 0; i < 199999999; ++i)
  {
    size_t rle_size = rle(32,tab,buffer);
  }
  // unrle( rle_size, buffer, res);
  
  // for(i = 0; i < 64; ++i)
  // {
  //   printf("%hhu | ", buffer[i]);
  // }
  // printf("\n");

  // for(i = 0; i < 1024; ++i)
  // {
  //   printf("%d | ", res[i]);
  // }
  // printf("\n");

  return 0;
}