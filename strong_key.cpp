
#include "strong_key.h"
#include <set>

#define DISPLAY(EXPR) std::cout<<#EXPR<<" : "<<(EXPR)<<std::endl

using Collection = std::set<int> ;
using Indice = const int * ;
using Key1 = StrongKey<Indice,struct IndiceTag1> ;
using Key2 = StrongKey<Indice,struct IndiceTag2> ;

int main()
 {
  Collection col { 1, 2, 3, 4, 5, 6 } ;

  Key1 i1 { &*col.find(1) } ;
  Key1 i2 { &*col.find(3) } ;
  Key1 i3 { &*col.find(5) } ;
  
  Key2 j1 { &*col.find(2) } ;
  Key2 j2 { &*col.find(4) } ;
  Key2 j3 { &*col.find(6) } ;

  std::cout<<"i: "<<(*i1.value())<<" "<<(*i2.value())<<" "<<(*i3.value())<<std::endl ;
  std::cout<<"j: "<<(*j1.value())<<" "<<(*j2.value())<<" "<<(*j3.value())<<std::endl ;

  //DISPLAY(i1==j1) ;
  DISPLAY(((*(i1.value()))+1)==(*(j1.value()))) ;
  DISPLAY(i2==i3) ;
  DISPLAY(j2<j3) ;

  return 0 ;
 }
 