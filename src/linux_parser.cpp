#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

#include <iostream>
#include <cassert>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  string value;
  string unit;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  
  vector<double> memories(2); // memories[0] is MemTotal, memories[1] is MemFree
  
  if (stream.is_open()) {
    for (int i = 0; i < 2; i++) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> value >> unit;
//       std::cout << "mem = " << value << "\n";
      memories[i] = std::stod(value);
    }
  }
  return (memories[0] - memories[1]) / memories[0]; 
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  string sys_uptime_str;
  string sys_idletime_str;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> sys_uptime_str >> sys_idletime_str;
  }
  return std::stol(sys_uptime_str); 
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  string line;
  string key;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  long jiffy_tot{0};
  
  assert(stream.is_open());
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
//     std::cout << "CPU: " << key << " ";
//     for (string time : CPU_utilization) {
    for (int i = 0; i < 8; i++) { 
      string s;
      linestream >> s;
      jiffy_tot += stol(s);
//       std::cout << s << " ";
//       CPU_utilization[i] = s;
    }
  }
  
  return jiffy_tot; 
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  string value;
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  long jiffy_proc_active {0};
  
  assert(stream.is_open());
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    int i{0};
    vector<int> indices {13,14};
    while (linestream >> value || i < 22) {
      if (std::find(indices.begin(), indices.end(), i) != indices.end()) {
        jiffy_proc_active + stol(value);
      }
      i++;
    }
//     std::cout << "CPU: " << key << " ";
  }
  return jiffy_proc_active;  
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string line;
  string key;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  long jiffy_idle{0};
  
  assert(stream.is_open());
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
//     std::cout << "CPU: " << key << " ";
//     for (string time : CPU_utilization) {
    for (int i = 0; i < 8; i++) { 
      if (i == 3 || i == 4) {
        string s;
        linestream >> s;
        jiffy_idle += stol(s);
      }
//       std::cout << s << " ";
//       CPU_utilization[i] = s;
    }
  }
  
  return jiffy_idle;  
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  string key;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  vector<string> CPU_utilization(10);
  
  assert(stream.is_open());
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    for (int i = 0; i < 10; i++) { 
      linestream >> CPU_utilization[i];
    }
  }
  return CPU_utilization; 
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return stoi(value);
        }
      }
    }
  }
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string value {"2"};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return stoi(value);
        }
      }
    }
  }
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string cmdline;
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  assert(stream.is_open());
  if (stream.is_open()) {
    std::stringstream strStream;
    strStream << stream.rdbuf();
    cmdline = strStream.str();
  }
  return cmdline; 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string ram_use, temp;
  string unit;
  
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  assert(stream.is_open());
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> ram_use >> unit;
      if (key.substr(0, 3) == "VmS") { break; }
    }
    // it's still in units of kB
    temp = std::to_string(stof(ram_use)/1000);
  }
  return temp; 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string uid;
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  assert(stream.is_open());
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key.substr(0,3) == "Uid") {linestream >> uid; break;}
    }
  }
  return uid; 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string uid;
  uid = LinuxParser::Uid(pid);
  
  string line;
  string usrname1, usrname2;
  string x_str;
  string id1, id2;
  string directory1, directory2;
  
  std::ifstream stream(kPasswordPath);
  assert(stream.is_open());
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> usrname1 >> x_str >> id1 >> id2 >> usrname2 >> directory1 >> directory2) {
        if (id1 == uid) {
          return usrname1;
        }
      }
    }
  }
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string line;
  string key;
  string uptime;
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  assert(stream.is_open());
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; i++) { linestream >> uptime; }
  }
  return stol(uptime)/ sysconf(_SC_CLK_TCK); 
}

// Read and return CPU utilization for processes
vector<float> LinuxParser::ProcessCpuUtilization(int pid) { 
  string line;
  string value;
  
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  vector<float> CPU_utilization {};
  
  assert(stream.is_open());
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    int i{0};
    vector<int> indices {13,14,15,16,21};
    while (linestream >> value || i < 22) {
      if (std::find(indices.begin(), indices.end(), i) != indices.end()) {
        CPU_utilization.push_back(stof(value));
      }
      i++;
    }
  }
  return CPU_utilization; 
}
