#include <string>

#include "format.h"

using std::string;

// Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
  int H = seconds / (3600);
  int M = (seconds % 3600) / 60;
  int S = seconds % 60;
  return std::to_string(H) + ":" + std::to_string(M) + ":" + std::to_string(S); 
}