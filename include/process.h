#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid();                               // See src/process.cpp
  std::string User();                      // See src/process.cpp
  std::string Command();                   // See src/process.cpp
  float CpuUtilization();                  // See src/process.cpp
  std::string Ram();                       // See src/process.cpp
  long int UpTime();                       // See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

//   Process() {}
  Process(int id) : id_{id} { cpu_util_ = CpuUtilization();}
  // Declare any necessary private members
 private:
  int id_;
  float cpu_util_;
};

#endif