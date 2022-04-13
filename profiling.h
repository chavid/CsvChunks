
#ifndef PROFILING_H
#define PROFILING_H

#include <iostream>
#include <array>
#include <chrono>


//=====================================================================
// Single timing
//=====================================================================

// for a void function
template< typename Fonction, typename... ArgTypes >
void time( std::string_view title, Fonction&& f, ArgTypes&&... args )
 {
  using namespace std::chrono ;
  auto t1 = steady_clock::now() ;

  f(args...) ;
   
  auto t2 = steady_clock::now() ;
  auto dt = duration_cast<milliseconds>(t2-t1).count() ;
  std::cout<<"("<<title<<": "<<dt<<" ms)"<<std::endl ;
 }

// for a non-void function
template< typename Fonction, typename... ArgTypes >
auto time_return( std::string_view title, Fonction&& f, ArgTypes&&... args )
 {
  using namespace std::chrono ;
  auto t1 = steady_clock::now() ;

  auto res = f(args...) ;
   
  auto t2 = steady_clock::now() ;
  auto dt = duration_cast<milliseconds>(t2-t1).count() ;
  std::cout<<"("<<title<<": "<<dt<<" ms)"<<std::endl ;

  return res ;
 }
 

//=====================================================================
// Accumulated timing
//=====================================================================

extern std::array<std::chrono::microseconds,10> time_compteurs ;

void time_init( int i ) ;

template< typename Fonction, typename... ArgTypes >
void time_add( int i, Fonction&& f, ArgTypes&&... args )
 {
  using namespace std::chrono ;
  auto t1 = steady_clock::now() ;
  f(args...) ;
  auto t2 = steady_clock::now() ;
  time_compteurs[i] += duration_cast<microseconds>(t2-t1) ;
 }
 
template< typename Fonction, typename... ArgTypes >
auto time_add_return( int i, Fonction&& f, ArgTypes&&... args )
 {
  using namespace std::chrono ;
  auto t1 = steady_clock::now() ;
  auto res = f(args...) ;
  auto t2 = steady_clock::now() ;
  time_compteurs[i] += duration_cast<microseconds>(t2-t1) ;
  return res ;
 }
 
void time_display( std::string_view title, int i ) ;


#endif

