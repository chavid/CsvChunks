
#include "chunks_file.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

struct Section
 {
  std::string nom ;
  std::string variante ;
  std::string version ;
  std::vector<std::string> colonnes ;
  std::vector<std::size_t> largeurs ;
  std::vector<std::vector<std::string>> contenu ;
 } ;

void lecture
 ( const std::string & chemin,
   std::vector<Section> & sections,
   ChunksFile & finput
 )
 {
  if (!(finput.open(chemin)))
   {
    std::ostringstream oss ;
    oss<<"echec à l'ouverture en lecture du fichier : "<<chemin<<std::endl ;
    throw std::runtime_error(oss.str()) ;
   }

  while (finput.next_chunk())
   {
    if ((finput.chunk_name()!="CANDIDATS")||(finput.chunk_version()!="v5"))
     { continue ; }
    Section section ;
    section.nom = finput.chunk_name() ;
    section.variante = finput.chunk_flavor() ;
    section.version = finput.chunk_version() ;
    section.colonnes = finput.chunk_columns() ;
    for ( auto colonne : section.colonnes )
      section.largeurs.push_back(colonne.size()) ;
    while (finput.read_next_line())
     {
      std::vector<std::string> ligne ;
      std::string mot ;
      std::size_t colonne = 0 ;
      while (finput>>mot)
       {
        std::cout<<mot<<" " ;
        ligne.push_back(mot) ;
        if ((colonne<section.largeurs.size())&&(mot.size()>section.largeurs[colonne]))
         { section.largeurs[colonne] = mot.size() ; }
        colonne++ ;
       }
      std::cout<<std::endl ;
      section.contenu.push_back(ligne) ;
     }
    sections.push_back(section) ;
   }
 }

void ecriture
 ( const std::string & chemin,
   const std::vector<Section> & sections,
   ChunksFile & foutput  
 )
 {
  if (!(foutput.open(chemin,"WRITE")))
   {
    std::ostringstream oss ;
    oss<<"echec à l'ouverture en ecriture du fichier : "<<chemin<<std::endl ;
    throw std::runtime_error(oss.str()) ;
   }
  
  for ( auto section : sections )
   {
    foutput.chunk_name(section.nom) ;
    foutput.chunk_flavor(section.variante) ;
    foutput.chunk_version(section.version) ;
    foutput.chunk_columns(section.colonnes) ;
    foutput.change_format(section.largeurs) ;
    foutput.chunk_write() ;
    for ( auto ligne : section.contenu )
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
  ChunksFile ft(true) ;
  ft.open("chunks_file.in.csv") ; ft.read_next_line() ; element = "" ; ft>>element ; std::cout<<"Premier element : "<<element<<std::endl ;
  ft.open("chunks_file.in.csv") ; ft.read_next_line() ; element = "" ; ft>>element ; std::cout<<"Premier element : "<<element<<std::endl ;

  // reading csv chunks
  std::vector<Section> sections1, sections2 ;

  lecture("chunks_file.in.csv",sections1,ft) ;
  ecriture("chunks_file.out.csv",sections1,ft) ;
  lecture("chunks_file.out.csv",sections2,ft) ;

  if (sections1.size()!=sections2.size())
   { throw std::runtime_error("Probleme de nombre de sections") ; }

  std::vector<Section>::const_iterator itr1, itr2 ;
  for ( itr1 = sections1.begin(), itr2 = sections2.begin() ;
        itr1 != sections1.end() ; ++itr1, ++itr2 )
   {
    if (itr1->nom!=itr2->nom)
     { throw std::runtime_error("Probleme de nom de section") ; }
    if (itr1->variante!=itr2->variante)
     { throw std::runtime_error("Probleme de variante de section") ; }
    if (itr1->version!=itr2->version)
     { throw std::runtime_error("Probleme de version de section") ; }
    if (itr1->colonnes!=itr2->colonnes)
     { throw std::runtime_error("Probleme de colonnes") ; }
    diff_contenu("Probleme de contenu",itr1->contenu,itr2->contenu) ;
   }

  // reutilisation du meme objet pour deux lecture de suite


  return 0 ;
 }




