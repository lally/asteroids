// Nick Brett - 15-02-05
//
// Passive.cxx
//
// This class is largly an instanciable version of Item.
// Background objects that won't have any effect on the
// game world, such as stars etc should be instances
// of Passive

#include "passive.h"

//<-- class passive -->
passive::passive( const vec2d& Position ):
  item(Position)
{}

passive::passive( const vec2d& Position,const vec2d& Velocity ):
  item(Position,Velocity)
{}

passive::~passive()
{}

//<-- class star -->
star::star( const vec2d& Position, const float Size ) : 
  passive( Position ),
     m_size(Size)
{}

star::~star()
{}

void star::draw()
{
  graphics::drawPoint( this->position(),m_size,0xFF,0xFF,0xFF );
 
  return;
}

void star::draw( const vec2d& Location )
{
  graphics::drawPoint( Location,m_size,0xFF,0xFF,0xFF );
 
  return;
}

