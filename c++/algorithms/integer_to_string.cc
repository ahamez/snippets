#include <iostream>
#include <string>
#include <deque>
#include <cmath>

typedef std::deque<unsigned char> deq_type;

std::string int2str(int x);
char* int10_to_str(long int val,char *dst,int radix);

char*
int10_to_str(long int val,char *dst,int radix)
{
  char buffer[65];
  register char *p;
  long int new_val;
  
  if (radix < 0)				/* -10 */
  {
    if (val < 0)
    {
      *dst++ = '-';
      val = -val;
    }
  }
  
  p = &buffer[sizeof(buffer)-1];
  *p = '\0';
  new_val= (long) ((unsigned long int) val / 10);
  *--p = '0'+ (char) ((unsigned long int) val - (unsigned long) new_val * 10);
  val = new_val;
  
  while (val != 0)
  {
    new_val=val/10;
    *--p = '0' + (char) (val-new_val*10);
    val= new_val;
  }
  while ((*dst++ = *p++) != 0) ;
  return dst-1;
}


std::string
int2str(int x)
{
  deq_type d;
  bool neg = false;
  
  if (x < 0)
  {
    neg = true;
    x = -x;
  }

  if (x == 0)
  {
    d.push_front('0');
  }
  else
  {
    while(x != 0)
    {
      const int rem = x % 10;
      x /= 10;
      d.push_front(rem + '0');
    }
  }

  if (neg)
  {
    d.push_front('-');
  }
  
  return std::string(d.begin(), d.end());
}

int main (int argc, const char * argv[])
{
  std::cout << int2str(0) << std::endl;
  std::cout << int2str(-1) << std::endl;
  std::cout << int2str(123) << std::endl;
  std::cout << int2str(1020) << std::endl;
  std::cout << int2str(-123456) << std::endl;
  std::cout << int2str(123456) << std::endl;
  
  return 0;
}