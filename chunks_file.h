
#ifndef FichierTable_h
#define FichierTable_h 1

//#include "data/matieres.h"

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#include <iomanip>

class FichierTable
 {
  public :

    FichierTable( bool debug =false ) ;
    bool ouvrir
     ( const std::string & nom,
       const std::string & option = "READ" ) ;
    operator void *() ;
    bool fermer() ;
    ~FichierTable() ;

    // Interface de lecture
    bool nouvelle_section() ;
    const std::string & section_nom() { return section_nom_ ; }
    const std::string & section_variante() { return section_variante_ ; }
    const std::string & section_version() { return section_version_ ; }
    const std::vector<std::string> & section_colonnes() { return section_colonnes_ ; }
    void verifie_colonnes( const std::string & colonnes ) const ;
    bool nouvelle_ligne() ;
    template <typename T>
    friend FichierTable & operator>>( FichierTable &, T & var ) ;

    // Interface d'écriture
    void section_nom( const std::string & nom ) { section_nom_ = nom ; }
    void section_variante( const std::string & variante ) { section_variante_ = variante ; }
    void section_version( const std::string & version ) { section_version_ = version ; }
    void section_colonnes( const std::vector<std::string> & colonnes ) { section_colonnes_ = colonnes ; }
    void section_colonnes( const std::string & colonnes ) ;
    void nouveau_format( const std::vector<std::size_t> & largeurs ) ;
    void section_write() ;
    void efface_format() ;
    void saut_de_ligne() ;
    template <typename T>
    friend FichierTable & operator<<( FichierTable &, T var ) ;

  private :

    bool is_ok_ ;
    bool debug_ ;

    std::string nom_ ;
    std::string option_ ;

    // section courante
    std::string section_nom_ ;
    std::string section_variante_ ;
    std::string section_version_ ;
    std::vector<std::string> section_colonnes_ ;

    std::ofstream ofile_ ;
    std::vector<std::size_t>::size_type indice_courant_ = 0 ;
    std::vector<std::size_t> largeurs_ ;

    std::ifstream ifile_ ;
    std::istringstream iline_ ;
    bool iline_prete_ = false ;
    std::string icell_ ;
    bool is_eol_ ;
    bool is_eof_ ;

    bool prepare_extraction() ;
    bool prepare_ligne() ;

    bool pret_a_lire() ;   // cannot be const because of is_open()
    bool pret_a_ecrire() ; // which is not const with gcc 3.4.6
 } ;


//========================================================
// templates
//========================================================

template <typename T>
FichierTable & operator>>( FichierTable & ft, T & var )
 {
  if (ft.prepare_extraction())
   {
    std::istringstream iss(ft.icell_) ;
    iss>>var ;
   }
  return ft ;
 }

template <>
FichierTable & operator>>< std::string >( FichierTable &, std::string & var ) ;

template <>
FichierTable & operator>>< bool >( FichierTable &, bool & var ) ;

template <>
FichierTable & operator>>< std::pair<int,int> >( FichierTable &, std::pair<int,int> & var ) ;

template <typename T>
FichierTable & operator<<( FichierTable & ft, T var )
 {
  if ( (ft.option_!="WRITE") || ! ft.pret_a_ecrire() )
   { return ft ; }
  if (ft.largeurs_.size()==0)
   { ft.ofile_<<var ; }
  else
   {
    std::ostringstream oss ;
    oss<<var ;
    ft.ofile_<<" " ;
    if (ft.indice_courant_<ft.largeurs_.size())
     { ft.ofile_<<std::setw(ft.largeurs_[ft.indice_courant_]) ; }
    ft.ofile_<<oss.str()<<" ;" ;
    ft.indice_courant_++ ;
   }
  return ft ;
 }

#endif
