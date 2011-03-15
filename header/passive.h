#ifndef PASSIVE_CLASS
#define PASSIVE_CLASS

// Copyright Nick Brett 2007
// contact nickdbrett@googlemail.com

#include <boost/shared_ptr.hpp>

#include "vec2d.h"
#include "item.h"

/**
 * Passive
 * 
 * Item with no physical presence, is background objects, smoke trails
 * etc.
 */
class passive : public item
{
 public:
  typedef boost::shared_ptr<passive> ptr;

  passive( const vec2d& );
  passive( const vec2d&,const vec2d& );
  virtual ~passive();
};

/**
 * Star
 *
 * Background object represented by a white dot.
 */
class star : public passive
{
 public:
  star( const vec2d&, const float = 1.0 );
  virtual ~star();

  virtual void draw();
  virtual void draw( const vec2d& );

 private:
  float m_size;
};

#endif // PASSIVE_CLASS
