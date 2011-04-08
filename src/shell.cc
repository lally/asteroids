/***************************************************************************
 *            Shell.cxx
 *
 *  Tue Mar 15 19:49:45 2005
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

#include "shell.h"
#include "lock.h"
#include <pthread.h>
//<-- shell class -->


static int s_shell_cnt;
static pthread_mutex_t s_shellcnt_lock = PTHREAD_MUTEX_INITIALIZER;

shell::shell(const vec2d& Position, const vec2d& Velocity ):
  particle( Position,Velocity,2.0 ),
     m_range( 450.0 ),
     m_travel(0.0),
     m_updateTime(physics::runTime::create()->now())
{
  Lock m(s_shellcnt_lock);
  s_shell_cnt++;
}

shell::shell( const shell& Arg ):
  particle( Arg ),
     m_range( Arg.m_range ),
     m_travel( Arg.m_travel ),
     m_updateTime( Arg.m_updateTime )
{
  Lock m(s_shellcnt_lock);
  s_shell_cnt++;
}

shell::~shell()
{
  Lock m(s_shellcnt_lock);
  s_shell_cnt--;
}

int shell::shellCount() {
  Lock m(s_shellcnt_lock);
  return s_shell_cnt;
}

const shell& shell::operator=( const shell& Arg )
{
  this->particle::operator=( Arg );
  
  this->m_range      = Arg.m_range;
  this->m_travel     = Arg.m_travel;
  this->m_updateTime = Arg.m_updateTime;

  return *this;
}

void shell::update()
{	
  physics::time_t now( physics::runTime::create()->now() );
  vec2d displacement( this->velocity() * (now - m_updateTime) );

  this->translate( displacement );
 
  m_travel += displacement.magnitude();
  
  if( m_travel > m_range )
    {
      this->destroy();
    }
	
  m_updateTime = now;
	
  return;
}

void shell::destroy()
{
  this->item::destroy();

  return;
}


void shell::draw()
{
  graphics::drawPoint( this->position(), this->radius(), 0x10, 0x10, 0x00 );

  return;
}

void shell::draw( const vec2d& Position )
{
  graphics::drawPoint( Position, this->radius(), 0x10, 0x10, 0x00 );

  return;
}
