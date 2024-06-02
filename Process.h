//Ella Song
#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <string>
#include <unordered_set>

class Process{
public:
    Process() = default;
    Process(int PID);

    void setState(const std::string& state);
    void setParentPID(int PID);
    void addChildPID(int PID);

    int getPID() const;
    std::string getState() const;
    int getParentPID() const;
    std::unordered_set<int> getChildrenPIDs() const;

private:
    int processPID, parentPID;
    std::string currentState;
    std::unordered_set<int> childrenPIDs;
};

#endif