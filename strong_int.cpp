
#include "strong_int.h"
#include <iostream>

//using WrongType = StrongInt<float,struct WrongTypeTag> ;
using Indice1 = StrongInt<int,struct Indice1Tag> ;
using Indice2 = StrongInt<int,struct Indice2Tag> ;

int main()
 {
  Indice1 i1{0}, i2{1}, i3{2} ;
  Indice2 j{} ;
  //std::cout<<(j==i1)<<std::endl ;
  std::cout<<i1<<" "<<i2<<" "<<i3<<" "<<j<<std::endl ;
  std::cout<<(j.value()==i1.value())<<std::endl ;
  std::cout<<(i2<i3)<<std::endl ;
  std::cout<<(i3<i2)<<std::endl ;
  //std::cout<<(j<=i1)<<std::endl ;
  return 0 ;
 }