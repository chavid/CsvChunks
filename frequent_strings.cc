

#include "frequent_strings.h"
#include <sstream>
#include <algorithm>


//===================================================
// StaticStrings
//===================================================

std::pmr::polymorphic_allocator<char> * StaticStrings::char_allocator_p_ ;
StaticStrings::Collection StaticStrings::strings_ ;
StaticStrings::Rank StaticStrings::empty_string_rank_ ;

void StaticStrings::init()
 {
  static std::array<std::byte,1024*1024> buffer ;
  static std::pmr::monotonic_buffer_resource mem_resource { buffer.data(), buffer.size() } ;
  static std::pmr::polymorphic_allocator<char> char_allocator { &mem_resource } ;
  char_allocator_p_ = & char_allocator ;

  // I want the empty string to be already here, with a well known
  // rank than can be used so to speed up empty testing and output
  auto [ itr, res ] = strings_.emplace("",*char_allocator_p_) ;
  empty_string_rank_ = Rank{&*itr} ;
 }

std::size_t StaticStrings::size()
 {
  std::size_t total {} ;
  for ( auto const & str : strings_ )
   { total += str.size()+1 ; }
  return total ;
 }

static void upper( std::string & str )
 {
  std::transform(str.begin(),str.end(),str.begin(),
   [](unsigned char c){ return std::toupper(c) ; } ) ;
 }

StaticStrings::Rank StaticStrings::rank( std::string_view sv )
 {
  if (sv.empty()) { return empty_string_rank_ ; }
  std::string str { sv } ;
  upper(str) ;
  auto [ itr, res ] = strings_.emplace(str,*char_allocator_p_) ;
  return Rank{&*itr} ;
 }

//===================================================
// FrequentString
//===================================================

FrequentString operator"" _fs ( char const * str, std::size_t )
 { return FrequentString(str) ; }

std::ostream & operator<<( std::ostream & os, FrequentString fs )
 { return (os<<fs.str()) ; }

std::istream & operator>>( std::istream & is, FrequentString & fs )
 {
  std::string str ;
  if (is>>str) fs = FrequentString(str) ;
  return is ;
 }


