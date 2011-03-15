#ifndef COMMON_CODE
#define COMMON_CODE

// Copyright Nick Brett 2007

#include <algorithm>
#include <functional>
#include <boost/shared_ptr.hpp>

#include <iostream>
#include <string>
#include <exception>

class exception : public std::exception
{
 public:
  exception( const std::string& );
  virtual ~exception() throw();

  virtual const char* what() const throw()
    {
      return m_msg.c_str();
    }

 private:
  std::string m_msg;
};


template < typename Ta,typename Tr > class mem_fun_ptr : public std::unary_function<boost::shared_ptr<Ta>,Tr>
{
 public:
  mem_fun_ptr( Tr(Ta::*Arg)() ):
    m_functionPtr(Arg)
    {}
  
  Tr operator()( const boost::shared_ptr<Ta>& Arg )
  {
    return ((*Arg).*m_functionPtr)();
  }

 private:
  Tr(Ta::*m_functionPtr)();

};

template < typename Ta,typename Tr > class const_mem_fun_ptr : public std::unary_function<boost::shared_ptr<Ta>,const Tr>
{
 public:
  const_mem_fun_ptr( const Tr (Ta::*Arg)() const ):
    m_functionPtr(Arg)
    {}
  
  const Tr operator()( const boost::shared_ptr<Ta>& Arg )
  {
    return ((*Arg).*m_functionPtr)();
  }

 private:
  const Tr (Ta::*m_functionPtr)() const;

};

template< typename T > struct addAssign : public std::binary_function< T, const T, T >
{
  const T& operator()( T& Lhs, const T& Rhs ) const
  {
    return Lhs += Rhs;
  }
};


template< typename T > struct subtractAssign : public std::binary_function< T, const T, T >
{
  const T& operator()( T& Lhs, const T& Rhs ) const
  {
    return Lhs -= Rhs;
  }
};


template< typename T > struct callDraw : public std::unary_function< boost::shared_ptr<T>&, void >
{
  void operator()( boost::shared_ptr<T>& Arg ) const
  {
    Arg->draw();
    return;
  }
};

template< typename T > struct destroyed : public std::unary_function< boost::shared_ptr<T>&, const bool >
{
  const bool operator()( boost::shared_ptr<T>& Arg ) const
  {
    return Arg->destroyed();
  }
};

template< typename T > struct callUpdate : public std::unary_function< boost::shared_ptr<T>&, void >
{
  void operator()( boost::shared_ptr<T>& Arg ) const
  {
    Arg->update();
    return;
  }
};



#endif
