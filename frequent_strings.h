
#ifndef FREQUENT_STRINGS_H
#define FREQUENT_STRINGS_H

#include "strong_int.h"
#include <string>
#include <string_view>
#include <vector>
#include <iostream>

//===================================================
// Replace frequently used strings with a rank
// within a static table. The strings are made
// lowercase, so that comparisons of the ranks are 
// case insensitive.
//===================================================

class Strings
 {
  public :

    using Collection = std::vector<std::string> ;
    using Indice = Collection::size_type  ;
    using Rank = StrongInt<Indice,struct StringsFoo> ;

    static Rank rank( std::string_view ) ;
    static const std::string & string( Rank rank )
     { return strings_[rank.value()] ; }
    
  private:
   
    static Collection strings_ ;
 } ;

using FrequentString = Strings::Rank ;

FrequentString make_string( std::string_view ) ;
FrequentString operator"" _fq ( const char * str, std::size_t ) ;

std::ostream & operator<<( std::ostream & os, FrequentString ) ;


namespace std
 {
   bool empty( FrequentString ) ;
   std::string::size_type size( FrequentString ) ;
 }

#endif
