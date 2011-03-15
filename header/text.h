#ifndef TEXT_CLASSES
#define TEXT_CLASSES

#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <SDL.h>
#include <SDL_opengl.h>

#include <boost/shared_array.hpp>
#include <boost/lexical_cast.hpp>

#include "common.h"

/**
 * A set of methods and classes for displaying 2D text on screen in
 * OpenGL.
 *
 */

class font
{
  typedef boost::shared_array<GLubyte> ptr;
  typedef std::map<char,ptr>::iterator iterator;

 public:
  font( const std::string&, const size_t, const size_t ) throw( exception );
  font( const font& );
  ~font();

  const font& operator=( const font& );

  const GLubyte* operator()( const char );
  
  const size_t width() const
    {
      return m_width;
    }

  const size_t height() const
    {
      return m_height;
    }

 private:
  size_t m_width;
  size_t m_height;
  std::map<char,ptr> m_font;
};

const size_t hexFromChar( const char ) throw( exception );

const size_t hexFromString( const std::string& ) throw( exception );

const GLubyte flipByteOrder( const GLubyte );

#endif
