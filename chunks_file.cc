
#include "chunks_file.h"
#include <regex>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>


// ***********************************************************************
//
// ***********************************************************************

ChunksFile::ChunksFile
 ( bool debug )
 : is_ok_(true), debug_(debug),
   name_(""), option_(""),
   is_eol_(false),is_eof_(false)
 {}

bool ChunksFile::open
 ( std::string_view name,
   std::string_view option )
 {
  if (ofile_.is_open()||ifile_.is_open())
   { close() ; }
  name_ = name ;
  option_ = option ;
  is_ok_ = true ;

  // READ opening
  if (option_=="READ")
   {
    if (debug_)
     { std::cout<<"open in read mode: "<<name_<<std::endl ; }
    ifile_.open(name_.c_str()) ;
    if (!ifile_.is_open())
     {
      std::cout<<"failed open in read mode: "<<name_<<std::endl ;
      close() ;
      is_ok_ = false ;
      return false ;
     }
    iline_ready_ = false ;
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
     { std::cout<<"open in write mode: "<<name_<<std::endl ; }
    ofile_.open(name_.c_str()) ;
    if (!ofile_.is_open())
     {
      std::cout<<"failed open in write mode: "<<name_<<std::endl ;
      close() ;
      is_ok_ = false ;
      return false ;
     }
    ofile_<<std::left ;
    return true ;
   }

  // Unknwon or not-implemented
  else
   {
    close() ;
    is_ok_ = false ;
    std::cerr
      <<"[ChunksFile::ChunksFile] "
      <<"l'option "<<option_<<" n'est pas implantee"
      <<", impossible d'ouvrir le fichier "<<name_
      <<std::endl ;
    return false ;
   }
 }

bool ChunksFile::close()
 {
  ofile_.close() ;
  ifile_.close() ;
  iline_.clear() ;
  icell_.clear() ;
  iline_ready_ = false ;
  is_eol_ = false ;
  is_eof_ = false ;
  is_ok_ = true ;
  name_ = "" ;
  option_ = "" ;
  return true ;
 }

ChunksFile::~ChunksFile()
 { close() ; }

ChunksFile::operator void *()
 {
  if ( ( !is_ok_ ) ||
	   ( (option_=="READ") && ( (!ready_for_reading()) || (!iline_ ) ) ) ||
       ( (option_=="WRITE") && !ready_for_writing() ) )
   {
	  return ((void*)0) ;
   }
  return this ;
 }



// ***************************************************************
// READ Interface
// ***************************************************************

bool ChunksFile::next_chunk()
 {
  // cherche la prochaine entete de section
  while ( prepare_line() && (iline_.peek()!='>' ) )
   { iline_ready_ = false ; }
  // verifie qu'il y a bien une entete de section
  if ( iline_.peek()!='>' )
   { return false ; }
  // marque que la ligne a été prise en compte
  iline_ready_ = false ;
  // extrait les informations de section
  // la première extraction sert à éliminer le caractère de section
  // on élimine les éventuels ";" qui trainent
  std::string prompt ;
  chunk_name_ = "" ;
  chunk_flavor_ = "" ;
  chunk_version_ = "" ;
  iline_>>prompt>>chunk_name_>>chunk_flavor_>>chunk_version_ ;
  chunk_name_ = std::regex_replace(chunk_name_,std::regex(";*$"),"") ;
  chunk_flavor_ = std::regex_replace(chunk_flavor_,std::regex(";*$"),"") ;
  chunk_version_ = std::regex_replace(chunk_version_,std::regex(";*$"),"") ;
  if (chunk_version_=="")
   {
    chunk_version_ = chunk_flavor_ ;
    chunk_flavor_ = "" ;
   }
  // depile les entetes de colonne
  chunk_columns_.clear() ;
  if (read_next_line())
   {
    std::string colonne ;
    while ((*this)>>colonne)
      if (colonne.size()>0)
        chunk_columns_.push_back(colonne) ;
    return true ;
   }
  else
   { return false ; }
 }

bool ChunksFile::read_next_line()
 {
  // verifie qu'il ne s'agit pas d'une entete de section
  if ( ( !prepare_line() ) || ( iline_.peek()=='>' ) )
   { return false ; }
  // marque que la ligne a été prise en compte
  iline_ready_ = false ;
  return true ;
 }


bool ChunksFile::prepare_line()
 {
  if ( (option_!="READ") || ! ready_for_reading() || ! ifile_ || ifile_.eof() )
   { return false ; }
  if ( iline_ready_ )
   { return true ; }

  // extract one line
  iline_.clear() ;
  icell_.clear() ;
  std::string ligne ;
  std::getline(ifile_,ligne,'\n') ;

  //std::cout<<"DEBUG: "<<ligne<<std::endl ;

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
      return prepare_line() ;
     }
   }
  
  // alimente la istringstream
  iline_.str(ligne) ;
  iline_ready_ = true ;
  return true ;
 }


bool ChunksFile::prepare_extraction()
 {
  if ( (option_!="READ") || ! ready_for_reading() || !iline_ )
   { return false ; }

  // extract one cell
  icell_.clear() ;
  std::getline(iline_,icell_,';') ;

  return (iline_||!icell_.empty()) ;
 }

template <>
ChunksFile & operator>>< std::string >( ChunksFile & ft, std::string & var )
 {
  if (ft.prepare_extraction())
   { var = ft.icell_ ; }
  return ft ;
 }

template <>
ChunksFile & operator>>< bool >( ChunksFile & ft, bool & var )
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
    std::cout<<"WARNING: invalue bool value: "<<ft.icell_<<std::endl ;
    ft.is_ok_ = false ;
    return ft ;
   }
  return ft ;
 }

template <>
ChunksFile & operator>>< std::pair<int,int> >( ChunksFile & ft, std::pair<int,int> & var )
 {
  if (!ft.prepare_extraction())
   { return ft ; }
  if (ft.icell_.empty())
   { return ft ; }
  std::string::size_type p = ft.icell_.find_first_of(':') ;
  if (p==std::string::npos)
   {
    std::cout<<"WARNING: invalid pair value: "<<ft.icell_<<std::endl ;
    ft.is_ok_ = false ;
    return ft ;
   }
  else
   { ft.icell_[p] = ' ' ; }
  std::istringstream iss(ft.icell_) ;
  iss>>var.first>>var.second ;
  return ft ;
 }

void ChunksFile::check_columns( const std::string & colonnes ) const
 {
  std::vector<std::string> split_colonnes ;
  std::istringstream iss(colonnes) ;
  std::string colonne ;
  while (std::getline(iss,colonne,';'))
    split_colonnes.push_back(colonne) ;
  if (split_colonnes!=chunk_columns_)
   {
    std::ostringstream oss ;
    oss<<"[ChunksFile::verifie_colonnes] Colonnes inattendues : "<<colonnes ;
    throw std::runtime_error(oss.str()) ;
   }
 }


// ***************************************************************
// WRITE Interface
// ***************************************************************

void ChunksFile::chunk_columns( std::string_view columns )
 {
  chunk_columns_.clear() ;
  std::string_view::size_type pos1 = 0 ;
  std::string_view::size_type pos2 = columns.find(';',pos1) ;
  while ( pos2 != std::string_view::npos )
   {
    chunk_columns_.push_back(std::string(columns.substr(pos1,pos2-pos1))) ;
    pos1 = pos2+1 ;
    pos2 = columns.find(';',pos1) ;
   }
  if (pos1<columns.size())
   { chunk_columns_.push_back(std::string(columns.substr(pos1))) ; }
 }

void ChunksFile::chunk_write()
 {
  if ( (option_!="WRITE") || ! ready_for_writing() )
   { return ; }
  ofile_<<"> "<<chunk_name_ ;
  if (chunk_flavor_!="")
    ofile_<<" "<<chunk_flavor_ ;
  if (chunk_version_!="")
    ofile_<<" "<<chunk_version_ ;
  ofile_<<std::endl ;

  // colonnes avec largeurs
  for ( auto colonne : chunk_columns_ )
   { (*this)<<colonne ; }
  (*this).write_next_line() ;
 }

void ChunksFile::remove_format()
 {
  if ( (option_!="WRITE") || ! ready_for_writing() )
   { return ; }
  widths_.clear() ;
 }

void ChunksFile::change_format( const std::vector<std::size_t> & largeurs )
 {
  if ( (option_!="WRITE") || ! ready_for_writing() )
   { return ; }
  widths_ = largeurs ;
 }

void ChunksFile::write_next_line()
 {
  if ( (option_!="WRITE") || ! ready_for_writing() )
   { return ; }
  ofile_<<std::endl ;
  current_indice_ = 0 ;
 }


// ****************************************************************************
// UTILITIES
// ****************************************************************************

bool ChunksFile::ready_for_reading()
 { return ( is_ok_ && ifile_.is_open() && !is_eof_ )  ; }

bool ChunksFile::ready_for_writing()
 { return ( is_ok_ && ofile_.is_open() ) ; }

