
#ifndef LINES_FILE_H
#define LINES_FILE_H

#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <utility>
#include <iomanip>

//==================================================================
// This class helps to read/write a textual file line by line.
// When the end-of-line is backslashed, concat the following line.
// Everything after a sharp character (#) is commented out.
//==================================================================

class LinesFile
 {
  public :

    enum class Mode { UNDEFINED, READ, WRITE } ;

    // construction/destruction, open/close, delim, status
    LinesFile( bool debug = false ) ;
    bool open( std::string_view name, Mode = Mode::READ ) ;
    operator void *() ;
    bool close() ;
    ~LinesFile() ;

    // config
    void word_delim( char c = ' ' ) { word_delim_ = c ; }
    void fill_char( char c = ' ' ) { padding_char_ = c ; }
    char word_delim() { return word_delim_ ; }
    char fill_char() { return padding_char_ ; }
    bool debug() { return debug_ ; }
    std::string_view name() { return name_ ; }
    Mode mode() { return mode_ ; }

    // read interface
    bool read_next_line() ; // get next non empty line
    std::string const & line() { return iline_ ; }
    std::size_t line_number() { return iline_number_ ; }
    template <typename T>
    friend LinesFile & operator>>( LinesFile &, T & var ) ;

    // write interface
    template <typename T>
    friend LinesFile & operator<<( LinesFile &, T var ) ;
    bool write_next_line() ;

  private :


    char line_delim_ = '\n' ;
    char word_delim_ = ' ' ;
    char padding_char_ = ' ' ;

    bool is_ok_ = true ;
    bool is_eol_ = true ;

    bool debug_ = false ;
    std::string name_ {} ;
    Mode mode_ { Mode::UNDEFINED } ;

    std::ofstream ofile_ ;
    std::ostringstream oss_ ;

    std::ifstream ifile_ ;
    std::string iline_ ;
    std::size_t iline_number_ = 0 ;
    std::istringstream iss_ ;
    std::string icell_ ;

    bool prepare_line() ;
    bool prepare_extraction() ;
 } ;


//========================================================
// Inline templates
//========================================================

template <typename T>
LinesFile & operator>>( LinesFile & lf, T & var )
 {
  if (lf.prepare_extraction())
   {
    std::istringstream iss(lf.icell_) ;
    iss>>var ;
    //lf.icell_.clear() ;
   }
  return lf ;
 }

template <>
LinesFile & operator>>< std::string >( LinesFile &, std::string & var ) ;

template <typename T>
LinesFile & operator<<( LinesFile & lf, T var )
 {
  if ( (lf.mode_!=LinesFile::Mode::WRITE) || (!lf.is_ok_) )
   { return lf ; }
  if (lf.word_delim_!=lf.padding_char_)
   {
    // We preserve empty values
    lf.oss_<<var<<lf.word_delim_ ;
   }
  else
   {
    // We only keep intermediary delimiters
    if (lf.oss_.view().size()>0)
    { lf.oss_<<lf.word_delim_ ; }
    lf.oss_<<var ;
   }
  return lf ;
 }

#endif
