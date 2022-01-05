
#ifndef CONSTANTES_H
#define CONSTANTES_H

#include "util/type_fort.h"
#include <string>
#include <vector>
#include <iostream>

class Constantes
 {
  public :

    using Collection = std::vector<std::string> ;
    using Indice = Collection::size_type  ;
    using Rang = TypeFort<Indice,struct ConstantesFoo> ;

    static Rang rang( const std::string & ) ;
    static const std::string & chaine( Rang rang )
     { return constantes_[rang.valeur()] ; }
    
  private:
   
    static Collection constantes_ ;
 } ;

std::ostream & operator<<( std::ostream &, Constantes::Rang ) ;

#endif
