// Nick Brett - 06-03-05
//
// Active.cxx
//
// An Item which interacts with other items in the
// game world. "Active" items must be able to collide,
// move about etc.

#include "active.h"

//<-- active class -->
active::active( const vec2d& Position ):
  item(Position)
{}

active::active( const vec2d& Position,const vec2d& Velocity ):
  item( Position,Velocity )
{}

active::active( const active& Arg ):
  item(Arg)
{}

active::~active()
{}

const physics::collision collide( active* A, active* B )
{
  physics::collision result;
 
  // objects donot collide with themselves
  if( A == B )
    {
      return result;
    }

 
  // test radi - are combined radi smaller than separation between
  // bodies ? if so no collision occurs, return.
  if( (A->radiusSqrd() + B->radiusSqrd()) < (A->position() - B->position()).magSqrd() )
    {
      return result;
    }
  
  particle* particleA( dynamic_cast<particle*>(A) );
  particle* particleB( dynamic_cast<particle*>(B) );

  if( (particleA != NULL) && (particleB != NULL) )
    {
      // particle's don't collide atm, do nothing
    }
  else if( particleA != NULL )
    {
      result = collideWithParticle( dynamic_cast<shape*>(B),particleA );
    }
  else if( particleB != NULL )
    {
      result = collideWithParticle( dynamic_cast<shape*>(A),particleB );      
    }
  else
    {
      result = collideWithShape( dynamic_cast<shape*>(A),dynamic_cast<shape*>(B) );
    }

  return result;
}

void resolveCollision( active::ptr A,active::ptr B,const vec2d& Location )
{
  A->destroy();
  B->destroy();
  
  return;
}


//<-- shape class -->
shape::shape( const vec2d& Position, const physics::clip& Clip ): 
  active(Position),
     m_clip(Clip)
{}

shape::shape(const vec2d& Position,const vec2d& Velocity,
	       const physics::clip& Clip ): 
  active( Position,Velocity ),
     m_clip(Clip)
{}

shape::shape( const shape& Arg ):
  active( Arg ),
     m_clip(Arg.box())
{}

shape::~shape()
{
  try
    {}
  catch(...)
    {}
}	

const shape& shape::operator=( const shape& Arg )
{
  (*this).active::operator=(Arg);

  this->m_clip = Arg.box();

  return *this;
}

const float shape::radiusSqrd() const
{
  return m_clip.radiusSqrd();
}

void shape::draw()
{
  physics::transform( this->box(), this->angle(), this->position() );

  graphics::draw( this->box() );

  this->box().reset();

  return;
}

void shape::draw( const vec2d& Position )
{
  physics::rotate( this->box(), this->angle() );

  graphics::draw( this->box(),Position );

  this->box().reset();

  return;
}


const physics::collision collideWithShape( shape* A, shape* B )
{
  // test clip boxes
  transform( A->box(), A->angle(), A->position() );
  transform( B->box(), B->angle(), B->position() );  

  physics::collision Collision( physics::collide( A->box(),B->box() ) );

  A->box().reset();
  B->box().reset();

  return Collision;
}
	
void resolveCollisionWithShape( shape* A, shape* B, const vec2d& Location )
{
  A->destroy();
  B->destroy();

  return;
}

// <-- particle class -->
particle::particle( const vec2d& Position, const float Radius ):
  active(Position),
     m_radius(Radius)
{}

particle::particle( const vec2d& Position,const vec2d& Velocity,const float Radius ):
  active(Position,Velocity),
     m_radius(Radius)
{}

particle::particle( const particle& Arg ):
  active(Arg),
     m_radius( Arg.radius() )
{}

particle::~particle()
{}

const particle& particle::operator=( const particle& Arg )
{
  (*this).active::operator=(Arg);
  
  this->m_radius = Arg.radius();

  return *this;
}

const float particle::radiusSqrd() const
{
  return m_radius*m_radius;
}

void particle::draw()
{
  graphics::drawPoint( this->position(), m_radius, 0xff, 0xff, 0xff );

  return;
}

void particle::draw( const vec2d& Position )
{
  graphics::drawPoint( Position, m_radius, 0xff, 0xff, 0xff );

  return;
}

const physics::collision collideWithParticle( shape* Shape, particle* Particle )
{
  physics::collision result;

  // test radi - are combined radi smaller than separation between
  // bodies ? if so no collision occurs, return.
  if( (Particle->radius()*Particle->radius() + Shape->box().radiusSqrd()) 
      < (Particle->position() - Shape->position()).magSqrd() )
    {
      return result;
    }

  // test clip boxes
  transform( Shape->box(), Shape->angle(), Shape->position() );  

  result = physics::collide( Particle->position(),Shape->box() );

  Shape->box().reset();

  return result;
}


void resolveCollisionWithParticle( shape* Shape, particle* Particle, const vec2d& Location )
{
  Particle->destroy();
  Shape->destroy();

  return;
}
