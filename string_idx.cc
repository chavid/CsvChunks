
#include "util/constantes.h"
#include <sstream>
#include <algorithm>

Constantes::Collection Constantes::constantes_({"rien"}) ;

static std::string lower( const std::string & str )
 {
  std::string res(str) ;
  std::transform(res.begin(),res.end(),res.begin(),
   [](unsigned char c){ return std::tolower(c) ; } ) ;
  return res ;
 }

Constantes::Rang Constantes::rang( const std::string & chaine )
 {
  std::string lower_chaine = chaine.empty()?"rien":lower(chaine) ;
  auto nc = constantes_.size() ;
  decltype(nc) ic ;
  for ( ic = 0 ; ic<nc ; ++ic )
  if (constantes_[ic]==lower_chaine)
    return Rang(ic) ;
  constantes_.push_back(lower_chaine) ;
  return Rang(nc) ;
 }

std::ostream & operator<<( std::ostream & os, Constantes::Rang rang )
 { return (os<<Constantes::chaine(rang)) ; }
