#ifndef ACTIVE_CLASS
#define ACTIVE_CLASS

// Copyright Nick Brett 2007

#include <vector>
#include <boost/shared_ptr.hpp>

#include "vec2d.h"
#include "item.h"
#include "physics.h"

// class physics::clip;

/**
 * Active.h
 *
 * An Item which interacts with other items in the
 * game world. "Active" items must be able to collide,
 * move about etc.
 */
class active : public item
{
 public:
  typedef boost::shared_ptr<active> ptr;
  enum kind_t {kLOCAL, kREMOTE};
	
  active( const vec2d& );
  active( const vec2d&,const vec2d& );
  active( const active& );
  virtual ~active();

  /** Act on the data provided by user input, AI etc */
  virtual void update()=0;
  virtual const float radiusSqrd() const=0;

  // only some object types can be remote, and they'll re-implement
  // this method
  virtual kind_t kind() const { return kLOCAL; }
};

class shape : public active
{
 public:
  shape( const vec2d&,const physics::clip& );
  shape( const vec2d&,const vec2d&,const physics::clip& );
  shape( const shape& );
  virtual ~shape();

  const shape& operator=( const shape& );

  /** Act on the data provided by user input, AI etc */
  virtual void update()=0;
  virtual const float radiusSqrd() const;

  physics::clip& box()
  {
    return m_clip;
  }

  const physics::clip& box() const
  {
    return m_clip;
  }

  virtual void draw();
  virtual void draw( const vec2d& );

  const vec2d front() const
    {
      return this->position() + (this->orientation() * sqrt(this->box().radiusSqrd() * 1.1)  );
    }

 private:
  physics::clip m_clip;	
};

const physics::collision collide( active*, active* );
void resolveCollision( active::ptr,active::ptr,const vec2d& );

const physics::collision collideWithShape( shape*,shape* );
void resolveCollisionWithShape( shape*,shape*,const vec2d& );

class particle : public active
{
 public:
  typedef boost::shared_ptr<particle> ptr;
	
  particle( const vec2d&,const float );
  particle( const vec2d&,const vec2d&,const float );
  particle( const particle& );
  virtual ~particle();

  const particle& operator=( const particle& );

  /** Act on the data provided by user input, AI etc */
  virtual const float radiusSqrd() const;

  virtual void draw();
  virtual void draw( const vec2d& );

  const float radius() const
    {
      return m_radius;
    }

 private:
  float m_radius;
};

const physics::collision collideWithParticle( shape*,particle* );
void resolveCollisionWithParticle( shape*,particle*,const vec2d&);

#endif // ACTIVE_CLASS
