#ifndef SHELL_CLASS
#define SHELL_CLASS

// Copyright Nick Brett 2007
// contact nickdbrett@googlemail.com
 
#include "active.h"
#include "physics.h" 

/**
 * Shell
 *
 * This class represents a simple bullet. The bullet has a maximum
 * range and destroys itself if that range is exceeded.
 */
class shell : public particle
{
 public:
  shell();
  shell( const vec2d&, const vec2d& );
  shell( const shell& );
  virtual ~shell();
	
  const shell& operator=( const shell& );

  virtual void update();
  virtual void destroy(); 
  
  virtual void draw();
  virtual void draw( const vec2d& );
	
 private:
  /** max distance shell can travel before destroying itself */
  float  m_range;
  /** distance shell has traveled */
  float  m_travel;

  /** time at which this was last updated */
  physics::time_t m_updateTime;
};

#endif // SHELL_CLASS
