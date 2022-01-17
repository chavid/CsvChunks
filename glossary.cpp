
#include "glossary.h"
#include <string_view>
#include <string>
#include <sstream>

using Columns = Glossary<struct ColumnsFoo> ;
Columns const COLUMNS("couleur1|color1;couleur2|color2") ;
Columns::Id C1 = COLUMNS.id("couleur1") ;
Columns::Id C2 = COLUMNS.id("color2") ;

int main()
 {
  auto c1 = COLUMNS.id("COLOR1"), c2 = COLUMNS.id("Couleur2") ;
  std::cout<<c1<<" "<<C2<<std::endl ;
  std::cout<<COLUMNS.str(C1)<<" "<<COLUMNS.str(c2)<<std::endl ;

  return 0 ;
 }