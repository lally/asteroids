// Nick Brett - 15-02-05
//
// Graphic.cxx
//
// An ABC to define the interface between a graphical object
// (Something which might get drawn on the screen) and
// whatever mechanism is used to do the drawing. The Drawing
// methods will be implimented in this class.

#include "graphics.h"
#include "SDL_net.h"

namespace graphics
{

  display* display::m_ptrToSelf = NULL;
  
  display::display():
    m_screen(),
    m_dimension(512,512)
    {} 

  display::~display()
    {}
  
  display* display::create()
    {
      if( m_ptrToSelf == NULL )
	{
	  m_ptrToSelf = new display();
	}
      
      return m_ptrToSelf;
    }


  void display::initialise() throw( exception )
    {
      if( -1 == SDL_Init(SDL_INIT_EVERYTHING))
	{
	  std::stringstream err;
	  err << "failed to initialise SDL with error: " << SDL_GetError();
	  throw( exception(err.str()) );
	}

      if(SDLNet_Init()==-1) {
	  std::stringstream err;
	  err << "failed to initialise SDL_Net with error: " << SDLNet_GetError();
	  throw( exception(err.str()) );
      }

      atexit(SDL_Quit);
	
      SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8);
      SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8);
      SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8);
      SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);

      m_screen = SDL_SetVideoMode( static_cast<int>(this->dimension().x()),
				   static_cast<int>(this->dimension().y())
				   ,0,SDL_OPENGL );
	
      if( m_screen == NULL )
	{
	  std::stringstream err;
	
	  err << "failed to set " 
	      <<  this->dimension().x() 
	      << "x" 
	      << this->dimension().y() 
	      << "video with error: "
	      << SDL_GetError();

	  throw( exception( err.str() ) );
	}
	
      glViewport(0, 0, m_screen->w, m_screen->h);
      glOrtho(0.0, m_screen->w, m_screen->h, 0.0, -1.0, 1.0);
      glClearColor(0.0, 0.0, 0.0, 0.0);
	
      SDL_WM_SetCaption("Asteroids", "Asteroids");
	
      return;
    }

  void display::kill()
    {
      return;
    }

  void display::update()
    {
      SDL_GL_SwapBuffers();
      glClear(GL_COLOR_BUFFER_BIT);
	
      return;
    }

  void display::lock()
    {
      if(SDL_MUSTLOCK(m_screen))
	{
	  SDL_LockSurface(m_screen);
	}

      return;
    }

  void display::unlock()
    {
      if(SDL_MUSTLOCK(m_screen))
	{
	  SDL_UnlockSurface(m_screen);
	}

      return;
    }


  // <-- class drawable -->
  drawable::~drawable()
    {}

  void drawLine( const vec2d& a, const vec2d& b, const int colour )
    {
      float R;
      float G;
      float B;

      switch(colour)
	{
	case 1:
	  R=1.0;
	  G=B=0.0;
	  break;
		
	case 2:
	  G=1.0;
	  R=B=0.0;
	  break;
		
	case 3:
	  B=1.0;
	  R=G=0.0;
	  break;
		
	default:
	  R=G=B=1.0;
	  break;
	}
	
      glColor3f(R,G,B);

      glBegin(GL_LINES);
        glVertex2f(a.x(), a.y());
        glVertex2f(b.x(), b.y());
      glEnd();

      return;
    }

  void drawCircle( const vec2d& centre, float radius, int colour )
    {
      int   a(0);
      float b(1),
	radius_sqrd = radius*radius;
	
      glColor3f(0.0,255.0,0.0);
      glBegin(GL_POINTS);
	
      while(a<b)
	{	
		
	  b = sqrt(radius_sqrd - a*a);
	  b = (b - int(b) > 0.5)? int(b)+1 : int(b);
		
	  glVertex2f( int(centre.x() +a), int(centre.y() +b) );
	  glVertex2f( int(centre.x() - a), int(centre.y() +b) );
	  glVertex2f( int(centre.x() +a), int(centre.y() - b) );
	  glVertex2f( int(centre.x() - a), int(centre.y() - b) );
		   
	  glVertex2f( int(centre.x()+ b), int(centre.y() +a) );
	  glVertex2f( int(centre.x() - b), int(centre.y() +a) );
	  glVertex2f( int(centre.x() +b), int(centre.y() - a) );
	  glVertex2f( int(centre.x() - b), int(centre.y() - a) );
		
	  a++;
	}
	
      glEnd();
	
      return;
    }

  void drawTriangle(const vec2d& centre, const vec2d& orientation, float radius, int colour)
    {
      vec2d displacement( orientation * radius );

      float theta = (2.0*M_PI)/3.0;
	
      vec2d a( centre + displacement );
      displacement.rotate( theta );
      vec2d b( centre + (displacement * 0.8) );
      displacement.rotate( theta );
      vec2d c( centre + (displacement * 0.8) );

      glColor3f(0.0,0.5,0.0);
      glBegin(GL_TRIANGLES);
      glVertex2f(a.x(),a.y());
      glVertex2f(b.x(),b.y());
      glVertex2f(c.x(),c.y());
      glEnd();
	
      glColor3f(0.0,1.0,0.0);
      glBegin(GL_LINES);
      glVertex2f(a.x(),a.y());
      glVertex2f(b.x(),b.y());
      glVertex2f(b.x(),b.y());
      glVertex2f(c.x(),c.y());
      glVertex2f(c.x(),c.y());
      glVertex2f(a.x(),a.y());
      glEnd();
	
      return;
    }

  void drawPixel( const int x, const int y, const Uint8 R, const Uint8 G, const Uint8 B)
    {
      glPointSize( 2.0 );
      glColor3f(R,G,B);
      glBegin(GL_POINTS);
      glVertex2i(x,y);
      glEnd();

      return;
    }

  void drawPoint( const vec2d& Location, const float Size, const Uint8 R, const Uint8 G, const Uint8 B)
    {
      glPointSize( Size );
      glColor3f(R,G,B);

      glBegin(GL_POINTS);
        glVertex2f( Location.x(),Location.y() );
      glEnd();

      return;
    }


  void draw( const physics::clip& Arg )
    {
      glColor4f(0.3,0.3,0.3,0.1);

      physics::clip::const_iterator itr( Arg.begin() );
      physics::clip::const_iterator end( Arg.end() );

      vec2d center( Arg.center() );

      glBegin(GL_TRIANGLE_FAN);
      
      glVertex2f( center.x(),center.y() );

      for(; itr!=end;++itr )
	{
	  glVertex2f( itr->x(),itr->y() );
	}

      glVertex2f( Arg.at(0).x(),Arg.at(0).y() );

      glEnd();

      glColor3f(1.0,1.0,1.0);
      itr = Arg.begin();

      glBegin(GL_LINE_LOOP);
      
      for(; itr!=end;++itr )
	{
	  glVertex2f( itr->x(),itr->y() );
	}

      glEnd();

     
      return;
    }
  
  void draw( const physics::clip& Arg, const vec2d& Location )
  {
      glColor4f(0.3,0.3,0.3,0.1);

      physics::clip::const_iterator itr( Arg.begin() );
      physics::clip::const_iterator end( Arg.end() );

      glBegin(GL_POLYGON);

      vec2d center( Arg.center() + Location );

      glBegin(GL_TRIANGLE_FAN);
      
      glVertex2f( center.x(),center.y() );

      for(; itr!=end;++itr )
	{
	  glVertex2f( itr->x() + Location.x(),itr->y() + Location.y() );
	}

      glVertex2f( Arg.at(0).x() + Location.x(),Arg.at(0).y() + Location.y() );
      
      glEnd();

      glColor3f(1.0,1.0,1.0);
      itr = Arg.begin();

      glBegin(GL_LINE_LOOP);
      
      for(; itr!=end;++itr )
	{
	  glVertex2f( itr->x() + Location.x(),itr->y() + Location.y() );
	}

      glEnd();

      return;
  }

  void drawChar( const char Character, font& Font, const vec2d& Position )
    {
      if( Character == ' ' ) return;

      glPushMatrix();
      //      glLoadIdentity();
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1 );
      glColor3f(1.0,1.0,1.0);
      
      glRasterPos2d( Position.x(),Position.y() );
      glBitmap( Font.width(),Font.height(), 0.0,0.0,0.0,0.0,Font(Character) );

      glPopMatrix();

      return;
    }
  
  void drawString( const std::string& String, font& Font, const vec2d& Position )
    {
      vec2d position(Position);

      std::string::const_iterator itr( String.begin() );
      std::string::const_iterator end( String.end() );

      for(; itr!=end;++itr )
	{
	  drawChar( *itr,Font,position );
	  position.x() += Font.width();
	}
      
    }

}
