
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

static std::string lower( std::string_view sv )
 {
  std::string res(sv) ;
  std::transform(res.begin(),res.end(),res.begin(),
   [](unsigned char c){ return std::tolower(c) ; } ) ;
  return res ;
 }

StaticStrings::Rank StaticStrings::rank( std::string_view sv )
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

//===================================================
// FrequentString
//===================================================

FrequentString operator"" _fs ( const char * str, std::size_t )
 { return FrequentString(str) ; }

std::ostream & operator<<( std::ostream & os, FrequentString str )
 { return (os<<str.str()) ; }

//===================================================
// Other utilities
//===================================================

std::vector<std::set<FrequentString>> fs_parse_line( std::string_view columns )
 {
  std::vector<std::set<FrequentString>> res1 ;
  std::istringstream iss1{std::string{columns}} ;
  std::string column ;
  while (std::getline(iss1,column,';'))
   {
    std::set<FrequentString> res2 ;
    std::istringstream iss2{column} ;
    std::string alias ;
    while (std::getline(iss2,alias,'|'))
     { res2.insert(FrequentString{alias}) ; }
    res1.push_back(res2) ;
   }
  return res1 ;
 }

std::vector<std::set<FrequentString>>::size_type fs_find( std::vector<std::set<FrequentString>> const & columns, std::string_view column )
 {
  std::vector<std::set<FrequentString>>::size_type res = 0 ;
  while (res<columns.size())
  if (columns[res].contains(column))
   { return res ; }
  else
   { ++res ; }
  return columns.size() ;
 }