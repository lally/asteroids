#ifndef SHIP_CLASS
#define SHIP_CLASS

// Copyright Nick Brett 2007
// contact nickdbrett@googlemail.com

#include "vec2d.h"
#include "active.h"
#include "input.h"
#include "ai.h"
#include "shell.h"
#include "elementManager.h"
#include "game.h"
#include "physics.h"

class ship;

/**
 * Weapon
 *
 * Represents a ships gun with a shell velocity and a fixed maximum
 * rate of fire.
 */
class weapon
{
 public:
  weapon();
  weapon( const float );
  weapon( const weapon& );
  ~weapon();
	
  const weapon& operator=( const weapon& );

  void fire( const shape* );
	
 private:
  float m_muzzel_velocity;
  /** time between fireing a shell - reloading time */
  physics::time_t m_period_of_fire;
  /** time after which the weapon will be able to fire again */
  physics::time_t m_time_of_next_fireing;
};

/**
 * Ship
 *
 * A class to represent a space ship in the game. Each instance is be
 * bound to a set of inputs (represented by the control object) and
 * responds acordingly
 */
class ship : public shape
{
 public:
  enum{FORWARD,BACKWARD,LEFT,RIGHT,FIRE};

  ship( const vec2d&,control* );
  ship( const vec2d&,const vec2d&,const vec2d&,control* );
  ship( const ship& );
  virtual ~ship();
	
  const ship& operator=( const ship& ); 

  virtual void update();
  virtual void draw();
  //  virtual void draw( const vec2d& );

  void setState(vec2d& pos, vec2d& vel, float rot);
	
  virtual control* control_pointer() const;
  virtual void destroy();

 private:
  control* m_control;

  float m_thrust;
  float m_rot;
  float m_mass;
	
  weapon* m_weapon_one;

  /** ship is invunrable for some short time after construction */
  physics::time_t m_invunrableTime;

  /** time at which this was last updated */
  physics::time_t m_updateTime;

  pthread_mutex_t m_mutex;
};

ship* insertPlayer();

/**
 * Rock
 *
 * An asteroid in the game world. Each asteriod has a randomly
 * derived, circular-ish shape and a size defined on contruction. When
 * destroyed, if the size of the asteriod is greater than 4, four new
 * smaller asteriods are added to the game world.
 *
 */
class rock : public shape
{
 public:
  rock( const vec2d&,const vec2d&,const size_t );
  rock( const rock& );
  virtual ~rock();
	
  const rock& operator=( const rock& );

  virtual void update();
  virtual void destroy();
	
  const size_t size() const
    {
      return m_size;
    }

 private:
  size_t m_size;

  /** time at which this was last updated */
  physics::time_t m_updateTime;

};

/**
 * Gun Turret
 *
 * Represents a gun turret which is unable to control its own velocity
 * but can rotate and fire.
 */
class turret : public shape
{
 public:
  enum{LEFT,RIGHT,FIRE};

  turret( const vec2d&,const vec2d&,control::ptr );
  turret( const turret& );
  ~turret();

  const turret& operator=( const turret& );

  virtual void update();
  virtual void destroy();

 private:
  control::ptr m_control;
  weapon*      m_weapon;
  float        m_rot;

  /** time at which this was last updated */
  physics::time_t m_updateTime;
};

std::vector<active::ptr>& generateRocks( const size_t, std::vector<active::ptr>& );
std::vector<active::ptr>& generateTurrets( const size_t, std::vector<active::ptr>& );

#endif // SHIP_CLASS
