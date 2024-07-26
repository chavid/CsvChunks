

#include "static_strings.h"
#include <sstream>
#include <algorithm>
#include <array>


std::pmr::polymorphic_allocator<char> * StaticStrings::char_allocator_p_ ;
StaticStrings::Collection StaticStrings::strings_ ;
StaticStrings::Ptr StaticStrings::empty_string_ptr_ ;
bool StaticStrings::init_done_ { false } ;

void StaticStrings::init()
 {
  if (init_done_) return ;
  init_done_ = true ;
  
  static std::array<std::byte,1024*1024> buffer ;
  static std::pmr::monotonic_buffer_resource mem_resource { buffer.data(), buffer.size() } ;
  static std::pmr::polymorphic_allocator<char> char_allocator { &mem_resource } ;
  char_allocator_p_ = & char_allocator ;

  // I want the empty string to be already here, with a well known
  // rank than can be used so to speed up empty testing and output
  auto [ itr, res ] = strings_.emplace("",*char_allocator_p_) ;
  empty_string_ptr_ = Ptr{&*itr} ;
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

StaticStrings::Ptr StaticStrings::ptr( std::string_view sv )
 {
  if (sv.empty()) { return empty_string_ptr_ ; }
  std::string str { sv } ;
  upper(str) ;
  auto [ itr, res ] = strings_.emplace(str,*char_allocator_p_) ;
  return Ptr{&*itr} ;
 }

