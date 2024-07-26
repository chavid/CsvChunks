
#include "ranked_collection.h"

class DataElement : public RankedItem<DataElement>
 {
  public :
    DataElement( int id ) : id_{id} {}
    int id() const { return id_ ; }
  private :
    int id_ =0 ; 
 } ;

class DataCollection : public RankedCollection<DataElement>
 {} ;

int main( int, const char** )
 {
  DataCollection collection ;
  collection.make(10) ;
  collection.make(20) ;
  collection.make(30) ;

  // iterative access
  for ( auto * element : collection )
   { std::cout<<element->rank()<<" -> "<<element->id()<<std::endl ; }

  // random access
  auto * element0 = collection.get(DataCollection::Rank(1)) ;
  std::cout<<element0->rank()<<" -> "<<element0->id()<<std::endl ;
  auto * element1 = collection.get(DataCollection::Rank(2)) ;
  std::cout<<element1->rank()<<" -> "<<element1->id()<<std::endl ;
  auto * element2 = collection.get(DataCollection::Rank(3)) ;
  std::cout<<element2->rank()<<" -> "<<element2->id()<<std::endl ;

  // nettoyage
  collection.clear() ;
 }




