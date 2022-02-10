
#ifndef STRONG_ARRAY_H
#define STRONG_ARRAY_H

#include "strong_int.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <typeinfo>


//===================================================
// Collection using a strong integral as index, so
// that one cannot misuse an index with the wrong
// collection.
// TODO : since we always go through resize(), is
// std::vector the very best choice ?
//===================================================


template <strong_int TStrongInt, typename TValue>
class StrongArray1D
 {
  public :

    using StrongInt = TStrongInt ;
    using Value = TValue ;
    using Collection = std::vector<Value> ;
    using Reference = typename Collection::reference ;
    using ConstReference = typename Collection::const_reference ;

    StrongArray1D() {}

    void resize( StrongInt e, Value const & v ) ;

    Reference operator[]( StrongInt e )
     { return collection_[e.value()] ; }
    ConstReference operator[]( StrongInt e ) const
     { return collection_[e.value()] ; }

    Reference at( StrongInt e )
     { return collection_.at(e.value()) ; }
    ConstReference at( StrongInt e ) const
     { return collection_.at(e.value()) ; }

    typename Collection::size_type size() const
     { return collection_.size() ; }

  private :

    Collection collection_ ;

 } ;

// je commence par un clear pour m'assurer que la valeur
// v sera diffusée dans tous les éléments
template <strong_int TStrongInt, typename TValue>
void StrongArray1D<TStrongInt,TValue>::resize( TStrongInt e, TValue const & v )
 {
  collection_.clear() ;
  collection_.resize(e.value(),v) ;
 }



//===================================================
// Same in 2D
//===================================================


template <strong_int TStrongInt1, strong_int TStrongInt2, typename TValue>
class StrongArray2D
 {
  public :

	  using StrongInt1 = TStrongInt1 ;
	  using StrongInt2 = TStrongInt2 ;
	  using Value = TValue ;
    using Collection = std::vector<Value> ;
    using Indice = typename Collection::size_type ;
    using Reference = typename Collection::reference ;
    using ConstReference = typename Collection::const_reference ;

    StrongArray2D() ;

    void resize( StrongInt1 s1, StrongInt2 s2, Value const & v ) ;

    Reference at( StrongInt1 e1, StrongInt2 e2 ) ;
    ConstReference at( StrongInt1 e1, StrongInt2 e2 ) const ;

  private :

    Indice s1s2_ ;
    Indice s2_ ;
    Collection collection_ ;

 } ;

template <strong_int TSI1, strong_int TSI2, typename TV>
StrongArray2D<TSI1,TSI2,TV>::StrongArray2D()
 {}

template <strong_int TSI1, strong_int TSI2, typename TV>
void StrongArray2D<TSI1,TSI2,TV>::resize( TSI1 s1, TSI2 s2, TV const & v )
 {
  s1s2_ = s1.value()*s2.value() ;
  s2_ = s2.value() ;
  collection_.clear() ;
  collection_.resize(s1s2_,v) ;
 }

template <strong_int TSI1, strong_int TSI2, typename TV>
typename StrongArray2D<TSI1,TSI2,TV>::Reference StrongArray2D<TSI1,TSI2,TV>::at( TSI1 e1, TSI2 e2 )
 {
  if (e2.value()>=s2_)
   {
    std::ostringstream oss ;
    oss<<"[StrongArray2D::at] second indice trop grand : "<<e2<<std::ends ;
    throw std::range_error(oss.str()) ;
   }
  return collection_.at(e1.value()*s2_+e2.value()) ;
 }

template <strong_int TSI1, strong_int TSI2, typename TV>
typename StrongArray2D<TSI1,TSI2,TV>::ConstReference StrongArray2D<TSI1,TSI2,TV>::at( TSI1 e1, TSI2 e2 ) const
 {
  if (e2.value()>=s2_)
   {
    std::ostringstream oss ;
    oss<<"[StrongArray2D::at] second indice trop grand : "<<e2<<std::ends ;
    throw std::range_error(oss.str()) ;
   }
  return collection_.at(e1.value()*s2_+e2.value()) ;
 }


//===================================================
// La même chose, en 3D
//===================================================


template <strong_int TStrongInt1, strong_int TStrongInt2, strong_int TStrongInt3, typename TValue>
class StrongArray3D
 {
  public :

	  using StrongInt1 = TStrongInt1 ;
	  using StrongInt2 = TStrongInt2 ;
	  using StrongInt3 = TStrongInt3 ;
	  using Value = TValue ;
    using Collection = std::vector<Value> ;
    using Indice = typename Collection::size_type ;
    using Reference = typename Collection::reference ;
    using ConstReference = typename Collection::const_reference ;

    StrongArray3D() ;

    void resize( StrongInt1 s1, StrongInt2 s2, StrongInt3 s3, Value const & v ) ;

    Reference at( StrongInt1 e1, StrongInt2 e2, StrongInt3 e3 ) ;
    ConstReference at( StrongInt1 e1, StrongInt2 e2, StrongInt3 e3 ) const ;

  private :

    Indice s1_{}, s2_{}, s3_{}, s2s3_{}, s1s2s3_{} ;
    Collection collection_ ;

 } ;

template <strong_int TSI1, strong_int TSI2, strong_int TSI3, typename TV>
StrongArray3D<TSI1,TSI2,TSI3,TV>::StrongArray3D()
 {}

template <strong_int TSI1, strong_int TSI2, strong_int TSI3, typename TV>
void StrongArray3D<TSI1,TSI2,TSI3,TV>::resize( TSI1 s1, TSI2 s2, TSI3 s3, TV const & v )
 {
  s1_ = s1.value() ; s2_ = s2.value() ; s3_ = s3.value() ;
  s2s3_ = s2_*s3_ ;
  s1s2s3_ = s1_*s2_*s3_ ;
  collection_.clear() ;
  collection_.resize(s1s2s3_,v) ;
 }

template <strong_int TSI1, strong_int TSI2, strong_int TSI3, typename TV>
typename StrongArray3D<TSI1,TSI2,TSI3,TV>::Reference StrongArray3D<TSI1,TSI2,TSI3,TV>::at( TSI1 e1, TSI2 e2, TSI3 e3 )
 {
  if (e1.value()>=s1_)
   {
    std::ostringstream oss ;
    oss<<"[StrongArray3D::at] premier indice trop grand : "<<e1<<std::ends ;
    throw std::range_error(oss.str()) ;
   }
  if (e2.value()>=s2_)
   {
    std::ostringstream oss ;
    oss<<"[StrongArray3D::at] second indice trop grand : "<<e2<<std::ends ;
    throw std::range_error(oss.str()) ;
   }
  if (e3.value()>=s3_)
   {
    std::ostringstream oss ;
    oss<<"[StrongArray3D::at] troisieme indice trop grand : "<<e3<<std::ends ;
    throw std::range_error(oss.str()) ;
   }
  return collection_.at(e1.value()*s2s3_+e2.value()*s3_+e3.value()) ;
 }

template <strong_int TSI1, strong_int TSI2, strong_int TSI3, typename TV>
typename StrongArray3D<TSI1,TSI2,TSI3,TV>::ConstReference StrongArray3D<TSI1,TSI2,TSI3,TV>::at( TSI1 e1, TSI2 e2, TSI3 e3 ) const
 {
  if (e1.value()>=s1_)
   {
    std::ostringstream oss ;
    oss<<"[StrongArray3D::at] premier indice trop grand : "<<e1<<std::ends ;
    throw std::range_error(oss.str()) ;
   }
  if (e2.value()>=s2_)
   {
    std::ostringstream oss ;
    oss<<"[StrongArray3D::at] second indice trop grand : "<<e2<<std::ends ;
    throw std::range_error(oss.str()) ;
   }
  if (e3.value()>=s3_)
   {
    std::ostringstream oss ;
    oss<<"[StrongArray3D::at] troisieme indice trop grand : "<<e3<<std::ends ;
    throw std::range_error(oss.str()) ;
   }
  return collection_.at(e1.value()*s2s3_+e2.value()*s3_+e3.value()) ;
 }





#endif


