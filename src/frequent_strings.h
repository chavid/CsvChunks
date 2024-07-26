

#ifndef FREQUENT_STRINGS_H
#define FREQUENT_STRINGS_H

#include "static_strings.h"
#include <iostream>
#include <format>


//===================================================
// The class FrequentString wraps the access
// to StaticStrings behind usual objects.
//===================================================

class FrequentString
 {
  public :

    using SSIZE = StaticStrings::String::size_type ;

    static void init() ;

    FrequentString() : ptr_{ StaticStrings::empty_string_ptr() } {}
    explicit FrequentString( std::string_view sv ) : ptr_{ StaticStrings::ptr(sv) } {}
    std::string_view str() const { return StaticStrings::str(ptr_) ; }

    // shortcuts so to avoid the call to str()
    bool empty() const { return (ptr_==StaticStrings::empty_string_ptr()) ; }
    int compare( SSIZE pos, SSIZE count, FrequentString const & fs ) const
     { return str().compare(pos,count,fs.str()) ; }
    std::size_t size() const { return StaticStrings::str(ptr_).size() ; }

    // fast comparison
    //StrongIntInternalType value() const { return ptr_ ; }
    bool operator==( FrequentString const & other ) const { return ptr_==other.ptr_ ; }
    std::strong_ordering operator<=>( FrequentString const & other ) const { return ptr_<=>other.ptr_ ; }

    // to make it usable within a StrongInt ;
    //using StrongIntInternalType = StaticStrings::Ptr ;

  private :

    StaticStrings::Ptr ptr_ ;
 } ;

FrequentString operator"" _fs ( char const * str, std::size_t ) ;

std::ostream & operator<<( std::ostream & os, FrequentString ) ;
std::istream & operator>>( std::istream & is, FrequentString & ) ;

template<>
struct std::formatter<FrequentString> : std::formatter<std::string_view>
 {
    template<class FormatContext>
    auto format( FrequentString fs, FormatContext & fc ) const
     { return std::formatter<std::string_view>::format(fs.str(), fc) ; }
 } ;

#endif

