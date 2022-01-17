
// TODO : review the implementation so to fasten
// Strings::rank. Even better : for some values
// known in advance, preprocess them at compile time ?

#include "frequent_strings.h"
#include <sstream>
#include <algorithm>


//===================================================
// StaticStrings
//===================================================

StaticStrings::Collection StaticStrings::strings_({"empty"}) ;

std::unordered_map<std::string,StaticStrings::Rank> StaticStrings::ranks_ = { { "empty", StaticStrings::Rank(0) } } ;

static void lower( std::string & str )
 {
  std::transform(str.begin(),str.end(),str.begin(),
   [](unsigned char c){ return std::tolower(c) ; } ) ;
 }

StaticStrings::Rank StaticStrings::rank( std::string_view sv )
 {
  if (sv.empty()) { return Rank(0) ; }
  std::string str { sv } ;
  lower(str) ;
  auto [ itr, res ] = ranks_.insert({str,StaticStrings::Rank(strings_.size())}) ;
  if (res) strings_.push_back(str) ;
  return (itr->second) ;
//
//
//
//  auto nc = strings_.size() ;
//  decltype(nc) ic ;
//  for ( ic = 0 ; ic<nc ; ++ic )
//  if (strings_[ic]==lower_str)
//   { return Rank(ic) ; }
//  strings_.push_back(lower_str) ;
//  return Rank(nc) ;
 }

//===================================================
// FrequentString
//===================================================

FrequentString operator"" _fs ( const char * str, std::size_t )
 { return FrequentString(str) ; }

std::ostream & operator<<( std::ostream & os, FrequentString fs )
 { return (os<<fs.str()) ; }

std::istream & operator>>( std::istream & is, FrequentString & fs )
 {
  std::string str ;
  if (is>>str) fs = FrequentString(str) ;
  return is ;
 }


