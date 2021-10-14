#include <string>
#include <iomanip>

#include "format.h"

using std::string;

// Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
  int H = seconds / (3600);
  int M = (seconds % 3600) / 60;
  int S = seconds % 60;
  
  std::stringstream ss{};
  ss << std::setw(2) << std::setfill('0') << H //hours.count()     // HH
     << std::setw(1) << ":"                                    // :
     << std::setw(2) << std::setfill('0') << M //minutes.count()   // MM
     << std::setw(1) << ":"                                    // :
     << std::setw(2) << std::setfill('0') << S; //seconds.count();  // SS
//   return std::to_string(H) + ":" + std::to_string(M) + ":" + std::to_string(S); 
  return ss.str();
}