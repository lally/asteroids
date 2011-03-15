#include "common.h"

exception::exception( const std::string& Arg ):
  m_msg( Arg )
{}

exception::~exception() throw()
{}
