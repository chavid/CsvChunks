

#ifndef STATIC_STRINGS_H
#define STATIC_STRINGS_H

#include "strong_key.h"
#include <string>
#include <string_view>
#include <unordered_set>
#include <memory_resource>


//===================================================
// Replace frequently used strings with a pointer
// within a static structure. The strings are made
// uppercase, so that there comparison is case
// insensitive.
//===================================================

class StaticStrings
 {
  public :

    using String = std::pmr::string ;
    using Collection = std::unordered_set<String> ;
    using Ptr = StrongKey<String const *, struct StaticStringsFoo> ;

    static void init() ;
    static Ptr empty_string_ptr()
     { return empty_string_ptr_ ; } 
    static std::size_t size() ;

    static Ptr ptr( std::string_view ) ;
    static std::string_view str( Ptr ptr )
     { return *ptr.value() ; }
    
  private:
   
    static std::pmr::polymorphic_allocator<char> * char_allocator_p_ ;
    static Collection strings_ ;
    static Ptr empty_string_ptr_ ;
    static bool init_done_ ;

 } ;


#endif

