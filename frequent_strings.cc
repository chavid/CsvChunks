
// TODO : review the implementation so to fasten
// Strings::rank. Even better : for some values
// known in advance, preprocess them at compile time ?

#include "frequent_strings.h"
#include <sstream>
#include <algorithm>

Strings::Collection Strings::strings_({"empty"}) ;

static std::string lower( std::string_view sv )
 {
  std::string res(sv) ;
  std::transform(res.begin(),res.end(),res.begin(),
   [](unsigned char c){ return std::tolower(c) ; } ) ;
  return res ;
 }

Strings::Rank Strings::rank( std::string_view sv )
 {
  if (sv.empty()) { return Rank(0) ; }
  std::string lower_str = lower(sv) ;
  auto nc = strings_.size() ;
  decltype(nc) ic ;
  for ( ic = 0 ; ic<nc ; ++ic )
  if (strings_[ic]==lower_str)
   { return Rank(ic) ; }
  strings_.push_back(lower_str) ;
  return Rank(nc) ;
 }

FrequentString make_string( std::string_view str )
 { return Strings::rank(str) ; }

FrequentString operator"" _fq ( const char * str, std::size_t )
 { return Strings::rank(str) ; }

std::ostream & operator<<( std::ostream & os, FrequentString str )
 { return (os<<Strings::string(str)) ; }

namespace std
 {
   bool empty( FrequentString str )
    { return (str==FrequentString(0)) ; }
   std::string::size_type size( FrequentString str )
    { return Strings::string(str).size() ; }
 }
