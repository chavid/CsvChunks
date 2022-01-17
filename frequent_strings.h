
#ifndef FREQUENT_STRINGS_H
#define FREQUENT_STRINGS_H

#include "strong_int.h"
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

//===================================================
// Replace frequently used strings with a rank
// within a static table. The strings are made
// lowercase, so that comparisons of the ranks are 
// case insensitive.
//===================================================

class StaticStrings
 {
  public :

    using Collection = std::vector<std::string> ;
    using Indice = Collection::size_type  ;
    using Rank = StrongInt<Indice,struct StringsFoo> ;

    static Rank rank( std::string_view ) ;
    static const std::string & str( Rank rank )
     { return strings_[rank.value()] ; }
    
  private:
   
    static Collection strings_ ;
    static std::unordered_map<std::string,Rank> ranks_ ;
 } ;

//===================================================
// The class FrequentString encapsulates the access
// to the static table behind usual string behavior.
//===================================================

class FrequentString
 {
  public :
    FrequentString() : rank_{} {}
    explicit FrequentString( std::string_view sv ) : rank_{ StaticStrings::rank(sv) } {}
    bool empty() const { return (rank_==StaticStrings::Rank(0)) ; }
    std::string const & str() const { return StaticStrings::str(rank_) ; }
    std::string::size_type size() const { return StaticStrings::str(rank_).size() ; }
    bool operator==( FrequentString const & other ) const { return rank_==other.rank_ ; }
    bool operator!=( FrequentString const & other ) const { return rank_!=other.rank_ ; }
    bool operator<( FrequentString const & other ) const { return rank_<other.rank_ ; }
    // to make it usable within a StrongInt ;
    using StrongIntInternalType = StaticStrings::Rank ;
    StrongIntInternalType value() const
     { return rank_ ; }
  private :
    StaticStrings::Rank rank_ ;
 } ;

FrequentString operator"" _fs ( const char * str, std::size_t ) ;
std::ostream & operator<<( std::ostream & os, FrequentString ) ;
std::istream & operator>>( std::istream & is, FrequentString & ) ;

#endif
