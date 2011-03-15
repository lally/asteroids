#include "physics.h"

namespace physics
{
  // <-- clock class -->
  clock::clock( const clock& arg ): m_startTime( arg.m_startTime ),
    m_stopTime( arg.m_stopTime ),
    m_running( arg.m_running )
    {}

  clock::clock(): m_startTime( SDL_GetTicks() ),
    m_stopTime(0),
    m_running( true )
    {}

  clock::~clock()
    {}

  void clock::set( const clock& arg )
    {
      this->m_startTime = arg.m_startTime;
      this->m_stopTime  = arg.m_stopTime;
      this->m_running   = arg.m_running;

      return;
    }

  void clock::reset()
    {
      m_startTime = SDL_GetTicks();
      m_stopTime  = 0;

      return;
    }

  void clock::start()
    {
      if( !m_running )
	{
	  m_startTime = SDL_GetTicks();
	  m_running   = true;
	}

      return;
    }

  void clock::stop()
    {
      if( m_running )
	{
	  m_stopTime  = this->milliseconds();
	  m_running   = false;
	}

      return;
    }

  bool clock::running() const
    {
      return m_running;
    }

  void clock::toggle()
    {
      if( this->running() ) 
	{
	  this->stop();
	}
      else
	{
	  this->start();
	} 

      return;
    }

  const time_t clock::milliseconds() const
    {
      time_t rtn;

      if( m_running )
	{
	  rtn = m_stopTime + (SDL_GetTicks() - m_startTime);
	}

      else
	{
	  rtn = m_stopTime;
	}

      return rtn;
    }

  const time_t clock::seconds() const
    {
      return static_cast<size_t>( this-> milliseconds() * 0.001 );
    }

  const time_t clock::minutes() const
    {
      return static_cast<size_t>( this->seconds() / 60.0 );
    }

  const time_t clock::hours() const
    {
      return static_cast<size_t>( this->seconds() / 3600.0 );
    }

  const std::string clock::read() const
    {
      std::ostringstream rtn;

      rtn << this->hours()
	  << ":"
	  << this->minutes() - ( this->hours() * 60.0 )
	  << ":"
	  << this->seconds() - ( this->minutes() * 60.0 );

      return rtn.str();
    }

  // <-- runTime class -->
  runTime* runTime::m_pointerToSelf = NULL;

  runTime::runTime():clock(){}

  runTime* runTime::create()
    {
      if( m_pointerToSelf == NULL )
	{
	  m_pointerToSelf = new runTime();
	}

      return m_pointerToSelf;
    }

  // <-- collision -->
  collision::collision():
    m_result(false),
    m_location()
    {}

  collision::collision( const vec2d& Location ):
    m_result(true),
    m_location(Location)
    {}

  collision::collision( const collision& Arg ):
    m_result(Arg.result()),
    m_location(Arg.location())
    {}

  // <-- ray -->
  ray::ray():
    m_begin(),
    m_end()
    {}

  ray::ray( const vec2d& Begin, const vec2d& End ):
    m_begin(Begin),
    m_end(End)
    {}

  ray::ray( const ray& Arg ):
    m_begin(Arg.begin()),
    m_end(Arg.end())
    {}
  
  const ray& ray::operator=( const ray& Arg )
    {
      this->m_begin = Arg.begin();
      this->m_end   = Arg.end();
      
      return *this;
    }

  const collision collide( const ray& A, const ray& B )
    {
      // treat ray A as a plane and find the point where ray B
      // intersects it
      const vec2d n( normalise( perpendicular( direction(A) ) ) );
      const float denominator( dot(n, direction(B) ) );

      // if zero A and B are parrallel
      if( denominator == 0.0 ) 
	return collision();

      // find position of intersect as a fraction of the length of ray B
      float parameter( ( dot(n,A.begin()) - dot(n,B.begin()) )/denominator );

      // test to see if intersect happens between the end points of
      // ray B
      if( (parameter < 0.0) || (parameter > 1.0) )
	return collision();

      // find actual location of intersect
      const vec2d location( B.begin() + direction(B) * parameter );
      
      // test whether intercect happens between the end points of ray
      // A
      parameter = direction(A).x();

      if(parameter == 0 )
	return collision();

      parameter = (location.x() - A.begin().x())/parameter;

      if( (parameter < 0.0) || (parameter > 1.0) )
	return collision();
      
      // return collision at the point identified
      return collision( location );
    }
  
  const float separation( const ray& Ray, const vec2d& Point )
    {
      const vec2d n( normalise( perpendicular( direction(Ray) ) ) );
      return dot(n,Point) - dot(n,Ray.begin());
    }

  // <-- clip -->
  clip::clip( const vec2d& A, const vec2d& B ):
    m_backup(),
    m_vertex(),
    m_center()
    {
      m_backup.push_back(A);
      m_backup.push_back(B);
      
      m_vertex.reserve( m_backup.size() );

      this->reset();
      this->findCenter();
      this->findRadiusSqrd();
    }
  
  clip::clip( const container& Arg ):
    m_backup(Arg),
    m_vertex(Arg),
    m_center()
    {
      this->findCenter();
      this->findRadiusSqrd();
    }

  clip::clip( const clip& Arg ):
    m_backup( Arg.m_backup ),
    m_vertex( Arg.m_vertex ),
    m_radiusSqrd( Arg.radiusSqrd() ),
    m_center( Arg.center() )
    {}

  clip::~clip()
    {}
  
  const clip& clip::operator=( const clip& Arg )
    {
      this->m_backup     = Arg.m_backup;
      this->m_vertex     = Arg.m_vertex;
      this->m_radiusSqrd = Arg.radiusSqrd();
      this->m_center     = Arg.center();

      return *this;
    }

  const ray clip::line( const size_t Arg ) const
    {
      size_t a(Arg);
      size_t b(Arg+1);

      if( b >= this->size() )
	{
	  a = this->size() - 1;
	  b = 0;
	}

      return ray( this->at(a),this->at(b) );
    }

  void clip::reset()
    {
      m_vertex = m_backup;
      m_center = vec2d();

      return;
    }

  void clip::findCenter()
    {
      const vec2d center( std::accumulate( this->begin(), this->end(), vec2d(0,0) ) );
      for_each( this->begin(),this->end(),std::bind2nd( subtractAssign<vec2d>(),center ) );
 
      return;
    }

  void clip::findRadiusSqrd()
    {
      iterator itr( this->begin() );
      iterator end( this->end() );

      float max(0);
      float mag(0);

      for(; itr!=end;++itr )
	{
	  mag = itr->magSqrd();

	  if( mag > max ) max = mag; 
	}

      m_radiusSqrd = max;

      return;
    }

  
  void rotate( clip& Clip, const float Angle )
    {
      for_each( Clip.begin(),Clip.end(),std::bind2nd( std::mem_fun_ref( &vec2d::rotate ), Angle) );
	
      return;
    }
 
  void translate( clip& Clip, const vec2d& Position )
    {
      for_each( Clip.begin(),Clip.end(),std::bind2nd( addAssign<vec2d>(), Position ) );
      Clip.center() += Position;

      return;
    }

  void transform( clip& Clip, const float Angle, const vec2d& Position )
    {
      rotate( Clip, Angle );
      translate( Clip, Position );
      
      return;
    }

  const collision collide( const clip& A, const clip& B )
    {
      collision rtn;

      for( size_t lineA(0); lineA < A.lineCount(); ++lineA )
	{
	  for( size_t lineB(0); lineB < B.lineCount(); ++lineB )
	    {
	      rtn = collide( A.line(lineA), B.line(lineB) );

	      if( rtn.result() )
		{
		  return rtn;
		}
	    }
	}
      
      return collision();
    }

  const collision collide( const vec2d& Point, const clip& Clip )
    {

      vec2d endPoint( Point );
      endPoint.x() += 500.0;

      const ray testRay( Point,endPoint );
      size_t count(0);

      for( size_t i(0);i<Clip.lineCount();++i )
	{
	  if( collide( testRay, Clip.line(i) ).result() )
	    {
	      ++count;
	    }
	}
      
      if( (count % 2) == 0 )
	{
	  return collision();
	}
      
      return collision(Point);      
    }

  const clip triangleClip( const float Size )
    {
      std::vector<vec2d> vertex;
      
      vec2d point(0.0,Size);
      vertex.push_back( point );
      point.rotate( M_PI / 1.5 ) ;
      vertex.push_back( point );
      point.rotate( M_PI / 1.5 ) ;
      vertex.push_back( point );

      return clip( vertex );
    }

  const clip shipClip( const float Size )
    {
      std::vector<vec2d> vertex;
      
      vec2d point(0.0,Size);
      vertex.push_back( point );

      point.rotate( M_PI * 0.8 ) ;
      vertex.push_back( point );
      point.rotate( M_PI * 0.4 ) ;
      vertex.push_back( point );

      return clip( vertex );
    }
 
  const clip rockClip( const float Radius, const size_t nSides )
    {
      const float angle( (2.0 * M_PI) / nSides );
      vec2d point;
       
      std::vector<vec2d> vertex;

      for( size_t i(1);i<nSides;++i )
	{
	  point.polar( angle * ((std::rand())/(double)RAND_MAX)  + ( angle * i), 
		       ((std::rand())/(double)RAND_MAX) * 0.3 *Radius + 0.7 * Radius );
	  vertex.push_back( point );
	}
    
      return clip( vertex );
    }

  const clip turretClip()
    {
      const float angle( 0.25 * M_PI ); // 1/8th of 2pi
      const float radius(15.0);
      const float length(5.0);
      vec2d point;

      std::vector<vec2d> vertex;

      // draw gun barrel
      point.polar(0.5*angle,radius);
      vertex.push_back( point );
      point.y() += length;
      vertex.push_back( point );
      point.polar(7.5*angle,radius);
      point.y() += length;
      vertex.push_back( point );
      point.y() -= length;
      vertex.push_back( point );
      
      for( size_t i(0);i<6;++i )
	{
	  vertex.push_back( point.rotate(angle) );
	}

      return clip( vertex );
    }

}
