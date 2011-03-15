// Nick Brett - 27-02-05
//
// Input.h
//
// A collection of classes to handle human
// input. Centered around the SDL event
// handler.

#include "input.h"

control::control()
{}

control::~control()
{}

//<-- class user Control -->
userControl::userControl():
  m_keyIndex()
{}

userControl::~userControl()
{}

void userControl::addAction( const int sdl_key )
{
  m_keyIndex.push_back( sdl_key );

  return;
}

void userControl::delAction( const int sdl_key )
{
  typedef std::vector<int>::iterator iterator;

  iterator result( std::find( m_keyIndex.begin(),m_keyIndex.end(),sdl_key ) );

  if( result != m_keyIndex.end() )
    {
      m_keyIndex.erase( result );
    }

  return;
}
		
const bool userControl::state( const int action ) const
{
  return inputState::create()->state( m_keyIndex[action] );
}

void userControl::setActiveTarget( active* Target)
{
  m_active_target = Target;
	
  return;
}

// <-- class inputState -->
inputState* inputState::m_ptrToSelf = NULL;

inputState::inputState():
  m_sdl_key_map(),
     m_event_queue(),
     m_sdl_quit(false)
{}

inputState* inputState::create()
{
  if( m_ptrToSelf == NULL )
    {
      m_ptrToSelf = new inputState();
    }

  return m_ptrToSelf;
}

const bool inputState::state( const int sdl_key ) const
{
  typedef std::map<int,bool>::const_iterator iterator;

  iterator result( m_sdl_key_map.find( sdl_key ) );

  if( result == m_sdl_key_map.end() )
    {
      return false;
    }

  return result->second;
}

void inputState::setKeyState( const int sdl_key, const bool state )
{
  // if sdl key is not already in key map then add it
  if( m_sdl_key_map.count( sdl_key ) < 1 )
    {
      m_sdl_key_map.insert( std::make_pair(sdl_key,false) );
    }

  // set state of key
  m_sdl_key_map[sdl_key] = state;
  
  return;
}
		
void inputState::readInput()
{
  while(SDL_PollEvent( &m_event_queue ) )
    {
      if(m_event_queue.type == SDL_KEYDOWN)
	{
	  this->setKeyState( m_event_queue.key.keysym.sym,true );
	}

      else if(m_event_queue.type == SDL_KEYUP)
	{
	  this->setKeyState( m_event_queue.key.keysym.sym,false );
	}
		
      else if(m_event_queue.type == SDL_QUIT)
	{
	  this->m_sdl_quit = true;
	}
    }
}

bool inputState::quit()
{
  return m_sdl_quit;
}
