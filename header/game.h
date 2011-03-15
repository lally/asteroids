#ifndef GAME_NAMESPACE
#define GAME_NAMESPACE

// Copyright Nick Brett 2007
// contact nickdbrett@googlemail.com

#include <string>
#include <sstream>
#include <algorithm>

#include "common.h"
#include "vec2d.h"
#include "graphics.h"
#include "elementManager.h"
#include "input.h"
#include "passive.h"
#include "ship.h"
#include "physics.h"

class ship;

/**
 * game namespace
 *
 * A collection of code for monitoring the game state and handling
 * user feed back through on screen messages.
 */
namespace game
{
  void checkState();
  void playerDestroyed();
  void newGame();
  void nextLevel();
  void gameOver();
  void pauseGame();

  class state
    {
    public:
      enum{PAUSE,QUIT};

      ~state();
      
      static state* create();

      void playerDestroyed()
	{
	  --m_lives;
	  
	  return;
	}
      
      const size_t lives() const
	{
	  return m_lives;
	}

      void targetDestroyed()
	{
	  --m_targetCount;

	  return;
	}

      void targetAdded()
	{
	  ++m_targetCount;

	  return;
	}

      const size_t targetCount() const
	{
	  return m_targetCount;
	}

      const size_t level() const
	{
	  return m_level;
	}

      void nextLevel()
	{
	  ++m_level;

	  return;
	}

      const size_t score() const
	{
	  return m_score;
	}

      void increaseScore( const size_t Arg )
	{
	  m_score += Arg;

	  return;
	}
      
      const bool gameOn() const
	{
	  return m_gameOn;
	}
      
      void gameOver()
	{
	  m_gameOn = false;
	  
	  return;
	}

      const bool startNewGame()
	{
	  if( m_startNewGame )
	    { 
	      m_startNewGame = false;
	     
	      return true;
	    }

	  return false;
	}

      const bool inputState( const size_t Arg ) const
	{
	  return m_control->state(Arg);
	}

      const bool pause()
	{
	  bool rtn(false);

	  if( m_pause )
	    {
	      rtn = false;
	    }
	  else
	    {
	      rtn = m_control->state(PAUSE);
	    }

	  m_pause = m_control->state(PAUSE);

	  return rtn;
	}

      const ship* player() const
	{
	  return m_player;
	}


      ship*& player()
	{
	  return m_player;
	}

      void reset()
	{
	  m_lives        = 3;	 	  
	  m_targetCount  = 0;
	  m_level        = 0;
	  m_score        = 0;    
	  m_gameOn       = true;
	  m_startNewGame = true;
	  m_pause        = false;

	  return;
	}

    private:
      state();

      static state* m_ptrToSelf;

      size_t m_lives;	 
      size_t m_targetCount;
      size_t m_level;	 
      size_t m_score;    
     
      bool   m_gameOn;      // true if game is not over
      bool   m_startNewGame;
      bool   m_pause;
      ship*  m_player;

      control* m_control;
    };

  
  
  class message
    {
    public:
      message();
      virtual ~message();

      virtual void draw()=0;
	  
      virtual void destroy()
	{
	  m_destroyed = true;
	      
	  return;
	}

      virtual const bool destroyed() const
	{
	  return m_destroyed;
	}

    private:
      bool m_destroyed;
    };


  class gui
    {
    public:
      typedef boost::shared_ptr<message> ptr;

      ~gui();

      static gui* create();

      void insert( message* );
      void draw();

      font& normFont()
	{
	  return m_font16;
	}

      font& hugeFont()
	{
	  return m_font48;
	}

      void clear()
	{
	  m_content.clear();
	  
	  return;
	}

    private:
      gui();

      static gui* m_ptrToSelf;
      std::vector<ptr> m_content;

      font m_font16;
      font m_font48;
    };

  class hudMessage : public message
    {
    public:
      hudMessage();
      virtual ~hudMessage();
      
      virtual void draw();      

    private:
      std::string m_livString;	
      std::string m_scoString;	
      vec2d       m_position;
    };

  class levelMessage : public message
    {
    public:
      levelMessage( const size_t );
      virtual ~levelMessage();
      
      virtual void draw();      

    private:
      size_t          m_level;
      std::string     m_content;
      vec2d           m_position;
      vec2d           m_velocity;
      physics::clock  m_clock;
      physics::time_t m_ttl;
    };

  class gameOverMessage : public message
    {
    public:
      gameOverMessage();
      virtual ~gameOverMessage();
      
      virtual void draw();      

    private:
      userControl* m_control;

      std::string m_msgString;
      vec2d       m_msgPosition;
      std::string m_infString;
      vec2d       m_infPosition;
    };


  class pauseMessage : public message
    {
    public:
      pauseMessage();
      virtual ~pauseMessage();
      
      virtual void draw();      

    private:
      std::string m_msgString;
      vec2d       m_msgPosition;
    };

}


#endif
