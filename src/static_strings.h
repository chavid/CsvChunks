

#ifndef STATIC_STRINGS_H
#define STATIC_STRINGS_H

#include "strong_int.h"
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
//#include <memory_resource>


//==================================================================
// Replace frequently used strings with an index in a static vector.
// I also tried with pointers in a set, but this appeared less
// reproductible. The strings are made uppercase, so that there
// comparison is case insensitive. I also made a not convincing
// try with std::pmr, probably due to my lack of understanding.
//==================================================================

class StaticStrings
 {
  public :

    using String = std::string ;
    using Collection = std::vector<String> ;
    using Indice = Collection::size_type  ;
    using Rank = StrongInt<Indice, struct StaticStringsFoo> ;

    //static void init() ;
    //static Ptr empty_string_ptr()
    // { return empty_string_ptr_ ; } 
    //static std::size_t size() ;

    static Rank rank( std::string_view ) ;
    static String const & str( Rank rank )
     { return strings_[rank.value()] ; }
    
  private:
   
    //static std::pmr::polymorphic_allocator<char> * char_allocator_p_ ;
    static Collection strings_ ;
    static std::unordered_map<String,Rank> ranks_ ;
    //static bool init_done_ ;

 } ;


#endif

