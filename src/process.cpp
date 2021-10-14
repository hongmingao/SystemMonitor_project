#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { 
  return id_; 
}

// Return this process's CPU utilization
float Process::CpuUtilization() { 
  float uptime = static_cast<float>(LinuxParser::UpTime()); // in seconds
  vector<float> cpu_util_data;
  cpu_util_data = LinuxParser::ProcessCpuUtilization(id_);
  // not include the time from children processes
  float process_time_s {(cpu_util_data[0] + cpu_util_data[1]) 
                        / sysconf(_SC_CLK_TCK)};
  float elapse_time_s { uptime - cpu_util_data[4] / sysconf(_SC_CLK_TCK) };
  cpu_util_ = process_time_s / elapse_time_s; // as a fraction (NOT percentage)
  return cpu_util_;
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(id_); }

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(id_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(id_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime() - LinuxParser::UpTime(id_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
  return this->cpu_util_ < a.cpu_util_ ; 
}