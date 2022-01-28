
#ifndef GLOSSARY_H
#define GLOSSARY_H

#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <sstream>
#include <utility>


//===================================================
// Glossaire
//===================================================

template <typename TagType>
class Glossary
 {

  public:
 
    using Collection = std::vector<std::vector<std::string>> ;
    using Id = Collection::size_type ;
  
    explicit Glossary( std::string_view = "" ) ;
    Id add( std::string_view ) ;
    Id id( std::string_view ) const ;
    Id size() const { return data_.size() ; }
    std::string const & str( Id id ) const { return (*(data_[id].begin())); }

  private:
  
    Id find_( std::string_view ) const ;
    Collection data_ ;

 } ;


//===================================================
// Enum
//===================================================

template <typename TagType>
class Enum
 {
  public :

    using EnumGlossary = Glossary<TagType> ;

    Enum() : id_(glo__.size()) {}
    Enum( std::string_view sv, bool add = false )
     : id_(add?glo__.add(sv):glo__.id(sv)) {}

    friend bool operator==( Enum lhs, Enum rhs ) { return (lhs.id_==rhs.id_) ; }
    friend bool operator!=( Enum lhs, Enum rhs ) { return (lhs.id_!=rhs.id_) ; }
    friend bool operator<( Enum lhs, Enum rhs ) { return (lhs.id_<rhs.id_) ; }

    friend std::ostream & operator<<( std::ostream & os, Enum<TagType> e )
     { return (os<<e.glo__.str(e.id_)) ; }
    friend std::istream & operator>>( std::istream & is, Enum<TagType> & e )
     { std::string str ; if (is>>str) e.id_ = e.glo__.id(str) ; return is ; }

  private :

    EnumGlossary::Id id_ ;
    static EnumGlossary glo__ ;
 } ;



//===================================================
// External implementations
//===================================================

static void glossary_upper(std::string & str)
{
  std::transform(str.begin(), str.end(), str.begin(),
                 [](unsigned char c)
                 { return std::toupper(c); });
}

template <typename TagType>
Glossary<TagType>::Id Glossary<TagType>::find_( std::string_view term_sv ) const
 {
  std::string term_str(term_sv);
  glossary_upper(term_str);
  Id res = 0;
  while (res < data_.size())
   {
    auto const &words = data_[res];
    if (find(words.begin(), words.end(), term_str) != words.end())
     { return res ; }
    else
     { ++res ; }
   }
  return data_.size() ;
 }

template <typename TagType>
Glossary<TagType>::Id Glossary<TagType>::add( std::string_view terms_sv )
 {
  // check there is no double terms
  std::istringstream terms_iss{std::string{terms_sv}} ;
  std::vector<std::string> terms ;
  std::string term ;
  while (std::getline(terms_iss,term,'|'))
  if (find_(term)!=data_.size())
   {
    std::ostringstream oss ;
    oss<<"Redundant term: "<<term ;
    throw std::runtime_error(oss.str()) ;
   }
  else
   {
    glossary_upper(term) ;
    terms.push_back(std::move(term)) ;
   }
  data_.push_back(std::move(terms)) ;
  return (data_.size()-1) ;
 }

template <typename TagType>
Glossary<TagType>::Glossary( std::string_view terms_sv )
{
  std::istringstream terms_iss{std::string{terms_sv}};
  std::string synonyms_str;
  while (std::getline(terms_iss, synonyms_str, ';'))
   { add(synonyms_str) ; }
}

template <typename TagType>
Glossary<TagType>::Id Glossary<TagType>::id( std::string_view term_sv ) const
 {
  Id id = find_(term_sv) ;
  if (id < data_.size())
   { return id ; }
  else
   {
    std::ostringstream oss ;
    oss<<"Unknown term: "<<term_sv ;
    throw std::runtime_error(oss.str()) ;
   }
 }


#endif
