#include "vec2d.h"

vec2d::vec2d():
  m_x(0.0),
     m_y(0.0)
{}

vec2d::vec2d( const float X, const float Y ):
  m_x(X),
     m_y(Y)
{}

vec2d::vec2d( const vec2d& Arg ):
     m_x( Arg.x() ),
  m_y( Arg.y() )

{}

vec2d::~vec2d()
{}
	
const vec2d& vec2d::polar( const float Angle, const float R )
{
  m_x = R*sin(Angle);
  m_y = R*cos(Angle);

  return *this;
}
	
const vec2d& vec2d::operator=(const vec2d& rhs)
{
  m_x=rhs.m_x;
  m_y=rhs.m_y;
	
  return *this;
}
		 
const vec2d vec2d::operator+(const vec2d& Rhs) const
{
  return vec2d(m_x+Rhs.m_x, m_y+Rhs.m_y);
}
	
const vec2d vec2d::operator-(const vec2d& rhs) const
{
  return vec2d(m_x-rhs.m_x,m_y-rhs.m_y);
}

const vec2d vec2d::operator*(const float rhs) const
{
  return vec2d(m_x*rhs, m_y*rhs);
}

const vec2d vec2d::operator/(const float rhs) const
{
  return vec2d(m_x/rhs, m_y/rhs);
}
	
const vec2d vec2d::cross(const vec2d& rhs) const
{
  return vec2d(m_x*rhs.m_y, -m_y*rhs.m_x);
}
	
const float vec2d::magnitude() const
{
  return sqrt(m_x*m_x + m_y*m_y);
}
	
const float vec2d::magSqrd() const
{
  return (m_x*m_x + m_y*m_y);
}
	
const vec2d& vec2d::rotate(const float Angle)
{
  float x = m_x;	
	
  m_x = x*(cos(Angle)) - m_y*(sin(Angle));
  m_y = x*(sin(Angle)) + m_y*(cos(Angle));		

  return *this;
}
	
const vec2d& vec2d::rotate( const float Angle, const vec2d& Center )
{
  vec2d tmp( *this - Center );
  tmp.rotate( Angle );
  *this = tmp + Center;

  return *this;
}
	
const bool vec2d::operator!=(const vec2d& Rhs) const
{
  return (m_x != Rhs.m_x) || (m_y != Rhs.m_y);
}
	
const bool vec2d::operator==(const vec2d& Rhs) const
{
  return (m_x == Rhs.m_x) && (m_y == Rhs.m_y);
}

	
const vec2d& vec2d::operator+=( const vec2d& Rhs)
{
  this->x() += Rhs.x();
  this->y() += Rhs.y();

  return *this;
}
	
const vec2d& vec2d::operator-=( const vec2d& Rhs )
{
  this->x() -= Rhs.x();
  this->y() -= Rhs.y();

  return *this;;
}

const vec2d& vec2d::operator*=( const float Arg )
{
  this->x() *= Arg;
  this->y() *= Arg;

  return *this;
}

const vec2d& vec2d::operator/=( const float Arg )
{
  this->x() /= Arg;
  this->y() /= Arg;

  return *this;
}

const vec2d& vec2d::invertX()
{
  this->x() =  -this->x();

  return *this;
}

const vec2d& vec2d::invertY()
{
  this->y() =  -this->y();

  return *this;
}
	
const vec2d normalise( const vec2d& Arg )
{
  return Arg / Arg.magnitude();
}

const float dot( const vec2d& Lhs, const vec2d& Rhs )
{
  return (Lhs.x()*Rhs.x()) + (Lhs.y()*Rhs.y());
}
	
const vec2d operator-( const vec2d& Arg )
{
  return vec2d( -Arg.x(), -Arg.y() );
}

const vec2d perpendicular( const vec2d& Arg )
{
  return vec2d( -Arg.y(), Arg.x() );
}

const float angle( const vec2d& A,const vec2d& B )
{
  return acos( dot(A,B)/( sqrt( A.magSqrd()*B.magSqrd() ) ) );
}
	
ostream& operator<<( ostream& Out, const vec2d& Vector)
{
  Out << "("
      << Vector.x()
      << ","
      << Vector.y()
      << ")";
		
  return Out;
}
