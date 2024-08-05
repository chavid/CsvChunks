

#ifndef RANKED_H
#define RANKED_H


#include "strong_int.h"
#include <vector>
#include <utility>
#include <exception>


// T is meant to inherit from RankedItem<T>, and should be
// built with RankedCollection<T>::make. This way, any
// instance of T has a stringly types rank within its
// collection, which can be used as index in strong arrays.

template<typename T>
struct RankedCollection : public std::vector<T *>
 {
  using Collection = std::vector<T *> ;
  using Rank = StrongInt<typename Collection::size_type,T> ;
  Rank begin_rank() const { return Rank(1) ; }
  Rank end_rank() const { return Rank(this->size()+1) ; }
  T const * get( Rank r ) const
   {
    typename Collection::size_type val = r.value() ;
    if (val==0) throw std::runtime_error("Not really implemented") ;
    else return this->operator[](val-1) ;
   }
  T * get( Rank r )
   {
    typename Collection::size_type val = r.value() ;
    if (val==0) throw std::runtime_error("Not really implemented") ;
    else return this->operator[](val-1) ;
   }

  template< typename ... Args >
  T * make( Args && ... args )
   {
    auto * element = new T(std::forward<Args>(args) ...) ;
    this->push_back(element) ;
    element->rank_ = Rank(this->size()) ;
    return element ;
   }

  void clear() 
   {
    for ( auto * element : *this )
     { delete element ; }
    std::vector<T *>::clear() ;
   }

 } ;

template<typename T>
class RankedItem
 {
  public:
    using Rank = typename RankedCollection<T>::Rank  ;
    Rank rank() const { return rank_ ; }
  private:
    friend struct RankedCollection<T> ;
    Rank rank_ {0} ;
 } ;

// Below is a concept to check that a type is a RankedItem,
// and a specialization of std::less for pointers to RankedItem,
// so that when using pointers to RankedItem in a std::set or
// std::map, they are sorted by rank.

template<typename T>
concept IsRankedItem = std::is_base_of_v<RankedItem<T>, T>;

template <IsRankedItem T>
struct std::less<T const *>
 {
  bool operator()( T const * i1, T const * i2 ) const
   { return i1->rank()<i2->rank() ; }
 } ;


#endif


