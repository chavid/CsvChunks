
#ifndef GLOSSARY_H
#define GLOSSARY_H

#include "strong_int.h"
#include "frequent_strings.h"
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

//===================================================
// Interface
//===================================================

template< typename TagType >
class Glossary : public std::vector<std::vector<std::string>>
 {
  public :
    using Collection = std::vector<std::vector<std::string>> ;
    using Id = Collection::size_type ;
    constexpr explicit Glossary( std::string_view ) ;
    Id id( std::string_view ) const ;
    const std::string & str( Id ) const ;
 } ;


//===================================================
// Implementation
//===================================================

static void glossary_lower( std::string & str )
 {
  std::transform(str.begin(),str.end(),str.begin(),
   [](unsigned char c){ return std::tolower(c) ; } ) ;
 }

template< typename TagType >
constexpr Glossary<TagType>::Glossary( std::string_view terms_sv )
 {
  std::istringstream terms_iss{std::string{terms_sv}} ;
  std::string synonyms_str ;
  while (std::getline(terms_iss,synonyms_str,';'))
   {
    std::vector<std::string> synonyms ;
    std::istringstream synonyms_iss{synonyms_str} ;
    std::string term ;
    while (std::getline(synonyms_iss,term,'|'))
     {
      glossary_lower(term) ;
      synonyms.push_back(std::move(term)) ;
     }
    push_back(std::move(synonyms)) ;
   }
 }

template< typename TagType >
Glossary<TagType>::Id Glossary<TagType>::id( std::string_view term_sv ) const
 {
  std::string term_str(term_sv) ;
  glossary_lower(term_str) ;
  Id res = 0 ;
  while (res<size())
   {
    auto const & words = operator[](res) ;
    if (find(words.begin(),words.end(),term_str)!=words.end())
     { return res ; }
    else
     { ++res ; }
   }
  throw std::runtime_error("Unknown term") ;
 }

template< typename TagType >
std::string const & Glossary<TagType>::str( Id term_id ) const
 { return (*(operator[](term_id).begin())) ; }

#endif
