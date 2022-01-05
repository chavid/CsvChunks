

#include "util/fichier_table.h"
#include "util/afficheur.h"
#include <regex>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>


// ***********************************************************************
//
// ***********************************************************************

FichierTable::FichierTable
 ( bool debug )
 : is_ok_(true), debug_(debug),
   nom_(""), option_(""),
   is_eol_(false),is_eof_(false)
 {}

bool FichierTable::ouvrir
 ( const std::string & nom,
   const std::string & option )
 {
  if (ofile_.is_open()||ifile_.is_open())
   { fermer() ; }
  nom_ = nom ;
  option_ = option ;
  is_ok_ = true ;

  // READ opening
  if (option_=="READ")
   {
    if (debug_)
     { Afficheur::defaut(VX::INFO)<<"ouverture en lecture du fichier "<<nom_<<endl ; }
    ifile_.open(nom_.c_str()) ;
    if (!ifile_.is_open())
     {
      Afficheur::defaut(VX::INFO)<<"pas de fichier "<<nom_<<endl ;
      fermer() ;
      is_ok_ = false ;
      return false ;
     }
    iline_prete_ = false ;
    is_eol_ = false ;
    is_eof_ = false ;
    iline_.clear() ;
    icell_.clear() ;
    return true ;
   }

  // WRITE opening
  else if (option_=="WRITE")
   {
    if (debug_)
     { Afficheur::defaut(VX::INFO)<<"ouverture en ecriture du fichier "<<nom_<<endl ; }
    ofile_.open(nom_.c_str()) ;
    if (!ofile_.is_open())
     {
      Afficheur::defaut(VX::INFO)<<"pas de fichier "<<nom_<<endl ;
      fermer() ;
      is_ok_ = false ;
      return false ;
     }
    ofile_<<std::left ;
    return true ;
   }

  // Unknwon or not-implemented
  else
   {
    fermer() ;
    is_ok_ = false ;
    std::cerr
      <<"[FichierTable::FichierTable] "
      <<"l'option "<<option_<<" n'est pas implantee"
      <<", impossible d'ouvrir le fichier "<<nom_
      <<endl ;
    return false ;
   }
 }

bool FichierTable::fermer()
 {
  ofile_.close() ;
  ifile_.close() ;
  iline_.clear() ;
  icell_.clear() ;
  iline_prete_ = false ;
  is_eol_ = false ;
  is_eof_ = false ;
  is_ok_ = true ;
  nom_ = "" ;
  option_ = "" ;
  return true ;
 }

FichierTable::~FichierTable()
 { fermer() ; }

FichierTable::operator void *()
 {
  if ( ( !is_ok_ ) ||
	   ( (option_=="READ") && ( (!pret_a_lire()) || (!iline_ ) ) ) ||
       ( (option_=="WRITE") && !pret_a_ecrire() ) )
   {
	  return ((void*)0) ;
   }
  return this ;
 }



// ***************************************************************
// READ Interface
// ***************************************************************

bool FichierTable::nouvelle_section()
 {
  // cherche la prochaine entete de section
  while ( prepare_ligne() && (iline_.peek()!='>' ) )
   { iline_prete_ = false ; }
  // verifie qu'il y a bien une entete de section
  if ( iline_.peek()!='>' )
   { return false ; }
  // marque que la ligne a été prise en compte
  iline_prete_ = false ;
  // extrait les informations de section
  // la première extraction sert à éliminer le caractère de section
  // on élimine les éventuels ";" qui trainent
  std::string prompt ;
  section_nom_ = "" ;
  section_variante_ = "" ;
  section_version_ = "" ;
  iline_>>prompt>>section_nom_>>section_variante_>>section_version_ ;
  section_nom_ = std::regex_replace(section_nom_,std::regex(";*$"),"") ;
  section_variante_ = std::regex_replace(section_variante_,std::regex(";*$"),"") ;
  section_version_ = std::regex_replace(section_version_,std::regex(";*$"),"") ;
  if (section_version_=="")
   {
    section_version_ = section_variante_ ;
    section_variante_ = "" ;
   }
  // depile les entetes de colonne
  section_colonnes_.clear() ;
  if (nouvelle_ligne())
   {
    std::string colonne ;
    while ((*this)>>colonne)
      if (colonne.size()>0)
        section_colonnes_.push_back(colonne) ;
    return true ;
   }
  else
   { return false ; }
 }

bool FichierTable::nouvelle_ligne()
 {
  // verifie qu'il ne s'agit pas d'une entete de section
  if ( ( !prepare_ligne() ) || ( iline_.peek()=='>' ) )
   { return false ; }
  // marque que la ligne a été prise en compte
  iline_prete_ = false ;
  return true ;
 }


bool FichierTable::prepare_ligne()
 {
  if ( (option_!="READ") || ! pret_a_lire() || ! ifile_ || ifile_.eof() )
   { return false ; }
  if ( iline_prete_ )
   { return true ; }

  // extract one line
  iline_.clear() ;
  icell_.clear() ;
  std::string ligne ;
  std::getline(ifile_,ligne,'\n') ;

  //std::cout<<"DEBUG: "<<ligne<<endl ;

  // suppression des eventuels caracteres de
  // controle qui traineraient en fin de ligne
  while ( (!ligne.empty()) && (ligne.back()<32) )
   { ligne.pop_back() ; }

  // jonction automatique en cas de \ final
  while (ligne.back()=='\\')
   {
    ligne.pop_back() ;
    std::string ligne2 ;
    std::getline(ifile_,ligne2,'\n') ;
    while ( (!ligne2.empty()) && (ligne2.back()<32) )
     { ligne2.pop_back() ; }
    ligne.append(ligne2) ;
   }

  // suppression des commentaires
  std::string::size_type p = ligne.find_first_of('#') ;
  if (p!=std::string::npos)
   { ligne.erase(p) ; }

  // nettoyage des espaces
  ligne = std::regex_replace(ligne,std::regex("^\\s+"),"") ;
  ligne = std::regex_replace(ligne,std::regex("\\s+$"),"") ;
  ligne = std::regex_replace(ligne,std::regex("\\s+")," ") ;

  // nettoyage des delimiteurs
  ligne = std::regex_replace(ligne,std::regex("\\s*;\\s*"),";") ;
  // en fait, je dois garder les ; finaux, sous peine de
  // voir des valeurs dupliquées non intentionnellement
  //ligne = std::regex_replace(ligne,std::regex(";*$"),"") ;
  //ligne = std::regex_replace(ligne,std::regex("^;*$"),"") ;
  //ligne = std::regex_replace(ligne,std::regex("^>([^;]*);*$"),">$1") ;

  // élimination des caractères accentués
  for ( auto & c : ligne )
    {
     unsigned char uc = c ;
     if ((192<=uc)&&(uc<=197)) c = 'A' ;
     if ((199<=uc)&&(uc<=199)) c = 'C' ;
     if ((200<=uc)&&(uc<=203)) c = 'E' ;
     if ((204<=uc)&&(uc<=207)) c = 'I' ;
     if ((210<=uc)&&(uc<=214)) c = 'O' ;
     if ((217<=uc)&&(uc<=220)) c = 'U' ;

     if ((224<=uc)&&(uc<=229)) c = 'a' ;
     if ((231<=uc)&&(uc<=231)) c = 'c' ;
     if ((232<=uc)&&(uc<=235)) c = 'e' ;
     if ((236<=uc)&&(uc<=239)) c = 'i' ;
     if ((242<=uc)&&(uc<=246)) c = 'o' ;
     if ((249<=uc)&&(uc<=252)) c = 'u' ;
    }

  // fin de fichier ou suppression d'une ligne vide
  if (ligne.empty())
   {
    if ((!ifile_)||ifile_.eof())
     {
      is_ok_ = false ;
      return is_ok_ ;
     }
    else
     {
      // suppression des lignes vides
      return prepare_ligne() ;
     }
   }
  
  // alimente la istringstream
  iline_.str(ligne) ;
  iline_prete_ = true ;
  return true ;
 }


bool FichierTable::prepare_extraction()
 {
  if ( (option_!="READ") || ! pret_a_lire() || !iline_ )
   { return false ; }

  // extract one cell
  icell_.clear() ;
  std::getline(iline_,icell_,';') ;

  return (iline_||!icell_.empty()) ;
 }

template <>
FichierTable & operator>>< std::string >( FichierTable & ft, std::string & var )
 {
  if (ft.prepare_extraction())
   { var = ft.icell_ ; }
  return ft ;
 }

template <>
FichierTable & operator>>< bool >( FichierTable & ft, bool & var )
 {
  if (!ft.prepare_extraction())
   { return ft ; }
  if (ft.icell_.empty())
   { return ft ; }
  if ((ft.icell_=="oui")||(ft.icell_=="O")||(ft.icell_=="yes")||(ft.icell_=="Y"))
   { var = true ; }
  else if ((ft.icell_=="non")||(ft.icell_=="no")||(ft.icell_=="N"))
   { var = false ; }
  else
   {
    Afficheur::defaut(VX::AVERTISSEMENT)<<"valeur invalide pour un booleen : "<<ft.icell_<<endl ;
    ft.is_ok_ = false ;
    return ft ;
   }
  return ft ;
 }

template <>
FichierTable & operator>>< std::pair<int,int> >( FichierTable & ft, std::pair<int,int> & var )
 {
  if (!ft.prepare_extraction())
   { return ft ; }
  if (ft.icell_.empty())
   { return ft ; }
  std::string::size_type p = ft.icell_.find_first_of(':') ;
  if (p==std::string::npos)
   {
    Afficheur::defaut(VX::AVERTISSEMENT)<<"valeur invalide pour une paire : "<<ft.icell_<<endl ;
    ft.is_ok_ = false ;
    return ft ;
   }
  else
   { ft.icell_[p] = ' ' ; }
  std::istringstream iss(ft.icell_) ;
  iss>>var.first>>var.second ;
  return ft ;
 }

void FichierTable::verifie_colonnes( const std::string & colonnes ) const
 {
  std::vector<std::string> split_colonnes ;
  std::istringstream iss(colonnes) ;
  std::string colonne ;
  while (std::getline(iss,colonne,';'))
    split_colonnes.push_back(colonne) ;
  if (split_colonnes!=section_colonnes_)
   {
    std::ostringstream oss ;
    oss<<"[FichierTable::verifie_colonnes] Colonnes inattendues : "<<colonnes ;
    throw std::runtime_error(oss.str()) ;
   }
 }


// ***************************************************************
// WRITE Interface
// ***************************************************************

void FichierTable::section_colonnes( const std::string & colonnes )
 {
  section_colonnes_.clear() ;
  std::istringstream iss(colonnes) ;
  std::string colonne ;
  while (std::getline(iss,colonne,';'))
    section_colonnes_.push_back(colonne) ;
 }

void FichierTable::section_write()
 {
  if ( (option_!="WRITE") || ! pret_a_ecrire() )
   { return ; }
  ofile_<<"> "<<section_nom_ ;
  if (section_variante_!="")
    ofile_<<" "<<section_variante_ ;
  if (section_version_!="")
    ofile_<<" "<<section_version_ ;
  ofile_<<endl ;

  // colonnes avec largeurs
  for ( auto colonne : section_colonnes_ )
   { (*this)<<colonne ; }
  (*this).saut_de_ligne() ;
 }

void FichierTable::efface_format()
 {
  if ( (option_!="WRITE") || ! pret_a_ecrire() )
   { return ; }
  largeurs_.clear() ;
 }

void FichierTable::nouveau_format( const std::vector<std::size_t> & largeurs )
 {
  if ( (option_!="WRITE") || ! pret_a_ecrire() )
   { return ; }
  largeurs_ = largeurs ;
 }

void FichierTable::saut_de_ligne()
 {
  if ( (option_!="WRITE") || ! pret_a_ecrire() )
   { return ; }
  ofile_<<endl ;
  indice_courant_ = 0 ;
 }


// ****************************************************************************
// UTILITIES
// ****************************************************************************

bool FichierTable::pret_a_lire()
 { return ( is_ok_ && ifile_.is_open() && !is_eof_ )  ; }

bool FichierTable::pret_a_ecrire()
 { return ( is_ok_ && ofile_.is_open() ) ; }

