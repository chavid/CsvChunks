
#include "chunks_file.h"
#include <regex>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <cassert>


// ***********************************************************************
//
// ***********************************************************************

ChunksFile::ChunksFile
 ( bool debug )
 : debug_(debug), is_eol_(false),is_eof_(false)
 {}

bool ChunksFile::open
 ( std::string_view name,
   Mode mode )
 {
  if (ofile_.is_open()||ifile_.is_open())
   { close() ; }
  name_ = name ;
  mode_ = mode ;
  is_ok_ = true ;

  // READ opening
  if (mode_==Mode::READ)
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
    icells_.clear() ;
    iorder_.clear() ;
    current_indice_ = 0 ;
    pcell_ = nullptr ;
    return true ;
   }

  // WRITE opening
  else if (mode_==Mode::WRITE)
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
      <<"[ChunksFile::open] "<<name_
      <<" can only be opened in mode READ or WRITE"
      <<std::endl ;
    return false ;
   }
 }

bool ChunksFile::close()
 {
  ofile_.close() ;
  ifile_.close() ;
  iline_.clear() ;
  icells_.clear() ;
  iorder_.clear() ;
  current_indice_ = 0 ;
  pcell_ = nullptr ;
  iline_ready_ = false ;
  is_eol_ = false ;
  is_eof_ = false ;
  is_ok_ = true ;
  name_ = "" ;
  mode_ = Mode::UNDEFINED ;
  return true ;
 }

ChunksFile::~ChunksFile()
 { close() ; }

ChunksFile::operator void *()
 {
  if
   (
     ( !is_ok_ ) ||
	   ( (mode_==Mode::READ) &&  ( (!ready_for_reading()) || eol_ ) ) ||
     ( (mode_==Mode::WRITE) && (!ready_for_writing()) )
   )
   { return ((void*)0) ; }
  return this ;
 }



// ***************************************************************
// READ Interface
// ***************************************************************

bool ChunksFile::read_next_chunk()
 {
  // cherche la prochaine entete de section
  while ( prepare_line() && (icells_.front().front()!='>' ) )
   { iline_ready_ = false ; }
  // verifie qu'il y a bien une entete de section
  if ( ( std::empty(icells_) ) || ( icells_.front().front()!='>' ) )
   { return false ; }
  // marque que la line a été prise en compte
  iline_ready_ = false ;
  // extrait les informations de section
  // la première extraction sert à éliminer le caractère de section
  // on élimine les éventuels ";" qui trainent
  std::string prompt ;
  std::string chunk_name = "" ;
  std::string chunk_flavor = "" ;
  std::string chunk_version = "" ;
  std::istringstream iss(icells_.front()) ;
  iss>>prompt>>chunk_name>>chunk_flavor>>chunk_version ;
  if (chunk_version=="")
   {
    chunk_version = chunk_flavor ;
    chunk_flavor = "" ;
   }
  chunk_name_ = FrequentString(std::regex_replace(chunk_name,std::regex(";*$"),"")) ;
  chunk_flavor_ = FrequentString(std::regex_replace(chunk_flavor,std::regex(";*$"),"")) ;
  chunk_version_ = FrequentString(std::regex_replace(chunk_version,std::regex(";*$"),"")) ;

  // columns titles
  chunk_columns_.clear() ;
  icells_.clear() ;
  iorder_.clear() ;
  if (read_next_line())
   {
    std::string column ;
    while ((*this)>>column)
    if (column.size()>0)
     {
      std::transform(column.begin(),column.end(),column.begin(),
        [](unsigned char c){ return std::toupper(c) ; } ) ;
      chunk_columns_.push_back(std::move(column)) ;
     }
    else
     { throw std::runtime_error("anonymous column ?!") ; }
    icells_.resize(chunk_columns_.size(),"") ;
    auto imax = icells_.size() ;
    for ( decltype(imax) i = 0 ; i < imax ; ++ i )
     { iorder_.push_back(i) ; }
    return true ;
   }
  else
   { return false ; }
 }

bool ChunksFile::read_next_line()
 {
  // verifie qu'il ne s'agit pas d'une entete de section
  if ( ( !prepare_line() ) ||
       ( (!std::empty(icells_.front())) && ( icells_.front().front()=='>' ) ) )
   { return false ; }
  // marque que la line a été prise en compte
  iline_ready_ = false ;
  return true ;
 }

bool ChunksFile::prepare_line()
 {
  if ( (mode_!=Mode::READ) || !ready_for_reading() || ! ifile_ || ifile_.eof() )
   { return false ; }
  if ( iline_ready_ )
   { return true ; }

  // extract one line
  iline_.clear() ;
  std::string line ;
  std::getline(ifile_,line,'\n') ;

  //std::cout<<"DEBUG: "<<line<<std::endl ;

  // suppression des eventuels caracteres de
  // controle qui traineraient en fin de line
  while ( (!line.empty()) && (line.back()<32) )
   { line.pop_back() ; }

  // jonction automatique en cas de \ final
  while (line.back()=='\\')
   {
    line.pop_back() ;
    std::string line2 ;
    std::getline(ifile_,line2,'\n') ;
    while ( (!line2.empty()) && (line2.back()<32) )
     { line2.pop_back() ; }
    line.append(line2) ;
   }

  // suppression des commentaires
  std::string::size_type p = line.find_first_of('#') ;
  if (p!=std::string::npos)
   { line.erase(p) ; }

  // nettoyage des espaces
  line = std::regex_replace(line,std::regex("^\\s+"),"") ;
  line = std::regex_replace(line,std::regex("\\s+$"),"") ;
  line = std::regex_replace(line,std::regex("\\s+")," ") ;

  // nettoyage des delimiteurs
  line = std::regex_replace(line,std::regex("\\s*;\\s*"),";") ;
  // en fait, je dois garder les ; finaux, sous peine de
  // voir des valeurs dupliquées non intentionnellement
  //line = std::regex_replace(line,std::regex(";*$"),"") ;
  //line = std::regex_replace(line,std::regex("^;*$"),"") ;
  //line = std::regex_replace(line,std::regex("^>([^;]*);*$"),">$1") ;

  // élimination des caractères accentués
  for ( auto & c : line )
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

  // fin de fichier ou suppression d'une line vide
  if (line.empty())
   {
    if ((!ifile_)||ifile_.eof())
     {
      is_ok_ = false ;
      return is_ok_ ;
     }
    else
     {
      // suppression des lines vides
      return prepare_line() ;
     }
   }
  
  // alimente la istringstream
  iline_.str(line) ;
  iline_ready_ = true ;
  // pre-extract cells
  std::string cell ;
  if (std::empty(icells_))
   {
    while (std::getline(iline_,cell,';'))
     { icells_.push_back(std::move(cell)) ; }
   }
  else
   {
    auto max = icells_.size() ;
    icells_.assign(max,"") ;
    decltype(max) i = 0 ;
    while ((i<max)&&(std::getline(iline_,cell,';')))
     { icells_[i++] = std::move(cell) ; }
   }

  current_indice_ = 0 ;
  eol_ = false ;
  pcell_ = nullptr ;
  return true ;
 }

bool ChunksFile::prepare_extraction()
 {
  pcell_ = nullptr ;
  if ( (mode_!=Mode::READ) || (!ready_for_reading()))
   { return false ; }
  else if (std::empty(iorder_)) // not in a chunk
   {
    if (current_indice_>=icells_.size())
     { eol_ = true ; return false ; }
    else
     {
      pcell_ = &(icells_[current_indice_++]) ;
      return true ; //(!(pcell_->empty())) ;
     }
   }
  else // in a chunk
   {
    if (current_indice_>=iorder_.size())
     { eol_ = true ; return false ; }
    else
     {
      auto i = iorder_[current_indice_++] ;
      if (i<icells_.size())
       {
        pcell_ = &(icells_[i]) ;
        return true ;
       }
      else
       { return false ; }
     }
   }
 }

template <>
ChunksFile & operator>>< std::string >( ChunksFile & ft, std::string & var )
 {
  if (ft.prepare_extraction())
   { var = (*ft.pcell_) ; }
  return ft ;
 }

template <>
ChunksFile & operator>>< bool >( ChunksFile & ft, bool & var )
 {
  if (!ft.prepare_extraction())
   { return ft ; }
  if ((*ft.pcell_).empty())
   { return ft ; }
  if (((*ft.pcell_)=="oui")||((*ft.pcell_)=="O")||((*ft.pcell_)=="yes")||((*ft.pcell_)=="Y"))
   { var = true ; }
  else if (((*ft.pcell_)=="non")||((*ft.pcell_)=="no")||((*ft.pcell_)=="N"))
   { var = false ; }
  else
   {
    std::cout<<"WARNING: invalue bool value: "<<(*ft.pcell_)<<std::endl ;
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
  if ((*ft.pcell_).empty())
   { return ft ; }
  std::string::size_type p = (*ft.pcell_).find_first_of(':') ;
  if (p==std::string::npos)
   {
    std::cout<<"WARNING: invalid pair value: "<<(*ft.pcell_)<<std::endl ;
    ft.is_ok_ = false ;
    return ft ;
   }
  else
   { (*ft.pcell_)[p] = ' ' ; }
  std::istringstream iss((*ft.pcell_)) ;
  iss>>var.first>>var.second ;
  return ft ;
 }

using Columns = Glossary<struct ColumnsFoo> ;

void ChunksFile::read_columns_order( std::string_view columns_sv )
 {
  Columns columns(columns_sv) ;

  // precompute the glossary ranks of the chunk columns
  constexpr auto max_rank = static_cast<Columns::Id>(-1) ;
  std::vector<Columns::Id> chunk_ranks(chunk_columns_.size()) ;
  for ( std::size_t i = 0 ; i<chunk_columns_.size() ; ++i )
   {
    chunk_ranks[i] = columns.id_opt(chunk_columns_[i]).value_or(max_rank) ;
    if (chunk_ranks[i]==max_rank)
     { std::cout<<"WARNING: unknown column: "<<chunk_columns_[i]<<std::endl ; }
   }

  // detect lacking columns
  for ( Columns::Id id = 0 ; id < columns.size() ; ++id )
  if ( std::find(chunk_ranks.cbegin(),chunk_ranks.cend(),id) == chunk_ranks.cend() )
   {
    std::cout<<"WARNING: lacking column: "<<columns.str(id)<<std::endl ;
    chunk_ranks.push_back(id) ;
    iorder_.push_back(iorder_.back()+1) ;
   }

  // sort the indirect indexes
  std::sort(iorder_.begin(),iorder_.end(),[&]( auto i1, auto i2 )
   { return (chunk_ranks[i1]<chunk_ranks[i2]) ; }) ;

  // so to ignore unknown columns
  auto nb_unknown = std::count(chunk_ranks.cbegin(),chunk_ranks.cend(),max_rank) ;
  iorder_.resize(iorder_.size()-nb_unknown) ;
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
    chunk_columns_.emplace_back(columns.substr(pos1,pos2-pos1)) ;
    pos1 = pos2+1 ;
    pos2 = columns.find(';',pos1) ;
   }
  if (pos1<columns.size())
   { chunk_columns_.emplace_back(columns.substr(pos1)) ; }
 }

void ChunksFile::chunk_write()
 {
  if ( (mode_!=Mode::WRITE) || ! ready_for_writing() )
   { return ; }
  ofile_<<"> "<<chunk_name_ ;
  if (!std::empty(chunk_flavor_))
    ofile_<<" "<<chunk_flavor_ ;
  if (!std::empty(chunk_version_))
    ofile_<<" "<<chunk_version_ ;
  ofile_<<std::endl ;

  // columns avec largeurs
  for ( auto column : chunk_columns_ )
   { (*this)<<column ; }
  (*this).write_next_line() ;
 }

void ChunksFile::remove_format()
 {
  if ( (mode_!=Mode::WRITE) || ! ready_for_writing() )
   { return ; }
  widths_.clear() ;
 }

void ChunksFile::change_format( const std::vector<std::size_t> & largeurs )
 {
  if ( (mode_!=Mode::WRITE) || ! ready_for_writing() )
   { return ; }
  widths_ = largeurs ;
 }

void ChunksFile::write_next_line()
 {
  if ( (mode_!=Mode::WRITE) || ! ready_for_writing() )
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

