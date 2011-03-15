#ifndef AI_NAMESPACE
#define AI_NAMESPACE

// Copyright Nick Brett 2007

#include <cstdlib>
#include <ctime>

#include <vector>
#include <bitset>

#include "vec2d.h"

#include "input.h"
#include "active.h"
#include "ship.h"

/**
 * Ai namespace
 *
 * A set of Artificial inteligence routines
 * to control the npc's
 */
namespace ai
{
  class actor : public control
    {
    public:
      actor();
      virtual ~actor();

      /** return state of action */
      virtual const bool state( const int ) const=0;

      /** set address of target Active object */
      virtual void setActiveTarget( active* )=0; 

      /** Make decisions based on current situation and update m_state
	  container */
      virtual void update()=0;
    };

  class turret : public actor
    {
    public:
      enum{LEFT, RIGHT, FIRE};

      turret();
      virtual ~turret();

      virtual const bool state( const int ) const;
      virtual void setActiveTarget( active* ); 
      virtual void update();

    private:
      /** pointer to active object that this instance of ai is
	  responsible for */
      active* m_active;

      /** 
       * container holding state of each "button" for the npc (trying
       * to keep ai's interface with active items the same as the
       * human interface)
       */
      std::bitset<3> m_state;
      
      float m_tolerance;
    };

  /***
   * Ai Manager
   *
   * Singleton container for ai control objects.
   */
  class manager
    {
    public:
      ~manager();

      static manager* create();

      /** Ask all objects to update their current state */
      void update();  

      /** 
	  generate a new instance of an ai object, add it to the
	  population and return a smart pointer to a control object
	  which refferes to the new ai
      */
      template< typename T > control::ptr generate();

      const size_t size() const
	{
	  return m_population.size();
	}

    private:
      manager();
      static manager* m_ptrToSelf;

      std::vector<control::ptr> m_population;
    };

  template< typename T > control::ptr manager::generate()
    {
      control::ptr candidate( new T );
      m_population.push_back( candidate );
      
      return candidate;
    }

}

#endif // AI_NAMESPACE
