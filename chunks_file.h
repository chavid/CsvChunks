
#ifndef CHUNKS_FILE_H
#define CHUNKS_FILE_H

#include "frequent_strings.h"
#include "glossary.h"
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <fstream>
#include <sstream>
#include <utility>
#include <iomanip>

class ChunksFile
 {
  public :

    enum class Mode { UNDEFINED, READ, WRITE } ;

    ChunksFile( bool debug =false ) ;
    bool open( std::string_view name, Mode = Mode::READ ) ;
    operator void *() ;
    bool close() ;
    ~ChunksFile() ;

    // Interface de lecture
    bool read_next_chunk() ;
    FrequentString chunk_name() { return chunk_name_ ; }
    FrequentString chunk_flavor() { return chunk_flavor_ ; }
    FrequentString chunk_version() { return chunk_version_ ; }
    const std::vector<std::string> & chunk_columns() { return chunk_columns_ ; }
    void read_columns_order( std::string_view ) ; // MUST be called after read_next_chunk()
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
    void change_format( const std::vector<std::size_t> & widths ) ;
    void chunk_write() ;
    void remove_format() ;
    void write_next_line() ;
    template <typename T>
    friend ChunksFile & operator<<( ChunksFile &, T var ) ;

  private :

    bool is_ok_ = true ;
    bool eol_ = false ;
    bool debug_ = false ;
    std::string name_ {} ;
    Mode mode_ { Mode::UNDEFINED } ;

    // section courante
    FrequentString chunk_name_ ;
    FrequentString chunk_flavor_ ;
    FrequentString chunk_version_ ;
    std::vector<std::string> chunk_columns_ ;

    std::ofstream ofile_ ;
    std::vector<std::size_t>::size_type current_indice_ = 0 ;
    std::vector<std::size_t> widths_ ;

    std::ifstream ifile_ ;
    std::istringstream iline_ ;
    bool iline_ready_ = false ;
    std::vector<std::string> icells_ ;
    std::vector<std::vector<std::string>::size_type> iorder_ ;
    std::string * pcell_ ;
    bool is_eol_ ;
    bool is_eof_ ;

    bool prepare_extraction() ;
    bool prepare_line() ;

    bool ready_for_reading() ;   // cannot be const because of is_open()
    bool ready_for_writing() ; // which is not const with gcc 3.4.6
 } ;


//========================================================
// templates
//========================================================

template <typename T>
ChunksFile & operator>>( ChunksFile & ft, T & var )
 {
  if (ft.prepare_extraction())
   {
    std::istringstream iss(*ft.pcell_) ;
    iss>>var ;
   }
  return ft ;
 }

template <>
ChunksFile & operator>>< std::string >( ChunksFile &, std::string & var ) ;

template <>
ChunksFile & operator>>< bool >( ChunksFile &, bool & var ) ;

template <>
ChunksFile & operator>>< std::pair<int,int> >( ChunksFile &, std::pair<int,int> & var ) ;

template <typename T>
ChunksFile & operator<<( ChunksFile & ft, T var )
 {
  if ( (ft.mode_!=ChunksFile::Mode::WRITE) || ! ft.ready_for_writing() )
   { return ft ; }
  if (ft.widths_.size()==0)
   { ft.ofile_<<var ; }
  else
   {
    std::ostringstream oss ;
    oss<<var ;
    if (ft.current_indice_>0)
     { ft.ofile_<<" " ; }
    if (ft.current_indice_<ft.widths_.size())
     { ft.ofile_<<std::setw(ft.widths_[ft.current_indice_]) ; }
    ft.ofile_<<oss.str()<<" ;" ;
    ft.current_indice_++ ;
   }
  return ft ;
 }

#endif
