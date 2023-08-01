
#include "glossary.h"

// Glossary
using Columns = Glossary<struct ColumnsFoo> ;
Columns const COLUMNS("couleur1|color1;couleur2|color2") ;

// Enum
using FiliereEnum = Enum<struct FiliereTag> ;
template<> FiliereEnum::EnumGlossary FiliereEnum::glo__ {} ;
FiliereEnum const FILIERE_MP { "MP", true } ;
FiliereEnum const FILIERE_PC { "PC", true } ;

int main()
 {
  auto c1 = COLUMNS.id("COLOR1"), c2 = COLUMNS.id("Couleur2") ;
  std::cout<<c1<<" "<<c2<<std::endl ;
  std::cout<<COLUMNS.str(c1)<<" "<<COLUMNS.str(c2)<<std::endl ;

  FiliereEnum f1, f2, f3 ;
  std::istringstream iss ("mp Pc PSI") ;
  iss>>f1>>f2 ; //>>f3 ;
  std::cout<<f1<<" "<<f2<<std::endl ;
  std::cout<<std::format("{} {}",FILIERE_MP,FILIERE_PC)<<std::endl ;
  std::cout<<(f1==FILIERE_MP)<<" "<<(FILIERE_PC==f2)<<std::endl ;

  return 0 ;
 }