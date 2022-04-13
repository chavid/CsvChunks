

#include "profiling.h"


std::array<std::chrono::microseconds,10> time_compteurs ;

void time_init( int i )
 { time_compteurs[i] = std::chrono::microseconds::zero() ; } 

void time_display( std::string_view title, int i )
 {
  using namespace std::chrono ;
  std::cout<<"("<<title<<": "<<duration_cast<milliseconds>(time_compteurs[i]).count()<<" ms)"<<std::endl ;
 }
