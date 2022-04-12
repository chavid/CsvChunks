
#ifndef STRONG_KEY_H
#define STRONG_KEY_H

#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <typeinfo>
#include <concepts>


//===================================================
// So to transform any key-like type into a standalone type.
//===================================================

template <std::totally_ordered InternalType, typename TagType>
class StrongKey
 {
  public :
    using StrongKeyInternalType = InternalType ;
    StrongKey()
     : value_{}
     {}
    explicit StrongKey( InternalType value )
     : value_(value)
     {}
    InternalType value() const
     { return value_ ; }
    InternalType & value()
     { return value_ ; }
    explicit operator bool()
     { return (value_?true:false) ; }
    bool operator==( StrongKey const & i ) const
     { return value_==i.value_ ; }
    std::strong_ordering operator<=>( StrongKey const & i ) const
     { return value_<=>i.value_ ; }
//    bool operator!=( StrongKey const & i ) const
//     { return value_!=i.value_ ; }
//    bool operator<( StrongKey const & i ) const
//     { return value_<i.value_ ; }
  private :
    InternalType value_ ;
 } ;


template <typename InternalType, typename TagType>
std::ostream & operator<<( std::ostream & os, StrongKey<InternalType,TagType> i )
 { return (os<<i.value()) ; }

template <typename InternalType, typename TagType>
std::istream & operator>>( std::istream & is, StrongKey<InternalType,TagType> & i )
 { return (is>>i.value()) ; }

//template <std::totally_ordered InternalType, typename TagType>
//bool operator<( const StrongKey<InternalType,TagType> & i1, const StrongKey<InternalType,TagType> & i2 )
// { return (i1.value()<i2.value()) ; }
//
//template <std::totally_ordered InternalType, typename TagType>
//bool operator>( const StrongKey<InternalType,TagType> & i1, const StrongKey<InternalType,TagType> & i2 )
// { return (i1.value()>i2.value()) ; }
//
//template <std::totally_ordered InternalType, typename TagType>
//bool operator<=( const StrongKey<InternalType,TagType> & i1, const StrongKey<InternalType,TagType> & i2 )
// { return (i1.value()<=i2.value()) ; }
//
//template <std::totally_ordered InternalType, typename TagType>
//bool operator>=( const StrongKey<InternalType,TagType> & i1, const StrongKey<InternalType,TagType> & i2 )
// { return (i1.value()>=i2.value()) ; }


#endif
