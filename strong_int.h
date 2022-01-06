
#ifndef STRONG_INT_H
#define STRONG_INT_H

#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <typeinfo>
#include <concepts>


//===================================================
// So to transform any index-like integer into
// a standalone type,
//===================================================


template <std::integral IntegralType, typename TagType>
class StrongInt
 {
  public :
    //using IntegralType = IntegralType ;
    StrongInt()
     : value_{}
     {}
    explicit StrongInt( IntegralType value )
     : value_(value)
     {}
    IntegralType value() const
     { return value_ ; }
    IntegralType & value()
     { return value_ ; }
    explicit operator bool()
     { return (value_?true:false) ; }
    bool operator==( const StrongInt & i ) const
     { return value_==i.value_ ; }
    bool operator!=( const StrongInt & i ) const
     { return value_!=i.value_ ; }
  private :
    IntegralType value_ ;
 } ;


template <std::integral IntegralType, typename TagType>
std::ostream & operator<<( std::ostream & os, StrongInt<IntegralType,TagType> i )
 { return (os<<i.value()) ; }

template <std::integral IntegralType, typename TagType>
std::istream & operator>>( std::istream & is, StrongInt<IntegralType,TagType> & i )
 { return (is>>i.value()) ; }

template <std::integral IntegralType, typename TagType>
bool operator<( const StrongInt<IntegralType,TagType> & i1, const StrongInt<IntegralType,TagType> & i2 )
 { return (i1.value()<i2.value()) ; }

template <std::integral IntegralType, typename TagType>
bool operator>( const StrongInt<IntegralType,TagType> & i1, const StrongInt<IntegralType,TagType> & i2 )
 { return (i1.value()>i2.value()) ; }

template <std::integral IntegralType, typename TagType>
bool operator<=( const StrongInt<IntegralType,TagType> & i1, const StrongInt<IntegralType,TagType> & i2 )
 { return (i1.value()<=i2.value()) ; }

template <std::integral IntegralType, typename TagType>
bool operator>=( const StrongInt<IntegralType,TagType> & i1, const StrongInt<IntegralType,TagType> & i2 )
 { return (i1.value()>=i2.value()) ; }

#endif
