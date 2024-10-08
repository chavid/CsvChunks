

#include "static_strings.h"
#include <sstream>
#include <algorithm>
#include <array>


//std::pmr::polymorphic_allocator<char> * StaticStrings::char_allocator_p_ ;
StaticStrings::Collection StaticStrings::strings_({""}) ;
std::unordered_map<StaticStrings::String,StaticStrings::Rank> StaticStrings::ranks_ = { { "", StaticStrings::Rank(0) } } ;
//StaticStrings::Ptr StaticStrings::empty_string_ptr_ ;
//bool StaticStrings::init_done_ { false } ;
//
//void StaticStrings::init()
// {
//  if (init_done_) return ;
//  init_done_ = true ;
//  
//  static std::array<std::byte,1024*1024> buffer ;
//  static std::pmr::monotonic_buffer_resource mem_resource { buffer.data(), buffer.size() } ;
//  static std::pmr::polymorphic_allocator<char> char_allocator { &mem_resource } ;
//  char_allocator_p_ = & char_allocator ;
//
//  //// I want the empty string to be already here, with a well known
//  //// rank than can be used so to speed up empty testing and output
//  //auto [ itr, res ] = strings_.emplace("",*char_allocator_p_) ;
//  //empty_string_ptr_ = Ptr{&*itr} ;
// }
//
//std::size_t StaticStrings::size()
// {
//  std::size_t total {} ;
//  for ( auto const & str : strings_ )
//   { total += str.size()+1 ; }
//  return total ;
// }

static std::string remove_spaces( std::string_view sv )
 {
  std::string str ;
  str.reserve(sv.size()) ;
  std::copy_if(sv.begin(),sv.end(),std::back_inserter(str),
   [](unsigned char c){ return !std::isspace(c) ; }) ;
  return str ;
 }

static void upper( std::string & str )
 {
  std::transform(str.begin(),str.end(),str.begin(),
   [](unsigned char c){ return std::toupper(c) ; } ) ;
 }

StaticStrings::Rank StaticStrings::rank( std::string_view sv )
 {
  if (sv.empty()) { return Rank(0) ; }
  std::string str { remove_spaces(sv) } ;
  upper(str) ;
  auto [ itr, res ] = ranks_.insert({str,StaticStrings::Rank(strings_.size())}) ;
  if (res) strings_.emplace_back(std::move(str)) ;
  return (itr->second) ;
 }

