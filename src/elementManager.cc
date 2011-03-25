/***************************************************************************
 *            ElementManager.cxx
 *
 *  Sat Mar 12 21:37:36 2005
 *  Copyright  2005  Nick Brett
 *  n.brett1@physics.ox.ac.uk
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include "elementManager.h"
#include "lock.h"

levelBoundary::levelBoundary( const vec2d& Arg ):
  m_dimension(Arg),
     m_edge()
{
  vec2d zero;

  m_edge[0] = physics::ray( zero, vec2d( m_dimension.x(),zero.y() ) );
  m_edge[1] = physics::ray( vec2d( m_dimension.x(),zero.y() ), m_dimension  );
  m_edge[2] = physics::ray( m_dimension,vec2d( zero.x(),m_dimension.y() ) );
  m_edge[3] = physics::ray( vec2d( zero.x(),m_dimension.y() ),zero );
}

levelBoundary::~levelBoundary()
{}

void levelBoundary::remap( active::ptr Arg, std::vector< std::pair<active::ptr,vec2d> >& Container ) const
{
  // if Arg's CoM is outside the boundary then move it inside the
  // boundary
  if( Arg->position().x() < 0.0 )
    {
      Arg->translate( vec2d( m_dimension.x(),m_dimension.y() - (Arg->position().y() * 2.0) ) );
      Arg->velocity().invertY();
    }
  else if( Arg->position().x() > m_dimension.x() )
    {
      Arg->translate( vec2d( -m_dimension.x(),m_dimension.y() - (Arg->position().y() * 2.0) ) );
      Arg->velocity().invertY();
    }

  if( Arg->position().y() < 0.0 )
    {
      Arg->translate( vec2d( m_dimension.x() - (Arg->position().x() * 2.0) ,m_dimension.y() ) );
      Arg->velocity().invertX();
    }
  else if( Arg->position().y() > m_dimension.y() )
    {
      Arg->translate( vec2d( m_dimension.x() - (Arg->position().x() * 2.0) , -m_dimension.y() ) );
      Arg->velocity().invertX();
    }

  // test Arg: is it a point object or does it have a shape ?
  shape* shapePtr( dynamic_cast<shape*>(Arg.get()) );

  if( shapePtr == NULL )
    return;

  // if Arg has a shape then test whether it collides with edges
  // of boundary. If Arg overlaps boundary then copy it into
  // Container with a position outside the boundary but following
  // the appropriate remapping.
      
  if( this->overlapEdge(Arg,0) )
    {
      vec2d location( m_dimension.x() - Arg->position().x(),Arg->position().y() + m_dimension.y() );
      Container.push_back( std::make_pair( Arg,location ) );
	  
      return;
    }

  if( this->overlapEdge(Arg,1) )
    {
      vec2d location( Arg->position().x() - m_dimension.x(),m_dimension.y() - Arg->position().y() );
      Container.push_back( std::make_pair( Arg,location ) );
	  
      return;
    }
      
  if( this->overlapEdge(Arg,2) )
    {
      vec2d location( m_dimension.x() - Arg->position().x(),Arg->position().y() - m_dimension.y() );
      Container.push_back( std::make_pair( Arg,location ) );
	  
      return;
    }

  if( this->overlapEdge(Arg,3) )
    {
      vec2d location( Arg->position().x() + m_dimension.x(),m_dimension.y() - Arg->position().y() );
      Container.push_back( std::make_pair( Arg,location ) );
	  
      return;
    }

  return;
}

const bool levelBoundary::overlapEdge( const active::ptr Arg, const size_t Edge ) const
{
  float separation( physics::separation( this->edge(Edge),Arg->position() ) );
  
  separation *= separation;

  return separation < Arg->radiusSqrd();
}

const bool levelBoundary::contains( const vec2d& Arg ) const
{
  if( Arg.x() < 0.0 )
    return false;
  
  if( Arg.x() > m_dimension.x() )
    return false;
  
  if( Arg.y() < 0.0 )
    return false;
  
  if( Arg.y() > m_dimension.y() )
    return false;
  
  return true;
}


elementManager* elementManager::m_ptrToSelf = NULL;
 
elementManager::elementManager(): 
  m_passivePopulation(),
  m_activePopulation(),
  m_activeAddEntries(),
  m_edgeOfScreen(),
  m_boundary( vec2d(512,512) ),
  m_lastUpdate( physics::runTime::create()->now() ),
  m_mutex(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP)
{
}
 
elementManager::~elementManager()
{
  try
    {
      this->clear();
    }
  catch(...)
    {}
  pthread_mutex_destroy(&m_mutex);
}
	
elementManager* elementManager::create()
{
  if( m_ptrToSelf == NULL )
    {
      m_ptrToSelf = new elementManager();
    }

  return m_ptrToSelf;
}

/*void elementManager::insert(active* Arg)
{
  Lock m(m_mutex);
  this->insert( active::ptr(Arg) );

  return;
}

void elementManager::insert(passive* Arg)
{
  Lock m(m_mutex);
  this->insert( passive::ptr(Arg) );
  
  return;
  }*/


void elementManager::insert( active::ptr Arg )
{
  Lock m(m_mutex);
  m_activeAddEntries.push_back( Arg );

  return;
}

void elementManager::insert( passive::ptr Arg )
{
  Lock m(m_mutex);
  m_passivePopulation.push_back( Arg );
  
  return;
}


void elementManager::erase(active* Arg)				// Remove elements from game world
{
  Lock m(m_mutex);
  m_activePopulation.erase( remove( m_activePopulation.begin(),
				    m_activePopulation.end(),active::ptr(Arg) ),
			    m_activePopulation.end() );

  return;
}

void elementManager::erase(passive* Arg)
{
  Lock m(m_mutex);
  m_passivePopulation.erase( remove( m_passivePopulation.begin(),
				     m_passivePopulation.end(),passive::ptr(Arg) ),
			     m_passivePopulation.end() );

  return;
}

int elementManager::localActives(elementManager::activeContainer* dest) {
  Lock m(m_mutex);
  int count = 0;
  for (int i=0; i<m_activePopulation.size(); ++i) {
    active::ptr k = m_activePopulation[i];
    if (k->kind() == active::kLOCAL) {
      dest->push_back(k);
      count++;
    }
  }
  for (int i=0; i<m_activeAddEntries.size(); ++i) {
    active::ptr k = m_activeAddEntries[i];
    if (k->kind() == active::kLOCAL) {
      dest->push_back(k);
      count++;
    }
  }
  return count;
}

int elementManager::remoteActives(elementManager::activeContainer* dest) {
  Lock m(m_mutex);
  int count = 0;
  for (int i=0; i<m_activePopulation.size(); ++i) {
    active::ptr k = m_activePopulation[i];
    if (k->kind() == active::kREMOTE) {
      dest->push_back(m_activePopulation[i]);
      count++;
    }
  }
  for (int i=0; i<m_activeAddEntries.size(); ++i) {
    active::ptr k = m_activeAddEntries[i];
    if (k->kind() == active::kREMOTE) {
      dest->push_back(k);
      count++;
    }
  }
  return count;
}

void elementManager::clear()
{
  Lock m(m_mutex);
  m_passivePopulation.clear();
  m_activePopulation.clear(); 
  m_activeAddEntries.clear(); 
  m_edgeOfScreen.clear();     
  
  return;
}
	
void elementManager::update()
{	
  Lock m(m_mutex);
  // update all active objects held in population
  for_each( m_activePopulation.begin(),m_activePopulation.end(),mem_fun_ptr<active,void>( &active::update ) );
  
  // remove destroyed elements
  m_activePopulation.erase( remove_if( m_activePopulation.begin(),m_activePopulation.end(), destroyed<active>() ), 
			    m_activePopulation.end() );

  // add new elements to active population
  copy( m_activeAddEntries.rbegin(),m_activeAddEntries.rend(), back_inserter(m_activePopulation) );
  m_activeAddEntries.clear();

  // enforce proper behaviour at screen edges
  m_edgeOfScreen.clear();

  std::vector<active::ptr>::iterator itr( m_activePopulation.begin() );
  std::vector<active::ptr>::iterator end( m_activePopulation.end() );
  
  for(; itr!=end; ++itr )
    {
      m_boundary.remap( *itr, m_edgeOfScreen );
    }

  return;
}

void elementManager::draw() const
{
  using std::for_each;
  using std::mem_fun;

  Lock m(m_mutex);
  for_each( m_passivePopulation.begin(),m_passivePopulation.end(),mem_fun_ptr<passive,void>( &passive::draw ) );
  for_each( m_activePopulation.begin(),m_activePopulation.end(),mem_fun_ptr<active,void>( &active::draw ) );

  // draw elements which overlap screen edges
  std::vector< std::pair<active::ptr,vec2d> >::const_iterator itr( m_edgeOfScreen.begin() );
  std::vector< std::pair<active::ptr,vec2d> >::const_iterator end( m_edgeOfScreen.end() );

  for(; itr!=end;++itr )
    {
      itr->first->draw( itr->second );
    }

  return;
}

void elementManager::collide()
{
  Lock m(m_mutex);

  // collide active population with self
  std::vector<active::ptr>::iterator itr1( m_activePopulation.begin() );
  std::vector<active::ptr>::iterator itr2( m_activePopulation.begin() );

  physics::collision Collision;	

  for(; itr1 != m_activePopulation.end(); ++itr1)
    {
      for(itr2 = (itr1 +1); itr2 != m_activePopulation.end(); ++itr2)
	{
	  Collision = ::collide( itr1->get(), itr2->get() );

	  if( Collision.result() && (m_boundary.contains( Collision.location() )) )
	    {
	      resolveCollision( *itr1,*itr2,Collision.location() );
	    }
	}
    }
 
  // collide screen edge population with active population and with self
   std::vector< std::pair<active::ptr,vec2d> >::iterator itrEdge1( m_edgeOfScreen.begin() );
   std::vector< std::pair<active::ptr,vec2d> >::iterator itrEdge2( m_edgeOfScreen.begin() );
   std::vector< std::pair<active::ptr,vec2d> >::iterator endEdge( m_edgeOfScreen.end() );
 
   vec2d originalPosition1;
   vec2d originalPosition2;
 
   for(; itrEdge1!=endEdge; ++itrEdge1)
     {
       originalPosition1 = itrEdge1->first->position(); 
       itrEdge1->first->position() = itrEdge1->second;
       
       for(itr1 = m_activePopulation.begin();
 	  itr1 != m_activePopulation.end(); 
 	  ++itr1 )
 	{
 	  Collision = ::collide( itr1->get(), itrEdge1->first.get() );
 
 	  if( Collision.result() && (m_boundary.contains( Collision.location() )) )
 	    {
	      resolveCollision( *itr1,itrEdge1->first,Collision.location() );
 	    }
 	}
 
       for(itrEdge2 = m_edgeOfScreen.begin();
 	  itrEdge2 != m_edgeOfScreen.end(); 
 	  ++itrEdge2 )
 	{

	  originalPosition2 = itrEdge2->first->position(); 
	  itrEdge2->first->position() = itrEdge2->second;
      

 	  Collision = ::collide( itrEdge2->first.get(), itrEdge1->first.get() );
 
 	  if( Collision.result() && (m_boundary.contains( Collision.location() )) )
 	    {
	      resolveCollision( itrEdge2->first,itrEdge1->first,Collision.location() );
 	    }

	  itrEdge2->first->position() = originalPosition2; 
 	}

       itrEdge1->first->position() = originalPosition1; 
     }

   return;
}

void insertStars( const size_t StarCount )
{
  elementManager*    world( elementManager::create() );
  graphics::display* display( graphics::display::create() );

  srand( static_cast<int>(time(NULL)) );
  vec2d position;

  // generate random star field
  for( size_t i(0);i<StarCount;++i )
    {
      position.x(display->dimension().x()*(rand()/(static_cast<double>(RAND_MAX))) );
      position.y(display->dimension().y()*(rand()/(static_cast<double>(RAND_MAX))) );
      passive::ptr s(new star(position));
      world->insert(s);
    }

  return;
}


void insertEvenlyDistributed( std::vector<active::ptr>& Container )
{
  typedef std::vector<active::ptr>::iterator iterator;

  elementManager*    world( elementManager::create() );
  graphics::display* display( graphics::display::create() );

  srand( static_cast<int>(time(NULL)) );
  const vec2d center( display->dimension() * 0.5 );

  const float angularSeparation( (2.0 * M_PI) / static_cast<float>(Container.size()) );
  vec2d position( center * 0.6 );
  position.rotate( (rand()/(static_cast<float>(RAND_MAX))) * (2.0 * M_PI) );

  // generate rocks and insert into world
  iterator itr( Container.begin() );
  iterator end( Container.end() );

  for(; itr!=end;++itr )
    {
      (*itr)->position() = center + position;
      world->insert( *itr );

      position.rotate( angularSeparation );
    }
  
  return;
}
