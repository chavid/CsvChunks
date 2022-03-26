
#include "strong_array.h"
#include "glossary.h"
#include <iostream>

using Size1 = StrongInt<int,struct Size1Tag> ;
using Array1 = StrongArray1D<Size1,int> ;

using Size2 = StrongInt<int,struct Size2Tag> ;
using Array2 = StrongArray1D<Size2,int> ;

// Enum
using FiliereEnum = Enum<struct FiliereTag> ;
template<> FiliereEnum::EnumGlossary FiliereEnum::glo__ {} ;
FiliereEnum const MP { "MP", true } ;
FiliereEnum const PC { "PC", true } ;
FiliereEnum const ENS { "ENS", true } ;
using Array3 = StrongArray1D<FiliereEnum,int> ;

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

  Array3 data3 ;
  FiliereEnum i30{MP}, i31{PC}, i32{ENS}, s3{FiliereEnum::size()} ;
  data3.resize(s3,0) ;
  data3[i30] = 30 ;
  data3[i31] = 31 ;
  data3[i32] = 32 ;

  //std::cout<<data1[i20] ;
  std::cout<<data1[i10]<<" "<<data1[i11]<<" "<<data1[i12]<<std::endl ;
  std::cout<<data2.at(i20)<<" "<<data2.at(i21)<<" "<<data2.at(i22)<<std::endl ;
  std::cout<<data3[i30]<<" "<<data3[i31]<<" "<<data3[i32]<<std::endl ;

  return 0 ;
 }