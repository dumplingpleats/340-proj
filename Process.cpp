//Ella Song
#include "Process.h"

Process::Process(int PID) 
: processPID(PID), parentPID(0), currentState("new") {}

void Process::setState(const std::string& state) {
    currentState = state;
}
    
void Process::setParentPID(int PID) {
    parentPID = PID;
}
    
void Process::addChildPID(int PID) {
    childrenPIDs.insert(PID);
}

int Process::getPID() const {
    return processPID;
}
    
std::string Process::getState() const {
    return currentState;
}
    
int Process::getParentPID() const {
    return parentPID;
}
    
std::unordered_set<int> Process::getChildrenPIDs() const {
    return childrenPIDs;
}