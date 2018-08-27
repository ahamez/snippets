#include <climits>

struct Bloom
{

  const unsigned int size;
  char*              content;
  const bool         own_content;


  Bloom( unsigned int size )
    : size(size)
    , content( new char[ size / CHAR_BIT ] )
    , own_content(true)
  {
  }

  Bloom( unsigned int size, char* content )
    : size(size)
    , content(content)
    , own_content(false)
  {
  }

  ~Bloom()
  {
    if( own_content )
    {
      delete[] content;
    }
  }

  void
  set_bit( unsigned int bit )
  {
    content[ bit / CHAR_BIT ] |= ( 1 << ( bit % CHAR_BIT ));
  }

  bool
  get_bit( unsigned int bit )
  const
  {
    return content[ bit / CHAR_BIT ] & ( 1 << ( bit % CHAR_BIT ));
  }

  bool
  insert( const std::size_t* hash )
  {
    for( unsigned int i = 0 ; i < 8 ; ++i )
    {
      if( not get_bit( hash[i] % size ) )
      {
        set(hash);
        return true;
      }
    }
    return false;
  }

  void
  set( const std::size_t* hash )
  {
    for( unsigned int j = 0 ; j < 8 ; ++j )
    {
      set_bit( hash[j] % (size));
    }
  }

  bool
  test( const std::size_t* hash )
  const
  {
    for( unsigned int i = 0 ; i < 8 ; ++i )
    {
      if( not get_bit( hash[i] % size ) )
      {
        return false;
      }
    }
    return true;
  }
};