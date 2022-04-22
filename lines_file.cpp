
#include "lines_file.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

using Line = std::vector<std::string> ;
using Lines = std::vector<Line> ;

void read
 ( const std::string & path,
   LinesFile & finput,
   Lines & lines
 )
 {
  if (!finput.open(path))
   { return ; }
  finput.word_delim(' ') ;
  std::string word ;
  while (finput.read_next_line())
   {
    //std::cout<<finput.line()<<std::endl ;
    if (finput.line().front()=='>')
     { finput.word_delim(' ') ; }
    lines.emplace_back() ;
    while (finput>>word)
     {
      //std::cout<<word<<';' ;
      lines.back().push_back(word) ;
     }
    //std::cout<<std::endl ;
    if (finput.line().front()=='>')
     { finput.word_delim(';') ; }
   }
 }

void write
 ( const std::string & path,
   LinesFile & foutput,
   Lines const & lines
 )
 {
  if (!foutput.open(path,LinesFile::Mode::WRITE))
   { return ; }
  foutput.word_delim(' ') ;
  for ( auto const & line : lines )
   {
    if (line[0][0]=='>')
     { foutput.word_delim(' ') ; }
    for ( auto const & word : line )
     {
      //std::cout<<word<<' ' ;
      foutput<<word ;
     }
    //std::cout<<std::endl ;
    foutput.write_next_line() ;
    if (line[0][0]=='>')
     { foutput.word_delim(';') ; }
   }
 
 }

void diff_line
 ( std::string_view message,
   Line const & line1,
   Line const & line2
 )
 {
  if (line1.size()!=line2.size())
   {
    std::ostringstream oss ;
    oss<<message<<": "<<line1.size()<<" elements != "<<line2.size()<<" elements !"<<std::endl ;
    throw std::runtime_error(oss.str()) ;
   }
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

void diff
 ( std::string_view message,
   Lines const & content1,   
   Lines const & content2
 )
 {
  if (content1.size()!=content2.size())
   {
    std::ostringstream oss ;
    oss<<message<<": "<<content1.size()<<" lines != "<<content2.size()<<" lines !"<<std::endl ;
    throw std::runtime_error(oss.str()) ;
   }
  auto ilinemax {content1.size()} ;
  decltype(ilinemax) iline {0} ;
  for ( ; iline<ilinemax ; ++iline )
   {
    std::ostringstream oss ;
    oss<<message<<" line "<<iline ;
    diff_line(oss.str(),content1[iline],content2[iline]) ;
   } 
 }

int main()
 {
  LinesFile lf(true) ;
  Lines l1, l2 ;

  read("lines_file.in.csv",lf,l1) ;
  write("lines_file.out.csv",lf,l1) ;
  read("lines_file.out.csv",lf,l2) ;
  diff("Content issue",l1,l2) ;

  return 0 ;
 }




