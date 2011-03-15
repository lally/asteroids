#ifndef PHYSICS_NAMESPACE
#define PHYSICS_NAMESPACE

// Copyright Nick Brett 2007
// contact nickdbrett@googlemail.com

#include <vector>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iostream>
#include <sstream>

#include <SDL.h>

#include "common.h"
#include "vec2d.h"

/**
 * physics namespace
 *
 * A collection of code for representing the physical shape of an
 * object in the game world and testing for collisions between those
 * shapes. This namespace also includes the clock classes and all code
 * pertaining to time.
 */
namespace physics
{

  typedef float time_t;

  /**
   * Clock class
   * 
   * A class to act as a timer for game events. Each instance will
   * begin counting time from the point at which it is instantiated
   * and will act independently of all other instances. The class
   * should be used like a stop watch for other objects.
   *
   */
  class clock
    {
    public:
      clock();
      clock( const clock& );
      ~clock();

      /**
       * Set current time
       *
       * Change the current value held by this clock to be equal to
       * that held by another clock.
       *
       */
      void set( const clock& );
      
      /** Set time to zero */
      void reset();

      /**
       * Start the clock
       *
       * This method ensures that the clocks local time is
       * incremented, following the system clock.
       *
       */
      void start();
      
      /**
       * Stop the clock
       *
       * This method will stop the clock from incrementing its local
       * time counter. The time on the clock will not change until a
       * call is made to the start() method.
       *
       */ 
      void stop();

      /**
       * Is the clock running ?
       *
       * This method returns true if the clock is running, false if it
       * is stopped.
       */
      bool running() const;
      

      /**
       * Start/Stop clock
       *
       * Starts the clock if it isn't currently running, stops it if
       * it is running.
       */
      void toggle();

      /**
       * Read current time (milliseconds)
       *
       * This method returns the number of milliseconds on the
       * clock. This value is returned as an unsigned integer, there
       * are no fractions of milliseconds.
       *
       */
      const time_t milliseconds() const;

      /**
       * Read current time (seconds)
       *
       * This method returns the number of seconds on the clock. This
       * value is returned as an unsigned integer, there are no
       * fractions of seconds.
       *
       */
      const time_t seconds() const;

      /**
       * Read current time (minutes)
       *
       * This method returns the number of minutes on the clock. This
       * value is returned as an unsigned integer, there are no
       * fractions of minutes.
       *
       */
      const time_t minutes() const;
      
      /**
       * Read current time (hours)
       *
       * This method returns the number of hours on the clock. This
       * value is returned as an unsigned integer, there are no
       * fractions of hours.
       *
       */
      const time_t hours() const;

      /**
       * Returns time as a string
       *
       * This method returns a human readable string containing the
       * current time_t in the format 'hours:minutes:seconds'. This can
       * be used to display the time on screen etc.
       *
       */
      const std::string read() const;

    private:
      /** Time when clock was last started */
      time_t m_startTime;
      
      /** Time on clock when it was last stopped*/
      time_t m_stopTime;

      /** Set true by a call to start, false by a call to stop */
      bool m_running;
    };

  /**
   * runTime class
   *
   * This class follows a singleton DP and acts as the reference time
   * frame for the game.
   */
  class runTime : public clock
    {
    public:
      static runTime* create();

      /** return time in fractions of a second as a float */
      const float now() const
	{
	  return this->milliseconds() * 0.001;
	}

    private:
      runTime();
      
      static runTime* m_pointerToSelf;
    };


  class collision
    {
    public:
      collision();
      collision( const vec2d& );
      collision( const collision& );

      const bool result() const
	{
	  return m_result;
	}

      const vec2d& location() const
	{
	  return m_location;
	}

    private:
      bool  m_result;
      vec2d m_location;
    };
  	
  /**
   * Ray
   *
   * A line with a begining and an end
   *
   */
  class ray
    {
    public:
      ray();
      ray( const vec2d&, const vec2d& );
      ray( const ray& );
      
      const ray& operator=( const ray& );

      const vec2d& begin() const
	{
	  return m_begin;
	}

      const vec2d& end() const
	{
	  return m_end;
	}

    private:
      vec2d m_begin;
      vec2d m_end;
    };


  /** returns the direction of a ray as end() - begin() */
  inline const vec2d direction( const ray& Arg )
    {
      return (Arg.end() - Arg.begin());
    }
  
  /** returns true + position if two rays intercect */
  const collision collide( const ray&, const ray& );

  /** 
   * Find Separation Between Ray and Point
   *
   * finds the perpendicular separation between a ray and a
   * point. Assumes that the ray extends to infinity in both
   * directions.
   */
  const float separation( const ray&, const vec2d& );

  /**
   * Clip Box
   *
   * Bounds the space occupied by an item in the game world. This
   * class is used to calculate collisions between object and
   * currently used to provide a graphical representation as well.
   */  
  class clip
    {
    public:
      typedef std::vector<vec2d> container;
      typedef container::iterator iterator;
      typedef container::const_iterator const_iterator;

      clip();
      clip( const vec2d&,const vec2d& );
      clip( const container& );
      clip( const clip& );
      ~clip();

      const clip& operator=( const clip& );

      iterator begin()
	{
	  return m_vertex.begin();
	}

      iterator end()
	{
	  return m_vertex.end();
	}

      const_iterator begin() const
	{
	  return m_vertex.begin();
	}

      const_iterator end() const
	{
	  return m_vertex.end();
	}

      vec2d& at( const size_t Arg )
	{
	  return m_vertex.at(Arg);
	}

      const vec2d& at( const size_t Arg ) const
	{
	  return m_vertex.at(Arg);
	}

      vec2d& center()
	{
	  return m_center;
	}

      const vec2d& center() const
	{
	  return m_center;
	}

      const float radiusSqrd() const
	{
	  return m_radiusSqrd;
	}

      /** returns the number of vetexs */
      const size_t size() const
	{
	  return m_vertex.size();
	}

      /** returns nth side of clip box, including side between last
	  vertex and 0th*/
      const ray line( const size_t ) const;
      
      const size_t lineCount() const
	{
	  return this->size();
	}
      
      /** revert to original vertex configuration */
      void reset();

    private:
      void findCenter();
      void findRadiusSqrd();

      container m_backup;
      container m_vertex;
      float     m_radiusSqrd;
      vec2d     m_center;
    };

  /** rotate all points in a clip about its center by the angle
      provided */
  void rotate( clip&, const float ); 

  /** translate all points in a clip along the vector provided */
  void translate( clip&, const vec2d& );

  /** rotate and translate a clip using the arguments */
  void transform( clip&,const float,const vec2d& );

  /** generate clip box in the shape of and equalatural triangle */
  const clip triangleClip( const float );

  /** generate triangular clip box for player's ship */
  const clip shipClip( const float );

  /** generate random circular rock shaped clip box for asteroids */
  const clip rockClip( const float, const size_t );

  /** generate appropriatly shaped clip box for gun turrets */
  const clip turretClip();

  /**
   * Collide Two Clip Boxes
   * 
   * returns the location of any collision beween two clip boxes and
   * a bool stateing weather or not a collision took place.
   */
  const collision collide( const clip&, const clip& );

  /**
   * Collide A Point With A Clip Box
   * 
   * returns the location of any collision beween a point and a clip
   * box and a bool stateing whether or not a collision took
   * place. Collision is calculated by testing weather or not the
   * point is inside the clip using Jordans Curve teqnique. This works
   * for any shape, convex or not. The point is returned as the
   * location of the collision if a collision takes place.
   */
  const collision collide( const vec2d&, const clip& );

}

#endif
