#ifndef ITEM_CLASS
#define ITEM_CLASS

// Copyright Nick Brett 2007
// contact nickdbrett@googlemail.com

#include "vec2d.h"
#include "graphics.h"

/**
 * Item
 *
 * A base class to define the interface and data members required by an
 * "Item". An Item is any object that may exist in the game world, such
 * as background objects, space ships, deep space vacume hard cows etc
 * ...
 */
class item : public graphics::drawable
{
 public:
  item( const vec2d& );
  item( const vec2d&,const vec2d& );
  item( const item& );
  virtual ~item();

  const item& operator=( const item& );

  /** Rotates the Item */
  virtual void rotate( const float );
  virtual void translate( const vec2d& );

  /** Accelerates the Item by changing its velocity */
  void accelerate( const vec2d& Acceleration )
    {
      m_velocity += Acceleration;
      
      return;
    }

  const vec2d& position() const
    {
      return m_position;
    }

  vec2d& position()
    {
      return m_position;
    }

  const vec2d& velocity() const
    {
      return m_velocity;
    }
  
  vec2d& velocity()
    {
      return m_velocity;
    }

  const float rotation() const 
    {
     return m_rotation;
    }

  float& rotation()
    {
     return m_rotation;
    }

  const vec2d& orientation() const
    {
      return m_orientation;
    }
	
  vec2d& orientation()
    {
      return m_orientation;
    }

  const float angle() const 
    {
     return m_angle;
    }  

  /** Marks the Object for removal */
  virtual void destroy();										
  virtual const bool destroyed() const;
	
 private:

  /** true if the object has been marked for removal */
  bool m_destroyed;

  /** Location of the Item in the world */
  vec2d m_position;

  /** Items Velocity (Relative to the prefered frame ;-) ) */
  vec2d m_velocity;

  /** Rotational velocity */
  float m_rotation;

  /** Direction in which Item is facing (angle clockwise from vertical) */
  vec2d m_orientation;

  /** angle though which item has been rotated */
  float m_angle;

};
 
#endif // ITEM_CLASS
