// Vector.h -> 2D vector header
//         		 -> Nick Brett 08-06-03
//
// Layout heavly modified on 13-02-05 but implimentation of most functions remains unchanged

#ifndef VEC2D_CLASS
#define VEC2D_CLASS

#include <cmath>
#include <iostream>
#include <ostream>
#include <fstream>

using std::ostream;
using std::cout;
using std::endl;

class vec2d
{
 public: 	
  vec2d();
  vec2d( const float, const float );
  vec2d( const vec2d& arg );
  ~vec2d();

  const vec2d& polar( const float, const float );

  const vec2d& set( const float X, const float Y )
    {
      m_x = X;
      m_y = Y;

      return *this;
    }
	
  void x( const float X )
    {
      m_x = X;

      return;
    }
	
  void y( const float Y )
    {
      m_y = Y;

      return;
    }

  const float x() const
    {
      return m_x;
    }

  const float y() const
    {
      return m_y;
    }

  float& x()
    {
      return m_x;
    }

  float& y()
    {
      return m_y;
    }

  const vec2d& operator=( const vec2d& );
  const vec2d& operator+=( const vec2d& );
  const vec2d& operator-=( const vec2d& );
  const vec2d& operator*=( const float );
  const vec2d& operator/=( const float );

  const vec2d  operator+( const vec2d& ) const;
  const vec2d  operator-( const vec2d& ) const;
  const vec2d  operator*( const float ) const;
  const vec2d  operator/( const float ) const;

  const vec2d cross( const vec2d& ) const;
  const float magnitude() const;
  const float magSqrd() const;
	
  /** Assumes right handed rotation */
  const vec2d& rotate( const float );

  /** Assumes right handed rotation */
  const vec2d& rotate( const float, const vec2d& );
	
  const bool operator!=( const vec2d& ) const;
  const bool operator==( const vec2d& ) const;

  /** inverts the sign of the arguments X component. Returns a reference
      to the arg.*/
  const vec2d& invertX();

  /** inverts the sign of the arguments Y component. Returns a reference
      to the arg.*/
  const vec2d& invertY();

	
 private:
  float m_x;
  float m_y;
};


/** 
 * Returns a copy of the argument with the sign of all elements of the
 * vector inverted
 */
const vec2d operator-( const vec2d& );

/** returns a vector with the same direction as the arg but with unit
    length */
const vec2d normalise( const vec2d& );

/** returns the dot product of the two vectors */
const float dot( const vec2d&, const vec2d& );

/** returns the arguement rotated through pi/2 (right handed)*/
const vec2d perpendicular( const vec2d& );

/** finds the angle between two vectors */
const float angle( const vec2d&,const vec2d& );

ostream& operator<<(ostream& out, const vec2d& vector);

#endif // VEC2D_CLASS
