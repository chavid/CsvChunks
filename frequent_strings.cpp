
#include "frequent_strings.h"
#include <string_view>
#include <string>
#include <iostream>

int main()
 {
  std::string s1 {"COLOR1"} ;
  FrequentString fs1(s1) ;
  FrequentString fs2("COLOR2") ;
  auto fs3 = "Color1"_fs ;

  std::cout<<fs1<<" "<<fs2<<" "<<fs3<<" "<<std::endl ;
  std::cout<<(fs1==fs2)<<" "<<(fs1==fs3)<<" "<<std::endl ;

  std::vector<std::set<FrequentString>> columns = fs_parse_line("couleur1|color1;couleur2|color2") ;
  std::cout<<fs_find(columns,"COLOR1")<<" "<<fs_find(columns,"Couleur2")<<std::endl ;

  return 0 ;
 }