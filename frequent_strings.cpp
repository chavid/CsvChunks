
#include "frequent_strings.h"
#include "strong_int.h"
#include <string_view>
#include <string>
#include <sstream>
#include <format>

int main()
 {
  StaticStrings::init() ;

  std::string s1 {"COLOR1"} ;
  FrequentString fs1(s1) ;

  std::istringstream iss{"COLOR2"} ;
  FrequentString fs2 ;
  iss>>fs2 ;
  
  auto fs3 = "Color1"_fs ;

  std::cout<<std::format("{} {} {} ",fs1,fs2,fs3)<<std::endl ;
  std::cout<<std::format("{} {} ",(fs1==fs2),(fs1==fs3))<<std::endl ;

  return 0 ;
 }