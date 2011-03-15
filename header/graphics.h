#ifndef GRAPHIC_CLASS
#define GRAPHIC_CLASS

// Copyright Nick Brett 2007
// contact nickdbrett@googlemail.com

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>

#include <SDL.h>
#include <SDL_opengl.h>

#include "common.h"
#include "vec2d.h"
#include "text.h"
#include "physics.h"

/**
 * Graphics namespace
 *
 * A set of code for drawing game elements and bitmap text on the
 * screen. There are methods for drawing primatives, physics::clip
 * objects and strings (using font objects provided as argument) as
 * well as a drawable pABC to define an interface for any object which
 * can be drawn on screen.
 */
namespace graphics
{
  
  /**
   * Represents the Display window
   *
   * There is only one screen, singleton DP.
   */
  class display
    {
    public:
      ~display();

      static display* create();

      void initialise() throw( exception );
      void kill();
      void update();
      void resize();
      
      /** Returns a vector holding the current width and height of the screen */
      const vec2d& dimension() const
	{
	  return m_dimension;
	}
      
      /** Returns a vector holding the center of the screen */
      const vec2d center() const
	{
	  return m_dimension * 0.5;
	}
  
      void lock();
      void unlock();
      
    private:
      display();
      static display* m_ptrToSelf;

      /** pointer to SDL surface */      
      SDL_Surface* m_screen;

      /** width and height of screen */
      vec2d        m_dimension; 
    };

  class drawable
    {
    public:
      virtual ~drawable();

      virtual void draw()=0;
	
      /** Draw Object on screen */
      virtual void draw( const vec2d& )=0;

    };
      
  void drawPixel( const vec2d& centre, int colour);
  void drawLine( const vec2d& start, const vec2d& end, const int colour);
  void drawCircle(const vec2d& centre, const float radius, const int colour);
  void drawTriangle(const vec2d& centre, const vec2d& orientation, const float radius, const int colour);
  void drawSquare(const vec2d& centre, const float radius, const int colour);
  void drawPolygon(const vec2d& centre, const float radius, const int numberOfSides, const int colour);
  void drawPixel( const int, const int, const Uint8, const Uint8, const Uint8);
  void drawPoint( const vec2d&, const float, const Uint8, const Uint8, const Uint8 );

  void drawChar( const char, font&, const vec2d& );
  void drawString( const std::string&, font&, const vec2d& );

  void draw( const physics::clip& );
  void draw( const physics::clip&, const vec2d& );
}

#endif
