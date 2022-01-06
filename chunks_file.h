
#ifndef CHUNKS_FILE_H
#define CHUNKS_FILE_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#include <iomanip>

class ChunksFile
 {
  public :

    ChunksFile( bool debug =false ) ;
    bool open
     ( std::string_view name,
       std::string_view option = "READ" ) ;
    operator void *() ;
    bool close() ;
    ~ChunksFile() ;

    // Interface de lecture
    bool next_chunk() ;
    const std::string & chunk_name() { return chunk_name_ ; }
    const std::string & chunk_flavor() { return chunk_flavor_ ; }
    const std::string & chunk_version() { return chunk_version_ ; }
    const std::vector<std::string> & chunk_columns() { return chunk_columns_ ; }
    void check_columns( const std::string & colonnes ) const ;
    bool read_next_line() ;
    template <typename T>
    friend ChunksFile & operator>>( ChunksFile &, T & var ) ;

    // Interface d'écriture
    void chunk_name( std::string_view nom ) { chunk_name_ = nom ; }
    void chunk_flavor( std::string_view variante ) { chunk_flavor_ = variante ; }
    void chunk_version( std::string_view version ) { chunk_version_ = version ; }
    void chunk_columns( const std::vector<std::string> & colonnes ) { chunk_columns_ = colonnes ; }
    void chunk_columns( std::string_view colonnes ) ;
    void change_format( const std::vector<std::size_t> & largeurs ) ;
    void chunk_write() ;
    void remove_format() ;
    void write_next_line() ;
    template <typename T>
    friend ChunksFile & operator<<( ChunksFile &, T var ) ;

  private :

    bool is_ok_ ;
    bool debug_ ;

    std::string name_ ;
    std::string option_ ;

    // section courante
    std::string chunk_name_ ;
    std::string chunk_flavor_ ;
    std::string chunk_version_ ;
    std::vector<std::string> chunk_columns_ ;

    std::ofstream ofile_ ;
    std::vector<std::size_t>::size_type current_indice_ = 0 ;
    std::vector<std::size_t> widths_ ;

    std::ifstream ifile_ ;
    std::istringstream iline_ ;
    bool iline_ready_ = false ;
    std::string icell_ ;
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
    std::istringstream iss(ft.icell_) ;
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
  if ( (ft.option_!="WRITE") || ! ft.ready_for_writing() )
   { return ft ; }
  if (ft.widths_.size()==0)
   { ft.ofile_<<var ; }
  else
   {
    std::ostringstream oss ;
    oss<<var ;
    ft.ofile_<<" " ;
    if (ft.current_indice_<ft.widths_.size())
     { ft.ofile_<<std::setw(ft.widths_[ft.current_indice_]) ; }
    ft.ofile_<<oss.str()<<" ;" ;
    ft.current_indice_++ ;
   }
  return ft ;
 }

#endif
