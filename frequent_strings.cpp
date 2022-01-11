
#include "frequent_strings.h"
#include "strong_int.h"
#include <string_view>
#include <string>
#include <iostream>

using Columns = Glossary<struct ColumnsFoo> ;
Columns const COLUMNS("couleur1|color1;couleur2|color2") ;

int main()
 {
  std::string s1 {"COLOR1"} ;
  FrequentString fs1(s1) ;
  FrequentString fs2("COLOR2") ;
  auto fs3 = "Color1"_fs ;

  std::cout<<fs1<<" "<<fs2<<" "<<fs3<<" "<<std::endl ;
  std::cout<<(fs1==fs2)<<" "<<(fs1==fs3)<<" "<<std::endl ;

  auto c1 = COLUMNS("COLOR1"), c2 = COLUMNS("Couleur2") ;
  std::cout<<c1<<" "<<COLUMNS("Couleur2")<<std::endl ;
  std::cout<<COLUMNS(c1)<<" "<<COLUMNS(c2)<<std::endl ;

  return 0 ;
 }