
#include "chunks_file.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

struct Chunk
 {
  std::string name ;
  std::string flavor ;
  std::string version ;
  std::vector<std::string> columns ;
  std::vector<std::size_t> widths ;
  std::vector<std::vector<std::string>> content ;
 } ;

void reading
 ( const std::string & chemin,
   std::vector<Chunk> & sections,
   ChunksFile & finput
 )
 {
  if (!(finput.open(chemin)))
   {
    std::ostringstream oss ;
    oss<<"echec à l'ouverture en reading du fichier : "<<chemin<<std::endl ;
    throw std::runtime_error(oss.str()) ;
   }

  while (finput.next_chunk())
   {
    if ((finput.chunk_name()!="CANDIDATS")||(finput.chunk_version()!="v5"))
     { continue ; }
    Chunk chunk ;
    chunk.name = finput.chunk_name() ;
    chunk.flavor = finput.chunk_flavor() ;
    chunk.version = finput.chunk_version() ;
    chunk.columns = finput.chunk_columns() ;
    for ( auto colonne : chunk.columns )
      chunk.widths.push_back(colonne.size()) ;
    while (finput.read_next_line())
     {
      std::vector<std::string> ligne ;
      std::string mot ;
      std::size_t colonne = 0 ;
      while (finput>>mot)
       {
        std::cout<<mot<<" " ;
        ligne.push_back(mot) ;
        if ((colonne<chunk.widths.size())&&(mot.size()>chunk.widths[colonne]))
         { chunk.widths[colonne] = mot.size() ; }
        colonne++ ;
       }
      std::cout<<std::endl ;
      chunk.content.push_back(ligne) ;
     }
    sections.push_back(chunk) ;
   }
 }

void writing
 ( const std::string & chemin,
   const std::vector<Chunk> & sections,
   ChunksFile & foutput  
 )
 {
  if (!(foutput.open(chemin,"WRITE")))
   {
    std::ostringstream oss ;
    oss<<"echec à l'ouverture en writing du fichier : "<<chemin<<std::endl ;
    throw std::runtime_error(oss.str()) ;
   }
  
  for ( auto chunk : sections )
   {
    foutput.chunk_name(chunk.name) ;
    foutput.chunk_flavor(chunk.flavor) ;
    foutput.chunk_version(chunk.version) ;
    foutput.chunk_columns(chunk.columns) ;
    foutput.change_format(chunk.widths) ;
    foutput.chunk_write() ;
    for ( auto ligne : chunk.content )
     {
      for ( auto mot : ligne )
        { foutput<<mot ; }
      foutput.write_next_line() ;
     }

    foutput.write_next_line() ;
   }
 
 }

void diff_ligne
 ( const std::string & message,
   const std::vector<std::string> & ligne1,
   const std::vector<std::string> & ligne2
 )
 {
  if (ligne1.size()!=ligne2.size())
   { throw std::runtime_error(message) ; }
  auto imotmax {ligne1.size()} ;
  decltype(imotmax) imot {0} ;
  for ( ; imot<imotmax ; ++imot )
   {
    if (ligne1[imot]!=ligne2[imot])
     { throw std::runtime_error(message) ; }
   } 
 }

void diff_contenu
 ( const std::string & message,
   const std::vector<std::vector<std::string>> & contenu1,   
   const std::vector<std::vector<std::string>> & contenu2
 )
 {
  if (contenu1.size()!=contenu2.size())
   { throw std::runtime_error(message) ; }
  auto ilignemax {contenu1.size()} ;
  decltype(ilignemax) iligne {0} ;
  for ( ; iligne<ilignemax ; ++iligne )
   { diff_ligne(message,contenu1[iligne],contenu2[iligne]) ; } 
 }

int main()
 {
  // reading a name/value list
  std::string element ;
  ChunksFile cf(true) ;
  cf.open("chunks_file.in.csv") ; cf.read_next_line() ; element = "" ; cf>>element ; std::cout<<"First element : "<<element<<std::endl ;
  cf.open("chunks_file.in.csv") ; cf.read_next_line() ; element = "" ; cf>>element ; std::cout<<"First element : "<<element<<std::endl ;

  // reading csv chunks
  std::vector<Chunk> sections1, sections2 ;

  reading("chunks_file.in.csv",sections1,cf) ;
  writing("chunks_file.out.csv",sections1,cf) ;
  reading("chunks_file.out.csv",sections2,cf) ;

  if (sections1.size()!=sections2.size())
   { throw std::runtime_error("Inconsistent number of chunks") ; }

  std::vector<Chunk>::const_iterator itr1, itr2 ;
  for ( itr1 = sections1.begin(), itr2 = sections2.begin() ;
        itr1 != sections1.end() ; ++itr1, ++itr2 )
   {
    if (itr1->name!=itr2->name)
     { throw std::runtime_error("Probleme de name de chunk") ; }
    if (itr1->flavor!=itr2->flavor)
     { throw std::runtime_error("Probleme de flavor de chunk") ; }
    if (itr1->version!=itr2->version)
     { throw std::runtime_error("Probleme de version de chunk") ; }
    if (itr1->columns!=itr2->columns)
     { throw std::runtime_error("Probleme de columns") ; }
    diff_contenu("Probleme de content",itr1->content,itr2->content) ;
   }

  // reutilisation du meme objet pour deux reading de suite


  return 0 ;
 }




