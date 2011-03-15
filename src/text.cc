#include "text.h"

font::font( const std::string& File, const size_t Width, const size_t Height ) throw( exception ):
  m_width(Width),
     m_height(Height)
{
  std::fstream file( File.c_str(), std::ios::in );
 
  if( file.fail() )
    {
      std::stringstream err;

      err << __FILE__ << " "
	  << __LINE__ << " "
	  << __FUNCTION__ << " "
	  << "failed to open file " << File;

      throw( exception( err.str() ) );
    }
 
  size_t fileWidth(0);
  std::string data;

  std::vector<char> fontFormat;
  
  for( char format('A'); format<='Z'; ++format )
    {
      fontFormat.push_back( format );
    }

  for( char format('a'); format<='z'; ++format )
    {
      fontFormat.push_back( format );
    }

  for( char format('0'); format<='9'; ++format )
    {
      fontFormat.push_back( format );
    }

  fontFormat.push_back( ':' );
  fontFormat.push_back( '?' );
  fontFormat.push_back( '%' );
  fontFormat.push_back( '(' );
  fontFormat.push_back( ')' );
  fontFormat.push_back( '|' );

  file >> data;
  file >> data;
  file >> fileWidth;

  const size_t lineWidth( static_cast<size_t>(fileWidth * 0.125) );
  const size_t bytesPerCharacter( lineWidth * this->height() );
  ptr array;

  GLubyte lineByte[lineWidth];

  while( file.get() != '{' );

  std::vector<char>::const_iterator formatItr( fontFormat.begin() );
  std::vector<char>::const_iterator formatEnd( fontFormat.end() );

  for(; formatItr!=formatEnd;++formatItr )
    {
      m_font.insert( std::make_pair( *formatItr, ptr(new GLubyte[bytesPerCharacter]) ) );
      array = m_font[*formatItr];

      for( size_t i(0); (i<bytesPerCharacter) && (file.good()); )
	{
	  for( size_t iByte(0);iByte<lineWidth;++iByte )
	    {
	      file >> data;
	      lineByte[iByte] = hexFromString( data.substr(2,2) );
	    }
	  
	  for( size_t iByte(0);iByte<lineWidth; )
	    {
	      array[bytesPerCharacter - ++i] = flipByteOrder( lineByte[lineWidth - ++iByte] ); 
	    }
	}
    }

  file.close();
}

font::~font()
{
  try
    {}
  catch(...)
    {} 
}

font::font( const font& Arg ):
  m_width( Arg.width() ),
     m_height( Arg.height() ),
     m_font( Arg.m_font )
{}

const font& font::operator=( const font& Arg )
{
  this->m_width  = Arg.width();
  this->m_height = Arg.height();
  this->m_font   = Arg.m_font;

  return *this;
}

const GLubyte* font::operator()( const char Arg ) 
{
  iterator rtn( m_font.find(Arg) );

  if( rtn == m_font.end() )
    {
      rtn = m_font.find('?');
    }

  return rtn->second.get();
}


const size_t hexFromChar( const char Arg ) throw( exception )
{
  size_t rtn;

  if( (Arg >= '0') && (Arg <= '9') )
    {
      rtn = Arg - '0';
    }
  else if( (Arg >= 'a') && (Arg <= 'f') )
    {
      rtn =  Arg - 'a' + 10;
    }
  else
    {
      std::stringstream err;
      
      err << __FILE__ << " "
	  << __LINE__ << " "
	  << __FUNCTION__ << " "
	  << Arg << " does not represent a hex value";

      throw( exception( err.str() ) );
    }
  
  return rtn;
}

const size_t hexFromString( const std::string& Arg ) throw(exception)
{
  // convert hex digits to unsined int
  size_t rtn(0);
  size_t mul(1);
  
  std::string::const_reverse_iterator itr( Arg.rbegin() );
  std::string::const_reverse_iterator end( Arg.rend() );

  for(; itr != end; ++itr )
    {
      rtn += hexFromChar( *itr ) * mul;
      mul *= 16;
    }

  return rtn;
}

const GLubyte flipByteOrder( const GLubyte Arg )
{
  unsigned char maskArg( 0x01 );
  unsigned char maskRtn( 0x80 );
  GLubyte rtn(0);

  for( size_t i(0);i<8;++i, maskArg <<= 1, maskRtn >>= 1 )
    {
      if(Arg & maskArg) rtn |= maskRtn;
    }
  
  return rtn;
}
