// Nick Brett - 15-02-05
//
// Item.cxx
//
// A pABC to define the interface and data members required 
// by an "Item". An Item is any object that may exist in
// the game world, such as background objects, space ships,
// deep space vacume hard cows etc ...

#include "item.h"

item::item( const vec2d& Position ):
  graphics::drawable(),
  m_destroyed(false),
  m_position(Position),
  m_velocity(),
  m_rotation(0.0),
  m_orientation(0,-1.0),
  m_angle(M_PI)
{}

item::item( const vec2d& Position, 
	    const vec2d& Velocity ):
  graphics::drawable(),
  m_destroyed(false),
  m_position(Position),
  m_velocity(Velocity),
  m_rotation(0.0),
  m_orientation(0,-1.0),
  m_angle(M_PI)
{}

item::item( const item& Arg ):
  graphics::drawable(),
  m_destroyed(Arg.destroyed()),
  m_position(Arg.position()),
  m_velocity(Arg.velocity()),
  m_rotation(Arg.rotation()),
  m_orientation(Arg.orientation()),
  m_angle(Arg.angle())
{}

item::~item()
{}

const item& item::operator=( const item& Arg )
{
  this->m_destroyed   = Arg.destroyed();
  this->m_position    = Arg.position();
  this->m_velocity    = Arg.velocity();
  this->m_rotation    = Arg.rotation();
  this->m_orientation = Arg.orientation();
  this->m_angle       = Arg.angle();

  return *this;
}

void item::rotate( const float Angle )
{
  m_orientation.rotate(Angle);
  m_angle += Angle;

  if( m_angle > 2.0*M_PI)
    {
      m_angle -= 2.0*M_PI;
    }
	
  return;
}

void item::translate( const vec2d& Arg )
{
  m_position += Arg;

  return;
}


void item::destroy()
{
  m_destroyed = true;

  return;
}

const bool item::destroyed() const
{
  return m_destroyed;
}
