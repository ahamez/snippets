#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

//return std::equal(s.begin(), s.end(), s.rbegin());

///////////////////////////////////////////////////////////////////////////
bool is_palindrome(const std::string&);

bool
is_palindrome(const std::string& s)
{
  const std::size_t size = s.size();

  for (std::size_t i = 0, j = size - 1; i < size/2; ++i,--j)
  {
    if(s[i] != s[j]) // ou if(s[i] != s[size - 1 - i])
    {
      return false;
    }
  }
  
  return true;
}

///////////////////////////////////////////////////////////////////////////
bool is_palindrome2(std::string::const_iterator, std::string::const_iterator);

bool
is_palindrome2(std::string::const_iterator first, std::string::const_iterator last)
{
  const std::size_t middle = std::distance(first, last - 1) / 2;
  std::string::const_iterator cit = first;
  std::string::const_iterator rcit = last - 1;
  for (std::size_t i = 0; i < middle; ++i, ++cit, --rcit)
  {
    if (*cit != *rcit)
    {
      return false;
    }
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////
bool is_palindrome_rec(const std::string&);

bool
is_palindrome_rec(const std::string& s)
{
  const std::size_t size = s.size();
  if (size <= 1)
  {
    return true;
  }
  if (s[0] != s[size - 1])
  {
    return false;
  }
  return is_palindrome_rec(s.substr(1, size - 2));
}

///////////////////////////////////////////////////////////////////////////
std::string find_longest_palindrom(const std::string&);

std::string
find_longest_palindrom(const std::string& s)
{
//  std::cout << "find largest palindrom in " << s << std::endl << std::endl;
  const std::size_t size = s.size();

  std::string::const_iterator first = s.begin();
  std::string::const_iterator last;
  
  for (std::size_t i = 0; i < size; ++i)
  {
    // j = 1 => odd | j = 2 => even
    for( std::size_t j = 1; j <=2; ++j)
    {
      std::string::const_iterator tmp_first = first + i;
      std::string::const_iterator tmp_last = tmp_first + j;
      while (true)
      {
        if (tmp_first >= s.begin() and tmp_last <= s.end())
        {
          if (not is_palindrome2(tmp_first, tmp_last))
          {
            break;
          }
          
          if (std::distance(tmp_first, tmp_last) > std::distance(first, last))
          {
            first = tmp_first;
            last = tmp_last;
          }
          --tmp_first;
          ++tmp_last;
        }
        else
        {
          break;
        }
      }
    }
  }
  
  return std::string(first, last);
}

///////////////////////////////////////////////////////////////////////////
std::string find_longest_palindrom2(const std::string&);

std::string
find_longest_palindrom2(const std::string& s)
{
  //  std::cout << "find largest palindrom in " << s << std::endl << std::endl;
  const std::size_t size = s.size();  
  const std::size_t nb_centers = 2 * size + 1;
  std::vector<std::size_t> lengths;
  
  for (std::size_t i = 0; i < nb_centers; ++i)
  {
    std::string::const_iterator first = s.begin() + i/2;
    std::string::const_iterator last = first + i % 2;
    while (first > s.begin() and last <= s.end() and *(first-1) == *last)
    {
      --first;
      ++last;
    }
    
    lengths.push_back(last - first);
  }
  
  const auto cit = std::max_element(lengths.begin(), lengths.end());
  const std::size_t pos = cit - lengths.begin();
  return std::string(s.begin() + pos/2 - *cit/2, s.begin() + pos/2 + *cit/2 + pos % 2);
}


///////////////////////////////////////////////////////////////////////////
int
main ()
{
//  std::cout << std::boolalpha << is_palindrome("A") << std::endl;
//  std::cout << std::boolalpha << is_palindrome("AA") << std::endl;
//  std::cout << std::boolalpha << is_palindrome("ABA") << std::endl;
//  std::cout << std::boolalpha << is_palindrome("ABBA") << std::endl;
//  std::cout << std::boolalpha << is_palindrome("AABBA") << std::endl;
//  std::cout << std::endl;
//  std::cout << std::boolalpha << is_palindrome_rec("A") << std::endl;
//  std::cout << std::boolalpha << is_palindrome_rec("AA") << std::endl;
//  std::cout << std::boolalpha << is_palindrome_rec("ABA") << std::endl;
//  std::cout << std::boolalpha << is_palindrome_rec("ABBA") << std::endl;
//  std::cout << std::boolalpha << is_palindrome_rec("AABBA") << std::endl;

  std::cout << find_longest_palindrom("AAAABAABBAAABBA") << std::endl;
  std::cout << find_longest_palindrom2("AZAA") << std::endl;
  return 0;
}