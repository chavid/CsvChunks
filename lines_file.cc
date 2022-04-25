
#include "lines_file.h"
//#include <regex>
#include <iostream>
#include <iomanip>
#include <fstream>


// ***********************************************************************
//
// ***********************************************************************

LinesFile::LinesFile
 ( bool debug )
 : debug_(debug)
 {}

bool LinesFile::open
 ( std::string_view name,
   Mode mode )
 {
  if (ofile_.is_open() or ifile_.is_open())
   { close() ; }
  name_ = name ;
  mode_ = mode ;
  is_ok_ = true ;

  // READ opening
  if (mode_==Mode::READ)
   {
    //is_eol_ = true ;
    //iline_.clear() ;
    //iss_.str("") ;
    //icell_.clear() ;

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

  // Unknown or not-implemented
  else
   {
    std::cout
      <<"[LinesFile::open] "<<name_
      <<" can only be opened in mode READ or WRITE"
      <<std::endl ;
    close() ;
    is_ok_ = false ;
    return false ;
   }
 }

bool LinesFile::close()
 {
  if ( (mode_==Mode::WRITE) && (!std::empty(oss_.view())) )
   { write_next_line() ; }

  ofile_.close() ;

  ifile_.close() ;
  iline_.clear() ;
  iss_.str("") ;
  iss_.clear() ;
  icell_.clear() ;

  is_eol_ = true ;
  is_ok_ = true ;

  name_ = "" ;
  mode_ = Mode::UNDEFINED ;

  return true ;
 }

LinesFile::~LinesFile()
 { close() ; }

// used by shift operators (for lines insertion/extraction)
LinesFile::operator void *()
 {
  if ( !is_ok_ )
   { return nullptr ; }

  if ( (mode_==Mode::READ) && is_eol_ )
   { return nullptr ; }
  return this ;
 }


// ***************************************************************
// READ Interface
// ***************************************************************

bool LinesFile::read_next_line()
 {
  if ( (mode_!=Mode::READ) or !is_ok_ or !ifile_ or ifile_.eof() )
   { return false ; }

  is_eol_ = true ;
  iline_.clear() ;
  icell_.clear() ;

  do
   {
    // extract one line
    if (!std::getline(ifile_,iline_,line_delim_))
     { return false ; }

    // remove trailing control characters
    while ( (!iline_.empty()) && (iline_.back()<32) )
     { iline_.pop_back() ; }

    // concatenate if trailing backslash
    while (iline_.back()=='\\')
     {
      iline_.pop_back() ;
      std::string line2 ;
      std::getline(ifile_,line2,line_delim_) ;
      while ( (!line2.empty()) && (line2.back()<32) )
      { line2.pop_back() ; }
      iline_.append(line2) ;
     }

    // remove comments
    std::string::size_type p = iline_.find_first_of('#') ;
    if (p!=std::string::npos)
     { iline_.erase(p) ; }

  } while (std::empty(iline_)) ;

  // élimination des caractères accentués
  for ( auto & c : iline_ )
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

  // feed iss_
  iss_.str(iline_) ;
  iss_.clear() ;
  is_eol_ = false ;
  return true ;
 }

bool LinesFile::prepare_extraction()
 {
  if (is_eol_)
   { return false ; }
  while (iss_.peek()==fill_char_)
   { iss_.get() ; }
  is_eol_ = !std::getline(iss_,icell_,word_delim_) ;
  if (!is_eol_)
  while (icell_.back()==fill_char_)
   { icell_.pop_back() ; }
  return (!is_eol_) ;
 }

template <>
LinesFile & operator>>< std::string >( LinesFile & lf, std::string & var )
 {
  if (lf.prepare_extraction())
   { std::swap(var,lf.icell_) ; }
  //lf.icell_.clear() ;
  return lf ;
 }


// ***************************************************************
// WRITE Interface
// ***************************************************************

bool LinesFile::write_next_line()
 {
  if ( (mode_!=Mode::WRITE) or (!is_ok_) )
   { return false ; }
  ofile_<< oss_.str() << line_delim_ ;
  oss_.str("") ;
  oss_.clear() ;
  return true ;
 }
