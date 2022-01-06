
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
  std::string lower_str = sv.empty()?"empty":lower(sv) ;
  auto nc = strings_.size() ;
  decltype(nc) ic ;
  for ( ic = 0 ; ic<nc ; ++ic )
  if (strings_[ic]==lower_str)
    return Rank(ic) ;
  strings_.push_back(lower_str) ;
  return Rank(nc) ;
 }

std::ostream & operator<<( std::ostream & os, Strings::Rank rank )
 { return (os<<Strings::string(rank)) ; }
