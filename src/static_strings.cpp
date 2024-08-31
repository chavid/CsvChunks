
#include "static_strings.h"
#include "strong_int.h"
#include <string_view>
#include <string>
#include <sstream>

using namespace std::string_view_literals ;

int main()
 {
  //StaticStrings::init() ;

  std::string s1 {"COLOR1"} ;
  auto s1r { StaticStrings::rank(s1) } ;
  auto s2r { StaticStrings::rank("Color2"sv) } ;
  auto s3r { StaticStrings::rank("Color 1") } ;

  std::cout<<StaticStrings::str(s1r)<<" "<<StaticStrings::str(s2r)<<" "<<StaticStrings::str(s3r)<<" "<<std::endl ;
  std::cout<<(s1r==s2r)<<" "<<(s1r==s3r)<<" "<<std::endl ;

  return 0 ;
 }

