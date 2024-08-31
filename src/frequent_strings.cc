

#include "frequent_strings.h"
#include <sstream>
#include <algorithm>


//===================================================
// FrequentString
//===================================================

void FrequentString::init()
 { /*StaticStrings::init()*/ ; }

FrequentString operator"" _fs ( char const * str, std::size_t )
 { return FrequentString(str) ; }

std::ostream & operator<<( std::ostream & os, FrequentString fs )
 { return (os<<fs.str()) ; }

std::istream & operator>>( std::istream & is, FrequentString & fs )
 {
  std::string str ;
  if (is>>str) fs = FrequentString(str) ;
  return is ;
 }


