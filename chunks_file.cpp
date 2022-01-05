
#include "util/outils.h" // for VxException
#include "util/afficheur.h"
#include "util/fichier_table.h"

#include <string>
#include <vector>
#include <ostream>

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
   FichierTable & finput
 )
 {
  if (!(finput.ouvrir(chemin)))
   {
    std::ostringstream oss ;
    oss<<"echec à l'ouverture en lecture du fichier : "<<chemin<<endl ;
    throw VxException(oss.str()) ;
   }

  while (finput.nouvelle_section())
   {
    if ((finput.section_nom()!="CANDIDATS")||(finput.section_version()!="v5"))
     { continue ; }
    Section section ;
    section.nom = finput.section_nom() ;
    section.variante = finput.section_variante() ;
    section.version = finput.section_version() ;
    section.colonnes = finput.section_colonnes() ;
    for ( auto colonne : section.colonnes )
      section.largeurs.push_back(colonne.size()) ;
    while (finput.nouvelle_ligne())
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
      std::cout<<endl ;
      section.contenu.push_back(ligne) ;
     }
    sections.push_back(section) ;
   }
 }

void ecriture
 ( const std::string & chemin,
   const std::vector<Section> & sections,
   FichierTable & foutput  
 )
 {
  if (!(foutput.ouvrir(chemin,"WRITE")))
   {
    std::ostringstream oss ;
    oss<<"echec à l'ouverture en ecriture du fichier : "<<chemin<<endl ;
    throw VxException(oss.str()) ;
   }
  
  for ( auto section : sections )
   {
    foutput.section_nom(section.nom) ;
    foutput.section_variante(section.variante) ;
    foutput.section_version(section.version) ;
    foutput.section_colonnes(section.colonnes) ;
    foutput.nouveau_format(section.largeurs) ;
    foutput.section_write() ;
    for ( auto ligne : section.contenu )
     {
      for ( auto mot : ligne )
        { foutput<<mot ; }
      foutput.saut_de_ligne() ;
     }

    foutput.saut_de_ligne() ;
   }
 
 }

void diff_ligne
 ( const std::string & message,
   const std::vector<std::string> & ligne1,
   const std::vector<std::string> & ligne2
 )
 {
  if (ligne1.size()!=ligne2.size())
   { throw VxException(message) ; }
  auto imotmax {ligne1.size()} ;
  decltype(imotmax) imot {0} ;
  for ( ; imot<imotmax ; ++imot )
   {
    if (ligne1[imot]!=ligne2[imot])
     { throw VxException(message) ; }
   } 
 }

void diff_contenu
 ( const std::string & message,
   const std::vector<std::vector<std::string>> & contenu1,   
   const std::vector<std::vector<std::string>> & contenu2
 )
 {
  if (contenu1.size()!=contenu2.size())
   { throw VxException(message) ; }
  auto ilignemax {contenu1.size()} ;
  decltype(ilignemax) iligne {0} ;
  for ( ; iligne<ilignemax ; ++iligne )
   { diff_ligne(message,contenu1[iligne],contenu2[iligne]) ; } 
 }

int main( int argc, const char * * argv )
 {
  Afficheur message("main> ","./.fichier_table.log") ;
  Afficheur::change_defaut(&message) ;

  // verification des arguments de la ligne de commande
  if (argc<2)
   { throw VxException("Quel fichier voulez-vous ouvrir ?") ; }
  if (argc>2)
   { throw VxException("Trop d'arguments sur la ligne de commande !") ; }

  // tests de reutilisation de FichierTable avec ventilx.cfg
  std::string element ;
  FichierTable ft(true) ;
  ft.ouvrir("ventilx.cfg") ; ft.nouvelle_ligne() ; element = "" ; ft>>element ; std::cout<<"Premier element : "<<element<<endl ;
  ft.ouvrir("ventilx.cfg") ; ft.nouvelle_ligne() ; element = "" ; ft>>element ; std::cout<<"Premier element : "<<element<<endl ;

  // test de contenu
  std::vector<Section> sections1, sections2 ;

  lecture(argv[1],sections1,ft) ;
  ecriture("fichier_table.csv",sections1,ft) ;
  lecture("fichier_table.csv",sections2,ft) ;

  if (sections1.size()!=sections2.size())
   { throw VxException("Probleme de nombre de sections") ; }

  std::vector<Section>::const_iterator itr1, itr2 ;
  for ( itr1 = sections1.begin(), itr2 = sections2.begin() ;
        itr1 != sections1.end() ; ++itr1, ++itr2 )
   {
    if (itr1->nom!=itr2->nom)
     { throw VxException("Probleme de nom de section") ; }
    if (itr1->variante!=itr2->variante)
     { throw VxException("Probleme de variante de section") ; }
    if (itr1->version!=itr2->version)
     { throw VxException("Probleme de version de section") ; }
    if (itr1->colonnes!=itr2->colonnes)
     { throw VxException("Probleme de colonnes") ; }
    diff_contenu("Probleme de contenu",itr1->contenu,itr2->contenu) ;
   }

  // reutilisation du meme objet pour deux lecture de suite


  return 0 ;
 }




