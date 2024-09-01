

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

    FrequentString() : rank_{ StaticStrings::Rank{0} } {}
    explicit FrequentString( std::string_view sv ) : rank_{ StaticStrings::rank(sv) } {}
    std::string_view str() const { return StaticStrings::str(rank_) ; }

    // shortcuts so to avoid the call to str()
    bool empty() const { return (rank_==StaticStrings::Rank{0}) ; }
    int compare( SSIZE pos, SSIZE count, FrequentString const & fs ) const
     { return str().compare(pos,count,fs.str()) ; }
    std::size_t size() const { return StaticStrings::str(rank_).size() ; }

    // fast comparison
    auto rank() const { return rank_ ; }
    bool operator==( FrequentString const & other ) const { return rank_==other.rank_ ; }
    std::strong_ordering operator<=>( FrequentString const & other ) const { return rank_<=>other.rank_ ; }

    // to make it usable within a StrongInt ;
    //using StrongIntInternalType = StaticStrings::Rank ;

  private :

    StaticStrings::Rank rank_ ;
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

// so to make it usable in a std::unordered_set or std::unordered_map
template<>
struct std::hash<FrequentString>
 {
  auto operator()(const FrequentString & fs) const noexcept
   { return fs.rank().value() ; }
 } ;

#endif

