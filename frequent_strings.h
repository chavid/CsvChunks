
#ifndef FREQUENT_STRINGS_H
#define FREQUENT_STRINGS_H

#include "strong_int.h"
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>

//===================================================
// Replace frequently used strings with a rank
// within a static table. The strings are made
// lowercase, so that comparisons of the ranks are 
// case insensitive.
//===================================================

class StaticStrings
 {
  public :

    using Collection = std::vector<std::string> ;
    using Indice = Collection::size_type  ;
    using Rank = StrongInt<Indice,struct StringsFoo> ;

    static Rank rank( std::string_view ) ;
    static const std::string & str( Rank rank )
     { return strings_[rank.value()] ; }
    
  private:
   
    static Collection strings_ ;
    static std::unordered_map<std::string,Rank> ranks_ ;
 } ;

//===================================================
// The class FrequentString encapsulates the access
// to the static table behind usual string behavior.
//===================================================

class FrequentString
 {
  public :
    FrequentString() : rank_{} {}
    explicit FrequentString( std::string_view sv ) : rank_{ StaticStrings::rank(sv) } {}
    bool empty() const { return (rank_==StaticStrings::Rank(0)) ; }
    std::string const & str() const { return StaticStrings::str(rank_) ; }
    std::string::size_type size() const { return StaticStrings::str(rank_).size() ; }
    bool operator==( FrequentString const & other ) const { return rank_==other.rank_ ; }
    bool operator!=( FrequentString const & other ) const { return rank_!=other.rank_ ; }
    bool operator<( FrequentString const & other ) const { return rank_<other.rank_ ; }
    // to make it usable within a StrongInt ;
    using StrongIntInternalType = StaticStrings::Rank ;
    StrongIntInternalType value() const
     { return rank_ ; }
  private :
    StaticStrings::Rank rank_ ;
 } ;

FrequentString operator"" _fs ( const char * str, std::size_t ) ;
std::ostream & operator<<( std::ostream & os, FrequentString ) ;

//===================================================
// Other utilities
//===================================================

template< typename TagType >
class Glossary : public std::vector<std::set<FrequentString>>
 {
  public :
    using Collection = std::vector<std::set<FrequentString>> ;
    using Id = Collection::size_type ;
    explicit Glossary( std::string_view ) ;
    Id operator()( std::string_view ) const ;
    const std::string & operator()( Id ) const ;
 } ;

template< typename TagType >
Glossary<TagType>::Glossary( std::string_view terms_sv )
 {
  std::istringstream terms_iss{std::string{terms_sv}} ;
  std::string synonyms_str ;
  while (std::getline(terms_iss,synonyms_str,';'))
   {
    std::set<FrequentString> synonyms ;
    std::istringstream synonyms_iss{synonyms_str} ;
    std::string term ;
    while (std::getline(synonyms_iss,term,'|'))
     { synonyms.insert(FrequentString{term}) ; }
    push_back(synonyms) ;
   }
 }

template< typename TagType >
Glossary<TagType>::Id Glossary<TagType>::operator()( std::string_view term_sv ) const
 {
  FrequentString term_fs(term_sv) ;
  Id res = 0 ;
  while (res<size())
   {
    if (operator[](res).contains(term_fs))
     { return res ; }
    else
     { ++res ; }
   }
  throw std::runtime_error("Unknown term") ;
 }

template< typename TagType >
std::string const & Glossary<TagType>::operator()( Id term_id ) const
 { return (operator[](term_id).begin())->str() ; }

#endif
