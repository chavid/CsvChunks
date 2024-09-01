
#ifndef STRONG_INT_H
#define STRONG_INT_H

#include <iostream>
#include <format>
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
concept strong_int = requires( T )
 { typename T::StrongIntInternalType ; } ;

template <typename T>
concept strong_int_internal = std::integral<T> || strong_int<T> ;


//===================================================
// So to transform any index-like integer into a standalone type.
//===================================================

template <strong_int_internal InternalType, typename TagType>
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
    bool operator==( StrongInt const & i ) const
     { return value_==i.value_ ; }
    //bool operator!=( StrongInt const & i ) const
    // { return value_!=i.value_ ; }
    std::strong_ordering operator<=>( StrongInt const & i ) const
     { return value_<=>i.value_ ; }
    StrongInt & operator++()
     { value_++ ; return *this ; }
    StrongInt operator++( int )
     { StrongInt copy(*this) ; ++*this ; return copy ; }
    friend StrongInt operator-( StrongInt lhs, StrongInt rhs  )
     { return StrongInt{lhs.value_-rhs.value_} ; }
  private :
    InternalType value_ ;
 } ;


template <strong_int_internal InternalType, typename TagType>
std::ostream & operator<<( std::ostream & os, StrongInt<InternalType,TagType> i )
 { return (os<<i.value()) ; }

template <strong_int_internal InternalType, typename TagType>
std::istream & operator>>( std::istream & is, StrongInt<InternalType,TagType> & i )
 { return (is>>i.value()) ; }

template <strong_int_internal InternalType, typename TagType>
struct std::formatter<StrongInt<InternalType,TagType>> : std::formatter<InternalType>
 {
    template<class FormatContext>
    auto format( StrongInt<InternalType,TagType> i, FormatContext & fc ) const
     { return std::formatter<InternalType>::format(i.value(), fc) ; }
 } ;

//template <strong_int_internal InternalType, typename TagType>
//bool operator<( const StrongInt<InternalType,TagType> & i1, const StrongInt<InternalType,TagType> & i2 )
// { return (i1.value()<i2.value()) ; }
//
//template <strong_int_internal InternalType, typename TagType>
//bool operator>( const StrongInt<InternalType,TagType> & i1, const StrongInt<InternalType,TagType> & i2 )
// { return (i1.value()>i2.value()) ; }
//
//template <strong_int_internal InternalType, typename TagType>
//bool operator<=( const StrongInt<InternalType,TagType> & i1, const StrongInt<InternalType,TagType> & i2 )
// { return (i1.value()<=i2.value()) ; }
//
//template <strong_int_internal InternalType, typename TagType>
//bool operator>=( const StrongInt<InternalType,TagType> & i1, const StrongInt<InternalType,TagType> & i2 )
// { return (i1.value()>=i2.value()) ; }

// so to make it usable in a std::unordered_set or std::unordered_map
template <strong_int_internal InternalType, typename TagType>
struct std::hash<StrongInt<InternalType,TagType>>
 {
  auto operator()(const StrongInt<InternalType,TagType> & si) const noexcept
   { return si.value() ; }
 } ;

#endif
