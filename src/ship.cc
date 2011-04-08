// Nick Brett - 06-03-05
//
// PlayerCharacter.cxx
//
// An instanciable class to handle human
// controlled items in the game. Each
// instance will be bound to a set of inputs
// and respond acordingly

#include "ship.h"
#include "lock.h"
#include "util.h"
#include <pthread.h>
// <-- class weapon -->

weapon::weapon():
  m_muzzel_velocity(150.0),
  m_period_of_fire(0.5),
  m_time_of_next_fireing(0)
{}

weapon::weapon( const float FirePeriod ):
  m_muzzel_velocity(150.0),
  m_period_of_fire(FirePeriod),
  m_time_of_next_fireing(0)
{}

weapon::weapon( const weapon& Arg ):
  m_muzzel_velocity(Arg.m_muzzel_velocity),
  m_period_of_fire(Arg.m_period_of_fire),
  m_time_of_next_fireing(Arg.m_time_of_next_fireing)
{}

weapon::~weapon()
{}

const weapon& weapon::operator=( const weapon& Arg )
{
  this->m_muzzel_velocity      = Arg.m_muzzel_velocity;
  this->m_period_of_fire       = Arg.m_period_of_fire;
  this->m_time_of_next_fireing = Arg.m_time_of_next_fireing;

  return *this;
}
	
void weapon::fire( const shape* Parent )
{
  const physics::time_t now( physics::runTime::create()->now() );

  if( now > m_time_of_next_fireing )
    {
        // locally-created bullets go into the sync queue.
        active::ptr sh(new shell( Parent->front(),Parent->velocity() 
                                  + Parent->orientation()*m_muzzel_velocity));
        util::note_new_bullet(sh);
        elementManager::create()->insert(sh);
	
      m_time_of_next_fireing = now + m_period_of_fire;
    }

  return;
}
	
// <-- ship -->

ship::ship( const vec2d& Position, control* Control, active::kind_t k ):
  shape(Position,physics::shipClip(20.0) ),
  m_control( Control ),
  m_thrust( 0.2 ),
  m_rot( 3.6 ),
  m_mass( 100.0 ),
  m_weapon_one( new weapon() ),
  m_invunrableTime(1.0),
  m_updateTime( physics::runTime::create()->now() ),
  m_mutex(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP),
  m_kind(k)
{
}

ship::ship( const ship& Arg):
  shape( Arg ),
  m_control( Arg.m_control ),
  m_thrust( Arg.m_thrust ),
  m_rot( Arg.m_rot ),
  m_mass( Arg.m_mass ),
  m_weapon_one( new weapon( *Arg.m_weapon_one) ),
  m_invunrableTime( Arg.m_invunrableTime ),
  m_updateTime( Arg.m_updateTime ),
  m_mutex(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP),
  m_kind(Arg.m_kind)
{
}

void ship::setState(const vec2d& pos, const vec2d& vel,
		    const vec2d& orient,
		    const float angle) {
  Lock m(m_mutex);
  position() = pos;
  velocity() = vel;
  orientation() = orient;
  setAngle(angle);
    //  printf ("setState(orient=(%f, %f))\n", orient.x(), orient.y());
}
	

ship::~ship()
{
  pthread_mutex_destroy(&m_mutex);
  try
    {
      delete m_weapon_one;
      delete m_control;
    }
  catch(...)
    {}
}

const ship& ship::operator=( const ship& Arg )
{
  pthread_mutex_lock(&m_mutex);
  this->shape::operator=( Arg );

  delete this->m_weapon_one;

  this->m_control        = Arg.m_control;
  this->m_thrust         = Arg.m_thrust;
  this->m_rot            = Arg.m_rot;
  this->m_mass           = Arg.m_mass;
  this->m_weapon_one     = new weapon( *Arg.m_weapon_one );
  this->m_invunrableTime = Arg.m_invunrableTime; 
  this->m_updateTime     = Arg.m_updateTime;
  this->m_kind = Arg.m_kind;

  pthread_mutex_unlock(&m_mutex);
  return *this;
}

void ship::update()
{
  //  if (!m_control) 
  //    return;
  pthread_mutex_lock(&m_mutex);
  if (m_control) {
    if( m_control->state(FORWARD) )  // forward
      {
	this->accelerate( this->orientation() * m_thrust);
      }

    if( m_control->state(BACKWARD) )  // backward
      {
	this->accelerate( this->orientation() * (-m_thrust * 0.1) );
      }	

    if( m_control->state(LEFT) )  // left
      {
	this->rotation() = -m_rot;
      }
	
    if( m_control->state(RIGHT) )  // right
      {
	this->rotation() = m_rot;
      }

    if( m_control->state(FIRE) )  // fire
      {
	m_weapon_one->fire(this);
      }
  }
  const physics::time_t now( physics::runTime::create()->now() );
  const physics::time_t duration( now - m_updateTime );
  
  this->rotate( this->rotation() * duration );
  this->translate( this->velocity() * duration);
  
  if( m_invunrableTime > 0.0 ) m_invunrableTime -= duration;

  m_updateTime = now;

  this->rotation() = 0.0;
  
  pthread_mutex_unlock(&m_mutex);
  return;
}

void ship::draw()
{
  pthread_mutex_lock(&m_mutex);

  if( m_invunrableTime == 0.0 )
    {
      this->shape::draw();
    }
  // if ship is still invunrable then make it flash
  else
    {
      if( (static_cast<size_t>(m_invunrableTime * 10) % 2) == 0 )
	{
	  this->shape::draw();
	}
    }

  pthread_mutex_unlock(&m_mutex);
  // possibly add flames, smoke trail etc

  return;
}

control* ship::control_pointer() const
{
  return m_control;
}

void ship::destroy()
{
  pthread_mutex_lock(&m_mutex);

  if( m_invunrableTime > 0.0 )
    {}
  else
    {
      game::playerDestroyed();
      this->item::destroy();
    }

  pthread_mutex_unlock(&m_mutex);

  return;
}

// Rock 
static int s_rock_cnt;
pthread_mutex_t s_rockcnt_lock = PTHREAD_MUTEX_INITIALIZER;

rock::rock( const vec2d& Location,const vec2d& Velocity,const size_t Size ):
  shape( Location,Velocity,physics::rockClip( Size*10.0,Size*2 + 3 ) ),
  m_size(Size),
  m_updateTime(physics::runTime::create()->now())
{
  this->rotation() = (std::rand())/static_cast<float>(RAND_MAX) - 0.5;
 
  game::state::create()->targetAdded(); 
  {
  Lock m(s_rockcnt_lock);
  s_rock_cnt++;
  }
}

rock::rock( const rock& Arg ):
  shape(Arg),
  m_size(Arg.size()),
  m_updateTime( Arg.m_updateTime )
{
  Lock m(s_rockcnt_lock);
  s_rock_cnt++;
}

rock::~rock()
{
  try
    {
      game::state::create()->targetDestroyed();
      game::state::create()->increaseScore( this->m_size );
    }
  catch(...)
    {}
  {
  Lock m(s_rockcnt_lock);
  s_rock_cnt--;
  }
}

int rock::rockCount() {
  Lock m(s_rockcnt_lock);
  return s_rock_cnt;
}

const rock& rock::operator=( const rock& Arg )
{
  this->shape::operator=(Arg);
  this->m_size = Arg.size();
  this->m_updateTime = Arg.m_updateTime;
  
  return *this;
}
	
void rock::update()
{
  const physics::time_t now( physics::runTime::create()->now() );
  const physics::time_t duration( now - m_updateTime );
  
  this->rotate( this->rotation() * duration );
  this->translate( this->velocity() * duration);
  
  m_updateTime = now;

  return;
}

void rock::destroy()
{
  this->item::destroy();
	
  if( m_size > 3.0 )
    {
      elementManager* world( elementManager::create() );

      vec2d direction(0.0,1.0);
      
      direction.rotate( ((std::rand())/(double)RAND_MAX) * M_PI );
      const float angle( M_PI/2.0 );
	
      for( size_t i(0);i<4;++i )
	{
	  active::ptr rck( new rock( this->position() + direction * 30.0,
				   direction * 10.0 + this->velocity(),
				     m_size / 4 ));
	  world->insert( rck);
	  
	  direction.rotate( angle );
	}

    }
	
  return;
}

ship* insertPlayer(active::kind_t k)
{
  userControl * input = NULL;
  if (k == active::kLOCAL) {
    // insert new player ship
    input = new userControl;
  
    input->addAction(SDLK_UP);
    input->addAction(SDLK_DOWN);
    input->addAction(SDLK_LEFT);
    input->addAction(SDLK_RIGHT);
    input->addAction(SDLK_x);
  }

  ship* player( new ship( graphics::display::create()->dimension() * 0.5, 
			  input, k ) );

  active::ptr pl(player);
  elementManager::create()->insert( pl );

  return player;
}


turret::turret( const vec2d& Location,const vec2d& Velocity,control::ptr Control ):
  shape(Location,Velocity,physics::turretClip()),
  m_control(Control),	
  m_weapon(new weapon(2.0)),
  m_rot(0.5)
{
  m_control->setActiveTarget(this);
  game::state::create()->targetAdded();
}

turret::turret( const turret& Arg ):
  shape(Arg),
  m_control(Arg.m_control),	
  m_weapon(new weapon()),
  m_rot(Arg.m_rot)
{
  m_control->setActiveTarget(this);
}

turret::~turret()
{
  try
    {
      game::state::create()->targetDestroyed();
      game::state::create()->increaseScore( 5 );
    }
  catch(...)
    {}
}

const turret& turret::operator=( const turret& Arg )
{
  this->shape::operator=(Arg);

  delete this->m_weapon;

  m_control = ai::manager::create()->generate<ai::turret>();	
  m_weapon  = new weapon();
  m_rot     = Arg.m_rot;

  m_control->setActiveTarget(this);
  
  return *this;
}

void turret::update()
{
  if( m_control->state(LEFT) )  // left
    {
      this->rotation() = -m_rot;
    }
	
  if( m_control->state(RIGHT) )  // right
    {
      this->rotation() = m_rot;
    }

  if( m_control->state(FIRE) )  // fire
    {
      m_weapon->fire(this);
    }

  const physics::time_t now( physics::runTime::create()->now() );
  const physics::time_t duration( now - m_updateTime );
  
  this->rotate( this->rotation() * duration );
  this->translate( this->velocity() * duration);
  
  m_updateTime = now;

  this->rotation() = 0.0;
  
  return;
}

void turret::destroy()
{
  this->item::destroy();
	
  elementManager* world( elementManager::create() );
  
  vec2d direction(0.0,1.0);
  
  direction.rotate( ((std::rand())/(double)RAND_MAX) * M_PI );
  const float angle( M_PI/2.0 );
  
  for( size_t i(0);i<4;++i )
    {
      active::ptr rck(new rock( this->position() + direction * 20.0,
			       direction * 10.0 + this->velocity(),
				1 ));
      world->insert( rck );
      
      direction.rotate( angle );
    }
	
  return;
}

std::vector<active::ptr>& generateRocks( const size_t Count, std::vector<active::ptr>& Container )
{
  vec2d position;
  vec2d velocity( 0.0,10.0 );
  
  // generate rocks and insert into world
  for( size_t i(0);i<Count;++i )
    {
      velocity.rotate( (rand()/(static_cast<float>(RAND_MAX))) * (2.0 * M_PI) );
      Container.push_back( active::ptr(new rock( position,velocity,4 )) );
    }

  return Container;
}

std::vector<active::ptr>& generateTurrets( const size_t Count, std::vector<active::ptr>& Container )
{
  vec2d position;
  vec2d velocity( 0.0,10.0 );
  
  // generate rocks and insert into world
  for( size_t i(0);i<Count;++i )
    {
      velocity.rotate( (rand()/(static_cast<float>(RAND_MAX))) * (2.0 * M_PI) );
      Container.push_back( active::ptr(new turret( 
         position,velocity,
	 ai::manager::create()->generate<ai::turret>() )) );
    }

  return Container;
}
