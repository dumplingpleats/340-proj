//Ella Song
#ifndef SIMOS_H
#define SIMOS_H

#include "Disk.h"
#include "Process.h"
#include <vector>
#include <deque>
#include <iostream>
#include <unordered_map>
#include <string>

struct MemoryItem {
    unsigned long long pageNumber;
    unsigned long long frameNumber;
    int PID; // PID of the process using this frame of memory
};

using MemoryUsage = std::vector<MemoryItem>;

constexpr int NO_PROCESS{0};

class SimOS {
public:
    SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize);

    void NewProcess();
    void SimFork();
    void SimExit();
    void SimWait();
    void TimerInterrupt();
    void DiskReadRequest(int diskNumber, std::string fileName);
    void DiskJobCompleted(int diskNumber);
    void AccessMemoryAddress(unsigned long long address);
    int GetCPU();
    std::deque<int> GetReadyQueue();
    MemoryUsage GetMemory();
    FileReadRequest GetDisk(int diskNumber);
    std::deque<FileReadRequest> GetDiskQueue(int diskNumber);

    void terminateChildren(std::unordered_set<int>& childrenPIDs);
    void terminateReadyQueueChildren(const std::unordered_set<int>& childrenPIDs);
    void terminateRAMChildren(const std::unordered_set<int>& childrenPIDs);

    std::unordered_set<int> getFamily(int PID);
    void getFamilyHelper(std::unordered_set<int>& allPIDs, int PID);

    void NoCPUCheck() const;
    void InvalidDiskCheck(int diskNumber) const;
    void fillCPU();

private:
    int currentPID;
    int currentCPU;
    unsigned long long ramSize;
    unsigned int pageSize;
    std::deque<int> readyQueue;
    std::unordered_map<int, Process> allProcesses;
    std::vector<Disk> disks;
    MemoryUsage memUsage;
};

#endif // SIMOS_H