
#ifndef CHUNKS_FILE_H
#define CHUNKS_FILE_H

#include "frequent_strings.h"
#include "glossary.h"
#include "lines_file.h"
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <utility>
#include <iomanip>
#include <cassert>

class ChunksFile
 {
  public :

    ChunksFile( bool debug =false ) ;
    bool open( std::string_view name, LinesFile::Mode = LinesFile::Mode::READ ) ;
    operator void *() ;
    bool close() ;
    ~ChunksFile() ;

    // Interface de lecture
    bool seek_next_chunk() ;
    FrequentString chunk_name() { return chunk_name_ ; }
    FrequentString chunk_flavor() { return chunk_flavor_ ; }
    FrequentString chunk_version() { return chunk_version_ ; }
    const std::vector<std::string> & chunk_columns() { return chunk_columns_ ; }
    void read_columns_order( std::string_view ) ; // MUST be called after seek_next_chunk()
    bool read_next_line() ;
    template <typename T>
    friend ChunksFile & operator>>( ChunksFile &, T & var ) ;

    // Interface d'écriture
    void chunk_name( std::string_view name ) { chunk_name_ = FrequentString(name) ; }
    void chunk_flavor( std::string_view flavor ) { chunk_flavor_ = FrequentString(flavor) ; }
    void chunk_version( std::string_view version ) { chunk_version_ = FrequentString(version) ; }
    void chunk_columns( std::string_view columns ) ;
    void chunk_name( FrequentString name ) { chunk_name_ = name ; }
    void chunk_flavor( FrequentString flavor ) { chunk_flavor_ = flavor ; }
    void chunk_version( FrequentString version ) { chunk_version_ = version ; }
    void chunk_columns( const std::vector<std::string> & columns ) { chunk_columns_ = columns ; }
    void chunk_widths( const std::vector<std::size_t> & widths ) ;
    void reset_widths() ;
    void chunk_write() ;
    void write_next_line() ;
    template <typename T>
    friend ChunksFile & operator<<( ChunksFile &, T var ) ;

  private :

    LinesFile file_ ;

    // current chunk
    FrequentString chunk_name_ ;
    FrequentString chunk_flavor_ ;
    FrequentString chunk_version_ ;
    std::vector<std::size_t> widths_ ;
    bool is_eoc_ ; // end of chunk

    // read
    std::vector<std::string> chunk_columns_ ;
    std::vector<std::string> icells_ ;
    std::vector<std::vector<std::string>::size_type> iorder_ ;
    std::vector<std::size_t>::size_type current_indice_ = 0 ;
    std::string * pcell_ ;
    bool is_eol_ ; // end of line
    bool remaining_header_line_ ;

    bool prepare_extraction() ;
    bool prepare_line() ;

    bool ready_for_reading() ;   // cannot be const because of is_open()
 } ;


//========================================================
// Read inline
//========================================================

template <typename T>
ChunksFile & operator>>( ChunksFile & cf, T & var )
 {
  if (cf.prepare_extraction())
   {
    std::istringstream iss(*cf.pcell_) ;
    iss>>var ;
   }
  return cf ;
 }

template <>
ChunksFile & operator>>< std::string >( ChunksFile &, std::string & var ) ;

template <>
ChunksFile & operator>>< bool >( ChunksFile &, bool & var ) ;

template <>
ChunksFile & operator>>< std::pair<int,int> >( ChunksFile &, std::pair<int,int> & var ) ;


//========================================================
// Write inline
//========================================================

template <typename T>
ChunksFile & operator<<( ChunksFile & cf, T var )
 {
  assert(cf.file_.mode()==LinesFile::Mode::WRITE) ;
  if (cf.is_eoc_) { return cf ; }
  if (cf.widths_.empty())
   {
    cf.file_<<var ;
   }
  else
   {
    std::ostringstream oss ;
    oss<<std::left ;
    if (cf.current_indice_>0)
     { oss<<" " ; }
    if (cf.current_indice_<cf.widths_.size())
     { oss<<std::setw(cf.widths_[cf.current_indice_]) ; }
    oss<<var<<" " ;
    cf.file_<<oss.str() ;
    cf.current_indice_++ ;
   }
  return cf ;
 }

#endif
