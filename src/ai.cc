// Nick Brett - 06-03-05
//
// Ai.cxx
//
// A set of Artificial inteligence routines
// to control the npc's

#include "ai.h"

namespace ai
{
  actor::actor()
    {}
 
  actor::~actor()
    {}
  
  //<-- class turret -->
  turret::turret():
    actor(),
    m_active(NULL),
    m_state(),
    m_tolerance(0.01)
    {}
  
  turret::~turret()
    {}
  
  const bool turret::state( const int Arg ) const
    {
      return m_state[Arg];
    }

  void turret::setActiveTarget( active* Arg )
    {
      m_active = Arg;

      return;
    }

  void turret::update()
    {
      ship* target( game::state::create()->player() );
      
      physics::ray attackPlane( m_active->position(),m_active->position() + m_active->orientation() );
      const float direction( physics::separation( attackPlane, target->position() ) );
      
      m_state.reset();

      if( fabs(direction) < m_tolerance )
	{
	  m_state[FIRE] = true;
	}
      else if( direction < 0.0 )
	{
	  m_state[LEFT] = true;
	}
      else
	{
	  m_state[RIGHT] = true;
	}

      return;
    }

  manager* manager::m_ptrToSelf = NULL;

  manager::manager():
    m_population()
    {}

  manager::~manager()
    {}

  manager* manager::create() 
    {
      if( m_ptrToSelf == NULL )
	{
	  m_ptrToSelf = new manager();
	}

      return m_ptrToSelf;
    }

  // <-- class manager -->
  void manager::update()
    {	
      typedef std::vector<control::ptr>::iterator iterator;

      // erase unused ai
      m_population.erase( remove_if(m_population.begin(),m_population.end(),std::mem_fun_ref( &control::ptr::unique )),
			  m_population.end() );

      // update remaining ai
      iterator itr( m_population.begin() );
      iterator end( m_population.end() );

      for(; itr!=end;++itr )
	{
	  dynamic_cast<actor*>((*itr).get())->update();
	}


      return;
    }

}
