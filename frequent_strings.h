

#ifndef FREQUENT_STRINGS_H
#define FREQUENT_STRINGS_H

#include "strong_key.h"
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <unordered_set>
#include <iostream>
#include <memory_resource>


//===================================================
// Replace frequently used strings with a rank
// within a static table. The strings are made
// uppercase, so that there comparison is
// case insensitive.
//===================================================

class StaticStrings
 {
  public :

    using String = std::pmr::string ;
    using Collection = std::unordered_set<String> ;
    using Indice = String const *  ;
    using Rank = StrongKey<Indice,struct StaticStringsFoo> ;

    static void init() ;
    static std::size_t size() ;
    static Rank rank_empty_string()
     { return empty_string_rank_ ; } 

    static Rank rank( std::string_view ) ;
    static String const & str( Rank rank )
     { return *rank.value() ; }
    
  private:
   
    static std::pmr::polymorphic_allocator<char> * char_allocator_p_ ;
    static Collection strings_ ;
    static Rank empty_string_rank_ ;
 } ;


//===================================================
// The class FrequentString encapsulates the access
// to the static table behind usual string behavior.
//===================================================

class FrequentString
 {
  public :

    using SSIZE = std::pmr::string::size_type ;

    FrequentString() : rank_{} {}
    explicit FrequentString( std::string_view sv ) : rank_{ StaticStrings::rank(sv) } {}
    StaticStrings::String const & str() const { return StaticStrings::str(rank_) ; }

    // shortcuts so to avoid the call to str()
    bool empty() const { return (rank_==StaticStrings::rank_empty_string()) ; }
    int compare( SSIZE pos, SSIZE count, FrequentString const & fs ) const
     { return str().compare(pos,count,fs.str()) ; }
    StaticStrings::String::size_type size() const { return StaticStrings::str(rank_).size() ; }

    // fast comparison
    //StrongIntInternalType value() const { return rank_ ; }
    bool operator==( FrequentString const & other ) const { return rank_==other.rank_ ; }
    std::strong_ordering operator<=>( FrequentString const & other ) const { return rank_<=>other.rank_ ; }
    //bool operator!=( FrequentString const & other ) const { return rank_!=other.rank_ ; }
    //bool operator<( FrequentString const & other ) const { return rank_<other.rank_ ; }

    // to make it usable within a StrongInt ;
    using StrongIntInternalType = StaticStrings::Rank ;

  private :

    StaticStrings::Rank rank_ ;
 } ;

FrequentString operator"" _fs ( char const * str, std::size_t ) ;
std::ostream & operator<<( std::ostream & os, FrequentString ) ;
std::istream & operator>>( std::istream & is, FrequentString & ) ;


#endif

