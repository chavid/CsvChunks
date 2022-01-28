
#include "strong_array.h"
#include <iostream>

using Size1 = StrongInt<int,struct Size1Tag> ;
using Array1 = StrongArray1D<Size1,int> ;

using Size2 = StrongInt<int,struct Size2Tag> ;
using Array2 = StrongArray1D<Size2,int> ;

int main()
 {
  Array1 data1 ;
  Size1 i10{0}, i11{1}, i12{2}, s1{3} ;
  data1.resize(s1,0) ;
  data1[i10] = 10 ;
  data1[i11] = 11 ;
  data1[i12] = 12 ;

  Array2 data2 ;
  Size2 i20{0}, i21{1}, i22{2}, s2{3} ;
  data2.resize(s2,0) ;
  data2[i20] = 20 ;
  data2[i21] = 21 ;
  data2[i22] = 22 ;

  //std::cout<<data1[i20] ;
  std::cout<<data1[i10]<<" "<<data1[i11]<<" "<<data1[i12]<<std::endl ;
  std::cout<<data2.at(i20)<<" "<<data2.at(i21)<<" "<<data2.at(i22)<<std::endl ;

  return 0 ;
 }