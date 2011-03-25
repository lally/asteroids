#include "game.h"

static game::mode_t s_mode = game::kAloneMode;


namespace game
{

  void setMode(mode_t m) {
    s_mode = m;
  }
  
  state* state::m_ptrToSelf = NULL;

  static void newPlayer() {
    state::create()->player() = insertPlayer();
    switch (s_mode) {
    case kServerMode: {
      // go halfway left.
      vec2d pos = graphics::display::create()->dimension() 
	* 0.5;
      pos.x() = pos.x() * 0.5;
      state::create()->player()->position() = pos;
      break;
    }
    case kClientMode: {
      // go halfway right.
      vec2d pos = graphics::display::create()->dimension() 
	* 0.5;
      pos.x() = pos.x() * 1.5;
      state::create()->player()->position() = pos;
      break;
    }
    default: // "alone" mode accepts the default.
      break;
    }

  }

  state::state():
    m_lives(),
    m_targetCount(),
    m_level(),
    m_score(),
    m_gameOn(),
    m_pause(),
    m_player(),
    m_control(NULL)
  {
    userControl* input( new userControl );
      
    input->addAction(SDLK_PAUSE);      
    input->addAction(SDLK_q);

    m_control = input;

    this->reset();
  }

  state::~state()
  {
    try
      {
	delete m_control;
      }
    catch(...)
      {}
  }

  state* state::create()
  {
    if( m_ptrToSelf == NULL )
      {
	m_ptrToSelf = new state();
      }

    return m_ptrToSelf;
  }

  void checkState()
  {
    if( state::create()->lives() < 1 )
      {
	// ignore the life count.
	//	gameOver();
      }
    else if( state::create()->startNewGame() )
      {
	// newGame();
      }
    else if( state::create()->targetCount() < 1 )
      {
	nextLevel();
      }

    if( state::create()->pause() )
      {
	//	pauseGame();
      }

    return;
  }

  void playerDestroyed()
  {
    state::create()->playerDestroyed();
      
    if( state::create()->lives() > 0 )
      {
	newPlayer();
      }

    return;
  }

  void nextLevel()
  {
    elementManager* world( elementManager::create() );
    game::state*    state( game::state::create() );

    world->clear();

    // increment level counter
    state->nextLevel();

    insertStars( 50 );
      
    std::vector<active::ptr> container;
      
    generateRocks( state->level() % 3, container );
    generateTurrets( state->level() / 3, container );
    insertEvenlyDistributed( container );

    newPlayer();
    //    state->player() = insertPlayer();

    gui::create()->insert( new levelMessage( state->level() ) );

    return;
  }

  void newGame()
  {
    elementManager::create()->clear();
    state::create()->reset();
    state::create()->startNewGame();

    gui::create()->insert( new game::hudMessage );

    nextLevel();
      
    return;
  }

  void gameOver()
  { 
    if( state::create()->gameOn() )
      {
	state::create()->gameOver();

	gui::create()->clear();
	gui::create()->insert( new gameOverMessage );
      }

    return;
  }

  void pauseGame()
  {
    physics::runTime::create()->stop();
    gui::create()->insert( new pauseMessage );

    return;
  }

  //<-- message class -->

  message::message():
    m_destroyed(false)
  {}

  message::~message()
  {}

  // <-- gui class -->
  gui* gui::m_ptrToSelf = NULL;

  gui::gui():
    m_content(),
    m_font16( "data/font-mono-sans-normal-16.bitmap",16,16 ),
    m_font48( "data/font-mono-sans-normal-48.bitmap",48,48 )
  {}

  gui::~gui()
  {}

  gui* gui::create()
  {
    if( m_ptrToSelf == NULL )
      {
	m_ptrToSelf = new gui();
      }

    return m_ptrToSelf; 
  }

  void gui::insert( message* Arg )
  {
    m_content.push_back( ptr(Arg) );

    return;
  }
  
  void gui::draw()
  {
    // remove destroyed elements
    m_content.erase( remove_if( m_content.begin(),m_content.end(),destroyed<message>() ),m_content.end() );

    // draw remaining elements
    for_each( m_content.begin(),m_content.end(),callDraw<message>() );

    return;
  }

  hudMessage::hudMessage():
    message(),
    m_livString("lives "),	
    m_scoString("score "),
    m_position( graphics::display::create()->dimension().x() * 0.02, graphics::display::create()->dimension().y() * 0.98 )
  {}

  hudMessage::~hudMessage()
  {}
  
  void hudMessage::draw()
  {
    std::stringstream strm;
    strm << m_livString << game::state::create()->lives() 
	 << " " 
	 << m_scoString << game::state::create()->score(); 
      
    graphics::drawString( strm.str(), gui::create()->normFont(), m_position );
      
    return;
  }    

  levelMessage::levelMessage( const size_t LevelNumber ):
    message(),
    m_level(LevelNumber),
    m_content(),
    m_position(graphics::display::create()->dimension().x(),graphics::display::create()->center().y()),
    m_velocity(),
    m_clock(), 
    m_ttl(3)
  {
    std::stringstream strm;
    strm << "level " << m_level;
    m_content = strm.str();

    m_position.y() += gui::create()->hugeFont().height() * 0.5;
    m_velocity = vec2d( -( graphics::display::create()->dimension().x() +
			   (m_content.size() * gui::create()->hugeFont().width())), 0.0 );

    m_velocity /= (m_ttl * 1000.0);

    m_clock.start();
  }
  
  levelMessage::~levelMessage()
  {}
  
  void levelMessage::draw()
  {
    graphics::drawString( m_content, gui::create()->hugeFont(), m_position + (m_velocity * m_clock.milliseconds()) );

    // remove message if it has exceeded its time to live or if we
    // have moved on to the next level
    if( (m_clock.seconds() > m_ttl) || (m_level != state::create()->level()) )
      {
	this->destroy();
      }

    return;
  }
  
  gameOverMessage::gameOverMessage():
    message(),
    m_control( new userControl ),
    m_msgString("GAME OVER"),
    m_msgPosition(graphics::display::create()->center()),
    m_infString("press space to continue"),
    m_infPosition(graphics::display::create()->center())
    
  {
    m_control->addAction(SDLK_SPACE);
      
    gui* theGui( gui::create() );

    m_msgPosition.x() -= theGui->hugeFont().width() * m_msgString.size() * 0.5;
    m_msgPosition.y() += theGui->hugeFont().height() * 0.5;
    m_msgPosition.y() -= theGui->normFont().height() * 0.5;

    m_infPosition.x() -= theGui->normFont().width() * m_infString.size() * 0.5;
    m_infPosition.y() += theGui->hugeFont().height() * 0.5;
    m_infPosition.y() += theGui->normFont().height() * 0.5;
  }

  gameOverMessage::~gameOverMessage()
  {
    try
      {
	delete m_control;
	state::create()->reset();
      }
    catch(...)
      {}
  }
  
  void gameOverMessage::draw()
  {
    graphics::drawString( m_msgString, gui::create()->hugeFont(), m_msgPosition );
    graphics::drawString( m_infString, gui::create()->normFont(), m_infPosition );
      
    if( m_control->state(0) )
      {
	this->destroy();
      } 
  }

  //<-- pause message class -->
  pauseMessage::pauseMessage():
    message(),
    m_msgString("PAUSED"),
    m_msgPosition(graphics::display::create()->center())
  { 
    gui* theGui( gui::create() );

    m_msgPosition.x() -= theGui->hugeFont().width() * m_msgString.size() * 0.5;
    m_msgPosition.y() += theGui->hugeFont().height() * 0.5;
  }

  pauseMessage::~pauseMessage()
  {
    try
      {
	physics::runTime::create()->start();
      }
    catch(...)
      {}
  }
  
  void pauseMessage::draw()
  {
    graphics::drawString( m_msgString, gui::create()->hugeFont(), m_msgPosition );
      
    if( state::create()->pause() )
      {
	this->destroy();
      } 
  }


  
}
