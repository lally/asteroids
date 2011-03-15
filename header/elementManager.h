#ifndef ELEMENTMANAGER_CLASS
#define ELEMENTMANAGER_CLASS

// Copyright Nick Brett 2007
// contact nickdbrett@googlemail.com

#include <vector>
#include <algorithm>
#include <functional>
#include <boost/shared_ptr.hpp>

#include "physics.h"

#include "active.h"
#include "passive.h"

#include "ai.h"
#include "game.h"

/**
 * Describes the limits of the game world
 *
 */
class levelBoundary
{
 public:
  typedef std::pair<bool,vec2d> result;

  levelBoundary( const vec2d& );
  ~levelBoundary();
      
  /**
   * Remap Position 
   *
   * If the center of the argument is outside of the game world
   * it's position is changed such that it is inside the game
   * world at the correct position. If the arg's center is inside
   * the world but parts of the arg extend outside of the world
   * the position at which to place a copy of the arg is returned.
   */
  void remap( active::ptr,std::vector< std::pair<active::ptr,vec2d> >& ) const;

  const bool overlapEdge( const active::ptr, const size_t ) const;

  const physics::ray& edge( const size_t Arg ) const
    {
      return m_edge[Arg];
    }

  /** returns true if the point specified by the arg is within this
      boundary */
  const bool contains( const vec2d& ) const;

 private:
  vec2d        m_dimension;
  physics::ray m_edge[4];
};


class elementManager
{
 public:
  typedef std::vector<active::ptr>  activeContainer;
  typedef std::vector<passive::ptr> passiveContainer;

  ~elementManager();

  static elementManager* create();  

  /** Add elements to game world */
  void insert(active*);
  void insert(passive*);

  void insert(active::ptr);
  void insert(passive::ptr);

  /** Remove elements from game world */
  void erase(active*);
  void erase(passive*);

  void clear();

  /** Return the number of active Elements currently managed */
  const size_t numberOfActiveElements() const
    {
      return m_activePopulation.size();
    }

  /** Ask all objects to update their current state */
  void update();

  /** Draw all elements currently on screen */
  void draw() const;

  /** Calculate all possible collisions */
  void collide();
	
 private:
  elementManager();
  mutable pthread_mutex_t m_mutex;

  static elementManager* m_ptrToSelf;

  passiveContainer  m_passivePopulation;
  activeContainer   m_activePopulation; 
  activeContainer   m_activeAddEntries; 

  std::vector< std::pair<active::ptr,vec2d> > m_edgeOfScreen;     

  levelBoundary  m_boundary;

  physics::time_t m_lastUpdate;
};

/** generate a random distribution of arg stars and insert into elementManager */
void insertStars( const size_t );

/** evenly distribute the positions of objects in the container then
    insert them into the element manager  */
void insertEvenlyDistributed( std::vector<active::ptr>& );

#endif // ELEMENTMANAGER_CLASS
