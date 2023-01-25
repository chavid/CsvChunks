
#include "chunks_file.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

struct Chunk
 {
  FrequentString name ;
  FrequentString flavor ;
  FrequentString version ;
  std::vector<std::string> columns ;
  std::vector<std::size_t> widths ;
  std::vector<std::vector<FrequentString>> content ;
 } ;

void read_chunks
 ( const std::string & path,
   std::vector<Chunk> & chunks,
   ChunksFile & finput
 )
 {
  if (finput.open(path))
   { 
    while (finput.seek_next_chunk())
     {
      if ((finput.chunk_name()!="CANDIDATS"_fs)||
          (finput.chunk_version()!="v5"_fs))
       { continue ; }
      Chunk chunk ;
      chunk.name = finput.chunk_name() ;
      chunk.flavor = finput.chunk_flavor() ;
      chunk.version = finput.chunk_version() ;
      if (chunk.flavor=="mppc"_fs)
       {
        finput.read_columns_order("comm;name|nom;id|numero|anonymat|scei;adm-x;lo;lf;ads") ;
        chunk.columns.assign({ "comm", "nom", "id", "adm-x", "lo", "lf", "ads" }) ;
       }
      else if (chunk.flavor=="ens"_fs)
       {
        finput.read_columns_order("Commission;Name|Nom;id|numero|scei;TAdd;algo;Lo") ;
        chunk.columns.assign({ "commission", "nom", "id", "tadd", "algo", "lo" }) ;
       }
      else
       { throw std::runtime_error("unknown flavor") ; }
      for ( auto column : chunk.columns )
       { chunk.widths.push_back(std::size(column)) ; }
      while (finput.read_next_line())
       {
        std::vector<FrequentString> line ;
        std::string word ;
        std::size_t column = 0 ;
        while (finput>>word)
         {
          std::cout<<word<<" " ;
          line.push_back(FrequentString{word}) ;
          if ((column<chunk.widths.size())&&(word.size()>chunk.widths[column]))
           { chunk.widths[column] = word.size() ; }
          column++ ;
          word = "" ;
         }
        std::cout<<std::endl ;
        chunk.content.push_back(line) ;
       }
      chunks.push_back(chunk) ;
     }
   }
 }

void write_chunks
 ( const std::string & path,
   const std::vector<Chunk> & chunks,
   ChunksFile & foutput  
 )
 {
  if (foutput.open(path,LinesFile::Mode::WRITE))
  for ( auto const & chunk : chunks )
   {
    foutput.chunk_name(chunk.name) ;
    foutput.chunk_flavor(chunk.flavor) ;
    foutput.chunk_version(chunk.version) ;
    foutput.chunk_columns(chunk.columns) ;
    foutput.chunk_widths(chunk.widths) ;
    foutput.chunk_write() ;
    for ( auto const & line : chunk.content )
     {
      for ( auto const & word : line )
       {
        foutput<<word ;
       }
      foutput.write_next_line() ;
     }
    foutput.write_next_line() ;
   }
 
 }

void diff_line
 ( const std::string & message,
   const std::vector<FrequentString> & line1,
   const std::vector<FrequentString> & line2
 )
 {
  if (line1.size()!=line2.size())
   { throw std::runtime_error(message) ; }
  auto iwordmax {line1.size()} ;
  decltype(iwordmax) iword {0} ;
  for ( ; iword<iwordmax ; ++iword )
   {
    if (line1[iword]!=line2[iword])
     {
      std::ostringstream oss ;
      oss<<message<<": "<<line1[iword]<<" != "<<line2[iword]<<std::endl ;
      throw std::runtime_error(oss.str()) ;
     }
   } 
 }

void diff_content
 ( const std::string & message,
   const std::vector<std::vector<FrequentString>> & content1,   
   const std::vector<std::vector<FrequentString>> & content2
 )
 {
  if (content1.size()!=content2.size())
   { throw std::runtime_error(message) ; }
  auto ilinemax {content1.size()} ;
  decltype(ilinemax) iline {0} ;
  for ( ; iline<ilinemax ; ++iline )
   { diff_line(message,content1[iline],content2[iline]) ; } 
 }

int main()
 {
  StaticStrings::init() ;
  ChunksFile cf(true) ;

  // reading csv chunks
  std::vector<Chunk> chunks1, chunks2 ;
  try
   {
    read_chunks("chunks_file.in.csv",chunks1,cf) ;
    write_chunks("chunks_file.out.csv",chunks1,cf) ;
    read_chunks("chunks_file.out.csv",chunks2,cf) ;

    if (chunks1.size()!=chunks2.size())
     { throw std::runtime_error("Inconsistent number of chunks") ; }

    std::vector<Chunk>::const_iterator itr1, itr2 ;
    for ( itr1 = chunks1.begin(), itr2 = chunks2.begin() ;
          itr1 != chunks1.end() ; ++itr1, ++itr2 )
     {
      if (itr1->name!=itr2->name)
      { throw std::runtime_error("Chunk name issue") ; }
      if (itr1->flavor!=itr2->flavor)
      { throw std::runtime_error("Flavor issue") ; }
      if (itr1->version!=itr2->version)
      { throw std::runtime_error("Version issue") ; }
      if (itr1->columns!=itr2->columns)
      { throw std::runtime_error("Columns issue") ; }
      diff_content("Content issue",itr1->content,itr2->content) ;
     }
   }
  catch ( const std::exception & e )
   {
    std::cout<<e.what()<<std::endl ;
   }
  std::cout<<"static strings size: "<<StaticStrings::size()<<std::endl ;
  return 0 ;
 }




