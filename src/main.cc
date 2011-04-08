// Nick Brett 15-02-05
//
// StarshipCommander.cxx
//
// An attempt to recreate the Acornsoft classic
// using Open-GL and ideal OO principals

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <typeinfo>
#include <sstream>
#include <ctime>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <strings.h>
#include <SDL.h>
#include "SDL_net.h"

#include "flags.h"
#include "vec2d.h"
#include "passive.h"
#include "input.h"
#include "ship.h"
#include "ai.h"
#include "elementManager.h"
#include "text.h"
#include "game.h"
#include "util.h"

// socket for sending our state to client.  if (client || server) {
// Transmitted at 10 Hz. }
UDPsocket udpsock;
pthread_mutex_t peerlock = PTHREAD_MUTEX_INITIALIZER;
bool have_peer;
IPaddress peer;

struct bullet_state_t {
  vec2d _position, _velocity;
};

struct playerstate_state_t {
  vec2d _position, _velocity;
  float _angle;
};

struct network_update_t {
  playerstate_state_t _player;
  // Use the packet size to determine how many new bullets there
  // really are.  If we lose a packet, we lose the bullets.
  bullet_state_t _new_bullets[1];
};

typedef std::pair<struct in_addr, ship*> thread_data_t;


// receive-handler thread.
static void * io_thread(void * arg /* unused */) {
  elementManager::activeContainer actives;
  puts ("[recv thread running]");
       
  //
  // start listening 

  // alloc packets for listening
  UDPpacket *recv_packet;

  recv_packet=SDLNet_AllocPacket(16384);
    
  if(!recv_packet) {
    printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
    exit(1);
  }
    
  const double muzzle_velocity = 150.0;
  elementManager* em = elementManager::create();

  while (1) {
    // wait for a packet & read.
    int retry = 16;
    int ret;
    while (SDLNet_UDP_Recv(udpsock, recv_packet) == 0 && retry)
      retry--;

    if (!retry) {
      // retries are failing, stop blowing cpu, wait 100ms.
      usleep(100000);
      continue;
    }
    
    putchar('r'); fflush(stdout);
    // decode
    pthread_mutex_lock(&peerlock);
    if (!have_peer) {
      peer = recv_packet->address;
      have_peer = true;
    }
    pthread_mutex_unlock(&peerlock);

    int nr_bl = (recv_packet->len - sizeof(playerstate_state_t)) 
      / sizeof(bullet_state_t);

    network_update_t *upd = (network_update_t*) recv_packet->data;

    // find the current player & apply the new state -- this is a
    // little fuzzy, as we may end up applying it to the old
    // (recently-deceased) player.  It'll get fixed at the next
    // packet receive.

    // apply
    playerstate_state_t& p = upd->_player;
    actives.clear();
    ship * s = 0;
    if (em->remoteActives(&actives) > 0) {
      for (int i=0; i<actives.size(); ++i) {
	if ((s = dynamic_cast<ship*>(actives[i].get()))) {
	  break;
	}
      }
      for (int i=0; i<nr_bl; ++i) {
	active::ptr sh(new shell(upd->_new_bullets[i]._position,
				 upd->_new_bullets[i]._velocity));
	em->insert(sh);
      }
    }
    if (!s) {
      s = insertPlayer(active::kREMOTE);
    }
    vec2d rot(0.0,-1.0);
    s->setState(p._position,  p._velocity, rot.rotate(p._angle),
		p._angle);
  }
  // SDLNet_FreePacket this packet when finished with it
  // well, we don't actually clean up, we just run until the
  // program's killed.
}

/*! \mainpage Asteroids
 *
 * \section intro_sec Introduction
 *
 * An asteroids clone written in C++ using OpenGL and the SDL
 * libraries. The player controls a triangular space-craft in a 2D
 * world populated by asteroids. The player must destroy all of the
 * asteroids to proceed to the next level. The 2D world is closed, so
 * if an object leaves the edge of the screen it will re-appear on the
 * opposite edge. The player has three lives and can lose them by
 * colliding with the asteroids or by getting shot. A side effect of a
 * closed world is that you can quite easily shoot yourself!  After
 * losing a life, the player re-spawns in the centre of the screen
 * with a one-second period of invulnerability (indicated by the
 * player's ship flashing on screen). When all lives have been lost,
 * the player is shown a game over message and invited to restart the
 * game.
 *
 * The controls are as follows:
 *
 * <table> 
 * <tr><td>accelerate forward</td> <td>cursor up</td></tr> 
 * <tr><td>accelerate backward</td><td>cursor down</td></tr> 
 * <tr><td>rotate left</td>        <td>cursor left</td></tr>
 * <tr><td>rotate right</td>       <td>cursor right</td></tr> 
 * <tr><td>fire</td>               <td>x</td></tr> 
 * </table> 
 * 
 * The code is written in C++, using OpenGL for rendering and SDL for
 * window creation and handling user input.
 * 
 * \section run_sec Running The Program
 *
 * Compiled versions of the game are available for Linux and Microsoft
 * Windows XP. To run the demo you'll need to install the SDL run time
 * libraries, availible from http://www.libsdl.org/download-1.2.php.
 * 
 * \subsection run_wxp Windows XP
 * 
 * Download the windows version of the SDL run time library, unzip the
 * file and copy SDL.dll file to your WINDOWS/system32 directory.  
 * 
 * \subsection run_lin Linux
 * 
 * The SDL run time library can be downloaded from the SDL site but
 * are most likely available through your distributions package
 * manager. You may need to change the permissions on the binary file
 * to make it executable first.
 * 
 * \section src_sec Building From Source
 * 
 * To compile the code from source you'll need to obtain the SDL
 * development libraries and a copy of the boost libraries.
 * 
 * \subsection src_wxp Windows XP
 * 
 * I used Dev-C++ version 4.9.9.2 to build the program on Windows
 * XP. The locations of the boost and SDL header files must be
 * specified in the box marked "Include Directories" which can be
 * found under the menu project->project options->Directories. The set
 * of required libraries must also be specified. Cut and paste "
 * -lmingw32 -lSDLmain -lSDL -mwindows -lwinmm -ldxguid -lopengl32
 * -lglu32 " into the box marked "linker options" which can be found
 * under the menu project->project options->parameters.
 * 
 * \subsection src_lin Linux
 * 
 * Once the boost and SDL dev libraries are installed, extract the
 * contents of the src tarball file, enter the base directory and type
 * 'make'. This should build and link the code, generating a binary
 * called 'asteroids.bin'. I built and tested the program using Ubuntu
 * Linux 7.04 with gcc (GCC) 4.1.2 and SDL version 1.2.11 (stable).
 *
 * 
 */
int main(int argc, char* argv[])
{
  int ch;
  bool server = false;
  bool client = false;
  struct timeval now, last_send;
  elementManager::activeContainer actives;

  try {
    physics::runTime*  clock( physics::runTime::create() );
    elementManager*    world( elementManager::create() );
    graphics::display* Display( graphics::display::create() );
    game::gui*         gui( game::gui::create() );
    ai::manager*       ai( ai::manager::create() );
    UDPpacket *send_packet;
    IPaddress ipself;
    int channel;

    while ((ch = getopt(argc, argv, "sc:h?a:b:")) != -1) {
      switch (ch) {
      case 's':
	server = true;
	game::setMode(game::kServerMode);
	break;
      case 'c':
	client = true;
	game::setMode(game::kClientMode);
	if (SDLNet_ResolveHost(&peer, optarg, 31337) != 0) {
	  puts ("bad server address");
	  exit(1);
	}
	break;
      case 'a':
        asteroid_factor = atoi(optarg);
        break;
      case 'b':
        bullet_factor = atoi(optarg);
        break;
      default:
	printf ("unknown option '%c'\n", ch);
      case 'h':
      case '?':
	printf("%s: [-s | -c hostname]\n", argv[0]);
      printf("  -s: be a server\n");
      printf("  -c: connect to a server, named 'hostname'\n");
      exit(1);
      break;
      }
    }
    
    printf ("Running asteroids\n");
    inputState* userInput( inputState::create() );
    
    printf("Initializing...");
    Display->initialise();
    
    clock->start();
    clock->reset();

    gettimeofday(&now, 0);
    gettimeofday(&last_send, 0);
    if (server || client) {
      // start up a listening thread and add a (thread-safe)
      // additional player

      if (server) {
	if (!(udpsock = SDLNet_UDP_Open(31337))) {
	  fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
	  exit(EXIT_FAILURE);
	}
      } else {
	if (!(udpsock = SDLNet_UDP_Open(0))) {
	  fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
	  exit(EXIT_FAILURE);
	}
	have_peer = true;
      }
      ship * remote = insertPlayer();
      pthread_t thr;
      int ret = pthread_create(&thr, NULL, io_thread, NULL);
      assert(ret == 0);
      util::enable_bullet_recording();
      puts ("[recv thread started]");
      send_packet=SDLNet_AllocPacket(16384);
    }

    printf("done\n");
    while( !(userInput->quit()) ) {
      game::checkState();

      userInput->readInput();

      ai->update();

      world->update();
      world->collide();

      world->draw();
      gui->draw();
            
      Display->update();

      // transmit at 10 Hz
      if (server || client) {
	gettimeofday(&now, 0);
	if (util::timeval_subtract(now, last_send) >= 0.1) {
	  // transmit state over.
	  network_update_t *upd = (network_update_t*) send_packet->data;
	  std::vector<active::ptr> bullets;
	  util::get_bullet_ptrs(&bullets);
	  int j = 0;
	  for (int i=0; i<bullets.size(); ++i) {
	    shell *sh = dynamic_cast<shell*>(bullets[i].get());
	    if (sh) {
	      upd->_new_bullets[j]._position = sh->position();
	      upd->_new_bullets[j]._velocity = sh->velocity();
	      j++;
	    }
	  }
	  // and now the ship
	  actives.clear();
	  world->localActives(&actives);
	  ship *self = 0;
	  bool got_self = false;
	  static bool reporting = true;
	  for (int i=0; i<actives.size(); ++i) {
	    if (reporting) {
	      printf("got a %s\n",
		     typeid(*actives[i].get()).name());
	    }
	    if ((self = dynamic_cast<ship*>(actives[i].get()))) {
	      upd->_player._position = self->position();
	      upd->_player._velocity = self->velocity();
	      upd->_player._angle = self->angle();
	      got_self = true;
	    }
	  }
	  reporting = false;
	  if (got_self) {
	    bool send = false;
	    pthread_mutex_lock(&peerlock);
	    if (have_peer) {
	      send = true;
	      send_packet->address.host = peer.host;
	      send_packet->address.port = peer.port;      
	    }
	    pthread_mutex_unlock(&peerlock);

	    if (send) {
	      send_packet->len = (sizeof (playerstate_state_t))
		+ bullets.size()*(sizeof (bullet_state_t));
	      SDLNet_UDP_Send(udpsock, -1, send_packet); 
	      putchar('S'); fflush(stdout);
	    }
	    last_send = now;
	  }
	}
      }
    }
    Display->kill();
  }
  catch( std::exception& exp ) {
    std::cout << "Exception: "
	      << exp.what()
	      << std::endl
	      << "exiting"
	      << std::endl;
  }

  exit(EXIT_SUCCESS);
}
