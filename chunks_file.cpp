
#include "chunks_file.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

void read_lines
 ( const std::string & path,
   ChunksFile & finput
 )
 {
  if (!finput.open(path)) return ; 
  while (finput.read_next_line())
   {
    std::string element = "" ;
    finput>>element ;
    std::cout<<element<<std::endl ;
   }
 }

struct Chunk
 {
  std::string name ;
  std::string flavor ;
  std::string version ;
  std::vector<std::string> columns ;
  std::vector<std::size_t> widths ;
  std::vector<std::vector<std::string>> content ;
 } ;

void read_chunks
 ( const std::string & path,
   std::vector<Chunk> & chunks,
   ChunksFile & finput
 )
 {
  if (finput.open(path))
   { 
    while (finput.read_next_chunk())
     {
      if ((finput.chunk_name()!="CANDIDATS")||(finput.chunk_version()!="v5"))
       { continue ; }
      Chunk chunk ;
      chunk.name = finput.chunk_name() ;
      chunk.flavor = finput.chunk_flavor() ;
      chunk.version = finput.chunk_version() ;
      chunk.columns = finput.chunk_columns() ;
      std::cout<<"COLUMNS: "<<chunk.columns.size()<<std::endl ;
      for ( auto column : chunk.columns )
       { chunk.widths.push_back(column.size()) ; }
      while (finput.read_next_line())
       {
        std::vector<std::string> line ;
        std::string word ;
        std::size_t column = 0 ;
        while (finput>>word)
         {
          std::cout<<word<<" " ;
          line.push_back(word) ;
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
  if (foutput.open(path,ChunksFile::Mode::WRITE))
  for ( auto chunk : chunks )
   {
    foutput.chunk_name(chunk.name) ;
    foutput.chunk_flavor(chunk.flavor) ;
    foutput.chunk_version(chunk.version) ;
    foutput.chunk_columns(chunk.columns) ;
    foutput.change_format(chunk.widths) ;
    foutput.chunk_write() ;
    for ( auto line : chunk.content )
     {
      for ( auto word : line )
        { foutput<<word ; }
      foutput.write_next_line() ;
     }
    foutput.write_next_line() ;
   }
 
 }

void diff_line
 ( const std::string & message,
   const std::vector<std::string> & line1,
   const std::vector<std::string> & line2
 )
 {
  if (line1.size()!=line2.size())
   { throw std::runtime_error(message) ; }
  auto iwordmax {line1.size()} ;
  decltype(iwordmax) iword {0} ;
  for ( ; iword<iwordmax ; ++iword )
   {
    if (line1[iword]!=line2[iword])
     { throw std::runtime_error(message) ; }
   } 
 }

void diff_content
 ( const std::string & message,
   const std::vector<std::vector<std::string>> & content1,   
   const std::vector<std::vector<std::string>> & content2
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
  ChunksFile cf(true) ;

  // reading an re-reading a name/value list
  read_lines("chunks_file.in.csv",cf) ;
  read_lines("chunks_file.in.csv",cf) ;

  // reading csv chunks
  std::vector<Chunk> chunks1, chunks2 ;

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

  return 0 ;
 }




