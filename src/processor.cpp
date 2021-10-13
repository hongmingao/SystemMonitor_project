#include "processor.h"
#include "linux_parser.h"
#include <vector>

using namespace std;

// Return the aggregate CPU utilization
float Processor::Utilization() { 
  vector<string> cpu_util = LinuxParser::CpuUtilization();
//   vector<string> cpu_util = {"0","1","2","3","4","5","6","7","8","9"};
  float idle = stof(cpu_util[3]) + stof(cpu_util[4]);
  float non_idle = 0.0;
  vector<int> indices = {0, 1, 2, 5, 6, 7};
  for (int ind : indices) { non_idle += stof(cpu_util[ind]); }
  return non_idle / (non_idle + idle) ; 
}