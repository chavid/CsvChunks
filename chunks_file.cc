
#include "chunks_file.h"
#include <regex>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <cassert>


//========================================================
// General implementation
//========================================================

ChunksFile::ChunksFile( bool debug )
 : file_(debug)
 {
  file_.word_delim(';') ;
  remaining_header_line_ = false ;
 }

bool ChunksFile::open
 ( std::string_view name,
   LinesFile::Mode mode )
 {
  if (!file_.open(name,mode))
   { return false ; }
  if (mode==LinesFile::Mode::READ)
   {
    is_eoc_ = true ;
    is_eol_ = true ;
    icells_.clear() ;
    iorder_.clear() ;
    current_indice_ = 0 ;
    pcell_ = nullptr ;
   }
  if (mode==LinesFile::Mode::WRITE)
   {
    is_eoc_ = true ;
    current_indice_ = 0 ;
   }
  return true ;
 }

ChunksFile::operator void *()
 { return (is_eol_?nullptr:this) ; }

bool ChunksFile::close()
 {
  file_.close() ;
  icells_.clear() ;
  iorder_.clear() ;
  current_indice_ = 0 ;
  pcell_ = nullptr ;
  //is_eol_ = false ;
  //is_eof_ = false ;
  return true ;
 }

ChunksFile::~ChunksFile()
 { close() ; }



//========================================================
// Read implementation
//========================================================

bool ChunksFile::seek_next_chunk()
 {
  assert(file_.mode()==LinesFile::Mode::READ) ;
  is_eoc_ = true ;

  // search chunk header line
  if (!remaining_header_line_)
   {
    bool iline_ready = false ;
    do
     {
      iline_ready = file_.read_next_line() ;
     } while ( iline_ready && (file_.line().front()!='>' ) ) ;
    if (!iline_ready) { return false ; }
   }
  remaining_header_line_ = false ;

  // extract information
  file_.word_delim(' ') ;
  std::string prompt ;
  std::string chunk_name ;
  std::string chunk_flavor ;
  std::string chunk_version ;
  file_>>prompt>>chunk_name>>chunk_flavor>>chunk_version ;
  if ((chunk_version.empty())||(chunk_version[0]==';'))
   {
    chunk_version = chunk_flavor ;
    chunk_flavor.clear() ;
   }
   
  // there may be trailing semicolons due to an excel edit
  chunk_name_ = FrequentString(std::regex_replace(chunk_name,std::regex(";*$"),"")) ;
  chunk_flavor_ = FrequentString(std::regex_replace(chunk_flavor,std::regex(";*$"),"")) ;
  chunk_version_ = FrequentString(std::regex_replace(chunk_version,std::regex(";*$"),"")) ;
  file_.word_delim(';') ;

  // columns titles
  chunk_columns_.clear() ;
  icells_.clear() ;
  iorder_.clear() ;
  if (file_.read_next_line())
   {
    std::string column ;
    while (file_>>column)
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
    is_eoc_ = false ;
    return true ;
   }
  else
   { return false ; }
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

bool ChunksFile::read_next_line()
 {
  assert(file_.mode()==LinesFile::Mode::READ) ;
  if (is_eoc_) { return false ; }
  // get next line and check it is not a chunk header line
  if (!file_.read_next_line()) { is_eoc_ = true ; return false ; }
  if (file_.line().front()=='>' )
   {
    is_eoc_ = true ;
    remaining_header_line_ = true ;
    return false ;
   }
  // pre-extract cells
  std::string cell ;
  if (std::empty(icells_))
   {
    while (file_>>cell)
     { icells_.push_back(std::move(cell)) ; }
   }
  else
   {
    auto max = icells_.size() ;
    icells_.assign(max,"") ;
    decltype(max) i = 0 ;
    while ((i<max)&&(file_>>cell))
     { icells_[i++] = std::move(cell) ; }
   }
  is_eol_ = false ;
  current_indice_ = 0 ;
  pcell_ = nullptr ;
  return true ;
 }

bool ChunksFile::prepare_extraction()
 {
  assert(file_.mode()==LinesFile::Mode::READ) ;
  if (is_eol_) { return false ; }

  pcell_ = nullptr ;

  if (std::empty(iorder_)) // not ordered chunk
   {
    if (current_indice_>=icells_.size())
     { is_eol_ = true ; return false ; }
    else
     {
      pcell_ = &(icells_[current_indice_++]) ;
      return true ; //(!(pcell_->empty())) ;
     }
   }
  else // ordered chunk
   {
    if (current_indice_>=iorder_.size())
     { is_eol_ = true ; return false ; }
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
ChunksFile & operator>>< std::string >( ChunksFile & cf, std::string & var )
 {
  if (cf.prepare_extraction())
   { std::swap(*cf.pcell_,var) ; }
  return cf ;
 }

bool is_true( std::string_view cell )
 { 
  if ((cell=="true")||(cell=="T")||(cell=="t")) return true ;
  if ((cell=="yes")||(cell=="Y")||(cell=="y")) return true ;
  if ((cell=="oui")||(cell=="O")||(cell=="o")) return true ;
  return false ;
 }

bool is_false( std::string_view cell )
 { 
  if ((cell=="false")||(cell=="F")||(cell=="f")) return true ;
  if ((cell=="no")||(cell=="N")||(cell=="n")) return true ;
  if ((cell=="non")||(cell=="N")||(cell=="n")) return true ;
  return false ;
 }

template <>
ChunksFile & operator>>< bool >( ChunksFile & cf, bool & var )
 {
  if (!cf.prepare_extraction())
   { return cf ; }
  if ((*cf.pcell_).empty())
   { return cf ; }

  if (is_true(*cf.pcell_)) { var = true ; }
  else if (is_false(*cf.pcell_)) { var = false ; }
  else
   {
    std::cout<<"WARNING: invalid bool value: "<<(*cf.pcell_)<<std::endl ;
    return cf ;
   }
  
  return cf ;
 }

template <>
ChunksFile & operator>>< std::pair<int,int> >( ChunksFile & cf, std::pair<int,int> & var )
 {
  if (!cf.prepare_extraction())
   { return cf ; }
  if ((*cf.pcell_).empty())
   { return cf ; }
  std::string::size_type p = (*cf.pcell_).find_first_of(':') ;
  if (p==std::string::npos)
   {
    std::cout<<"WARNING: invalid pair value: "<<(*cf.pcell_)<<std::endl ;
    return cf ;
   }
  else
   { (*cf.pcell_)[p] = ' ' ; }
  std::istringstream iss((*cf.pcell_)) ;
  iss>>var.first>>var.second ;
  return cf ;
 }


//========================================================
// Write implementation
//========================================================

void ChunksFile::chunk_columns( std::string_view columns )
 {
  assert(file_.mode()==LinesFile::Mode::WRITE) ;

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

void ChunksFile::chunk_widths( const std::vector<std::size_t> & widths )
 {
  assert(file_.mode()==LinesFile::Mode::WRITE) ;
  widths_ = widths ;
 }

void ChunksFile::reset_widths()
 {
  assert(file_.mode()==LinesFile::Mode::WRITE) ;
  widths_.clear() ;
 }

void ChunksFile::chunk_write()
 {
  assert(file_.mode()==LinesFile::Mode::WRITE) ;

  // chunk header
  file_.word_delim(' ') ;
  file_<<">"<<chunk_name_ ;
  if (!std::empty(chunk_flavor_))
    file_<<chunk_flavor_ ;
  if (!std::empty(chunk_version_))
    file_<<chunk_version_ ;
  file_.write_next_line() ;
  file_.word_delim(';') ;
  // I allow from here the <<
  is_eoc_ = false ;

  // chunk columns
  current_indice_ = 0 ;
  for ( auto column : chunk_columns_ )
   { (*this)<<column ; }
  file_.write_next_line() ;

  // reset columns
  current_indice_ = 0 ;
 }

void ChunksFile::write_next_line()
 {
  assert(file_.mode()==LinesFile::Mode::WRITE) ;
  if (is_eoc_) { return ; }
  file_.write_next_line() ;
  current_indice_ = 0 ;
 }
