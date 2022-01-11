
#ifndef STRONG_INT_H
#define STRONG_INT_H

#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <typeinfo>
#include <concepts>


//===================================================
// Concept of what is allowed as StrongInt internal :
// any integral type, or another StrongInt.
//===================================================

template <typename T>
concept StrongIntInstance = requires( T )
 { typename T::StrongIntInternalType ; } ;

template <typename T>
concept StrongIntInternal = std::integral<T> || StrongIntInstance<T> ;


//===================================================
// So to transform any index-like integer into
// a standalone type,
//===================================================

template <StrongIntInternal InternalType, typename TagType>
class StrongInt
 {
  public :
    using StrongIntInternalType = InternalType ;
    StrongInt()
     : value_{}
     {}
    explicit StrongInt( InternalType value )
     : value_(value)
     {}
    InternalType value() const
     { return value_ ; }
    InternalType & value()
     { return value_ ; }
    explicit operator bool()
     { return (value_?true:false) ; }
    bool operator==( const StrongInt & i ) const
     { return value_==i.value_ ; }
    bool operator!=( const StrongInt & i ) const
     { return value_!=i.value_ ; }
    bool operator<( const StrongInt & i ) const
     { return value_<i.value_ ; }
    StrongInt & operator++()
     { value_++ ; return *this ; }
    StrongInt operator++( int )
     { StrongInt copy(*this) ; ++*this ; return copy ; }
  private :
    InternalType value_ ;
 } ;


template <StrongIntInternal InternalType, typename TagType>
std::ostream & operator<<( std::ostream & os, StrongInt<InternalType,TagType> i )
 { return (os<<i.value()) ; }

template <StrongIntInternal InternalType, typename TagType>
std::istream & operator>>( std::istream & is, StrongInt<InternalType,TagType> & i )
 { return (is>>i.value()) ; }

template <StrongIntInternal InternalType, typename TagType>
bool operator<( const StrongInt<InternalType,TagType> & i1, const StrongInt<InternalType,TagType> & i2 )
 { return (i1.value()<i2.value()) ; }

template <StrongIntInternal InternalType, typename TagType>
bool operator>( const StrongInt<InternalType,TagType> & i1, const StrongInt<InternalType,TagType> & i2 )
 { return (i1.value()>i2.value()) ; }

template <StrongIntInternal InternalType, typename TagType>
bool operator<=( const StrongInt<InternalType,TagType> & i1, const StrongInt<InternalType,TagType> & i2 )
 { return (i1.value()<=i2.value()) ; }

template <StrongIntInternal InternalType, typename TagType>
bool operator>=( const StrongInt<InternalType,TagType> & i1, const StrongInt<InternalType,TagType> & i2 )
 { return (i1.value()>=i2.value()) ; }


//===================================================
// So to transform any index-like integer into
// a standalone type,
//===================================================

#endif
