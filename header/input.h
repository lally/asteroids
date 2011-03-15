#ifndef INPUT_CLASSES
#define INPUT_CLASSES

// Copyright Nick Brett 2007
// contact nickdbrett@googlemail.com

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <SDL/SDL.h>

#include "active.h"

/**
 * A collection of classes to handle human
 * input. Centered around the SDL event
 * handler.
 */
 
/**
 * Control
 *
 * Every active element in the game will 
 * contain an instance of control. It 
 * should act as an interface between
 * game elements and either the HID (Human
 * Interface Device, keyboard etc) or an
 * Artificial inteligence 
 */
class control
{
 public:
  typedef boost::shared_ptr<control> ptr;

  control();
  virtual ~control();

  virtual const bool state( int ) const =0;
  virtual void setActiveTarget( active* target)=0;
};

/**
 * User Control
 *
 * reads the state of specified user inputs an provides a 'control'
 * interface to the infomation.
 */
class userControl : public control
{
 public:
  userControl();
  ~userControl();
		
  void addAction( const int );
  void delAction( const int );

  /** Return state of a particular key */
  virtual const bool state( const int ) const;	

  /** Set the pointer to the game element under control */
  virtual void setActiveTarget( active* target);
		
 private:
  std::vector<int> m_keyIndex; /** maps the ith action held by this to an SDL key (represented by an int) */		
  active* m_active_target;     /** Pointer to active object under the control of this instance */
		
};

/**
 * Input State 
 *
 * Holds the state of all keyboard buttons as true (pressed) or false
 * (not pressed). This infomation is made availible to any other
 * object through a singleton interface.
 */
class inputState
{
 public:
  ~inputState();

  static inputState* create();

  /** Read event que and update all userControl objects */
  void readInput(); 

  /** returns the state of an SDL key, true (pressed) or false (not pressed) */
  const bool state( const int ) const;

  /** Returns true if SDL_QUIT event seen */
  bool quit();

 private:
  inputState();
	
  /** Define the state (true/false) of a user input key */
  void setKeyState( const int, const bool );


  static inputState* m_ptrToSelf;

  std::map<int,bool> m_sdl_key_map;
  SDL_Event m_event_queue;
  bool      m_sdl_quit;

};

#endif // INPUT_CLASSES
