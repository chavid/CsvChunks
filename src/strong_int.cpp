
#include "strong_int.h"
#include <sstream>
#include <format>
#include <unordered_set>

#define DISPLAY(EXPR) std::cout<<#EXPR<<" : "<<(EXPR)<<std::endl
#define BOOL(VAR) std::cout<<#VAR<<" is "<<(VAR?"true":"false")<<std::endl ;

//using WrongType = StrongInt<float,struct WrongTypeTag> ;
using Indice1 = StrongInt<int,struct Indice1Tag> ;
using Indice2 = StrongInt<int,struct Indice2Tag> ;
using Indice3 = StrongInt<Indice2,struct Indice3Tag> ;

int main()
 {
  Indice1 i1, i2, i3 ;
  std::istringstream iss {"0 1 2"} ;
  iss>>i1>>i2>>i3 ;
  Indice2 j1 {}, j2 {1}, j3 {2} ;
  Indice3 k1 {j1}, k2{j2}, k3 ;

  std::cout<<"i: "<<i1<<" "<<i2<<" "<<i3<<std::endl ;
  std::cout<<"j: "<<j1<<" "<<j2<<" "<<j3<<std::endl ;
  std::cout<<std::format("k: {} {} {}",k1,k2,k3)<<std::endl ;

  BOOL(i1) ; BOOL(i2) ; BOOL(i3) ;
  BOOL(j1) ; BOOL(j2) ; BOOL(j3) ;
  BOOL(k1) ; BOOL(k2) ; BOOL(k3) ;

  //DISPLAY(i1==j1) ;
  DISPLAY(i2.value()==j2.value()) ;
  DISPLAY(i2==i3) ;
  DISPLAY(k1==k3) ;
  DISPLAY(++k3) ;
  DISPLAY(k2==k3) ;

  DISPLAY(i2<i3) ;
  DISPLAY(k3<k2) ;
  DISPLAY(k3<=k2) ;

  std::unordered_set<Indice1> i1set ;
  i1set.insert(i1) ;
  i1set.insert(i2) ;
  i1set.insert(i3) ;
  for ( auto const & i : i1set )
   { std::cout<<i<<std::endl ; }

  return 0 ;
 }