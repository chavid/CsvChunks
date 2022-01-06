
#include "frequent_strings.h"
#include <string_view>
#include <string>
#include <iostream>

int main()
 {
  std::string s1 {"COLOR1"} ;
  auto r1 = Strings::rank(s1) ;
  auto r2 = Strings::rank("COLOR2") ;
  auto r3 = Strings::rank("Color1") ;

  std::cout<<r1<<" "<<r2<<" "<<r3<<" "<<std::endl ;
  std::cout<<(r1==r2)<<" "<<(r1==r3)<<" "<<std::endl ;

  return 0 ;
 }