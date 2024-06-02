//Ella Song
#include "SimOS.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

SimOS::SimOS(int numberOfDisks, unsigned long long amountOfRAM, unsigned int pageSize)
: currentPID(1), currentCPU(NO_PROCESS), ramSize(amountOfRAM), pageSize(pageSize), disks{numberOfDisks} {}

void SimOS::NewProcess() {
    readyQueue.push_back(currentPID);
    
    allProcesses[currentPID] = {currentPID};
    allProcesses[currentPID].setState("ready");

    fillCPU();
    currentPID++;
}

void SimOS::SimFork() {
    NoCPUCheck();

    readyQueue.push_back(currentPID);

    allProcesses[currentPID] = {currentPID};
    allProcesses[currentPID].setState("ready");
    allProcesses[currentPID].setParentPID(currentCPU);

    allProcesses[currentCPU].addChildPID(currentPID);

    fillCPU();
    currentPID++;
}

void SimOS::SimExit() {
    NoCPUCheck();

    std::unordered_set<int> childrenPIDs = getFamily(currentCPU);
    terminateChildren(childrenPIDs);

    if (allProcesses[currentCPU].getParentPID() != 0) {
        if (allProcesses[allProcesses[currentCPU].getParentPID()].getState() == "waiting") {
            readyQueue.push_back(allProcesses[currentCPU].getParentPID());
            allProcesses[readyQueue.back()].setState("ready");

            allProcesses[currentCPU].setState("terminated");
        }
        else {
            allProcesses[currentCPU].setState("zombie");
        }
    }
    else {
        allProcesses[currentCPU].setState("terminated");
    }

    childrenPIDs.insert(currentCPU);


    currentCPU = NO_PROCESS;
    fillCPU();
}

void SimOS::SimWait() {
    NoCPUCheck();

    std::unordered_set<int> children = allProcesses[currentCPU].getChildrenPIDs();
    bool canWorkAgain = false;

    for (auto child : children) {
        std::string state = allProcesses[child].getState();

        if (state == "zombie") {
            std::unordered_set<int> toBeKilled = getFamily(child);
            toBeKilled.insert(child);

            allProcesses[child].setState("terminated");
            terminateChildren(toBeKilled);

            canWorkAgain = true;
            break;
        }
    }

    if (!canWorkAgain) {
        currentCPU = NO_PROCESS;
    }

    fillCPU();
}

void SimOS::TimerInterrupt() {
    NoCPUCheck();

    readyQueue.push_back(currentCPU);
    allProcesses[currentCPU].setState("ready");

    currentCPU = NO_PROCESS;
    fillCPU();
}

void SimOS::DiskReadRequest(int diskNumber, std::string fileName) {
    InvalidDiskCheck(diskNumber);
    NoCPUCheck();

    FileReadRequest request{currentCPU, fileName};
    disks[diskNumber].addIOFile(request);
    allProcesses[currentCPU].setState("waiting");

    currentCPU = NO_PROCESS;
    fillCPU();
}

void SimOS::DiskJobCompleted(int diskNumber) {
    InvalidDiskCheck(diskNumber);

    readyQueue.push_back(disks[diskNumber].getIOFile().PID);
    disks[diskNumber].removeIOFile();

    fillCPU();
}

void SimOS::AccessMemoryAddress(unsigned long long address) {
    NoCPUCheck();

    unsigned long long pageNumber = address / pageSize;
    bool pageInMemory = false;

    for (MemoryItem& item : memUsage) {
        if (item.PID == currentCPU && item.pageNumber == pageNumber) {
            pageInMemory = true;
            item.frameNumber = address % pageSize;
            break;
        }
    }

    if (!pageInMemory) {
        if (memUsage.size() * pageSize >= ramSize) {
            memUsage.erase(memUsage.begin());
        }

        MemoryItem newItem{pageNumber, address % pageSize, currentCPU};
        memUsage.push_back(newItem);
    }
}

int SimOS::GetCPU() {
    return currentCPU;
}

std::deque<int> SimOS::GetReadyQueue() {
    return readyQueue;
}

//
MemoryUsage SimOS::GetMemory() {
    MemoryUsage ram;

    for (auto item : memUsage) {
        std::string status = allProcesses[item.PID].getState();

        if (status != "terminated") {
            ram.push_back(item);
        }
    }

    return ram;
}

FileReadRequest SimOS::GetDisk(int diskNumber) {
    InvalidDiskCheck(diskNumber);
    return disks[diskNumber].getIOFile();
}

std::deque<FileReadRequest> SimOS::GetDiskQueue(int diskNumber) {
    InvalidDiskCheck(diskNumber);
    return disks[diskNumber].getIOQueue();
}

void SimOS::terminateChildren(std::unordered_set<int>& childrenPIDs) {
    terminateReadyQueueChildren(childrenPIDs);
    
    for (int i = 0; i < disks.size(); ++i) {
        disks[i].terminateChildrenRequests(childrenPIDs);
    }

    childrenPIDs.insert(currentCPU);
    terminateRAMChildren(childrenPIDs);
}
void SimOS::terminateReadyQueueChildren(const std::unordered_set<int>& childrenPIDs) {
    if (!currentCPU != NO_PROCESS && childrenPIDs.find(currentCPU) != childrenPIDs.end()) {
        allProcesses[currentCPU].setState("terminated");
        currentCPU = NO_PROCESS;
    }

    if (!readyQueue.empty()) {
        int counter = 0, size = readyQueue.size();

        while (counter != size) {
            int front_PID = readyQueue.front();
            bool found = childrenPIDs.find(front_PID) != childrenPIDs.end();

            if (found) {
                allProcesses[front_PID].setState("terminated");
            }
            else {
                readyQueue.push_back(front_PID);
            }

            readyQueue.pop_front();
            counter++;
        }
    }
}

void SimOS::terminateRAMChildren(const std::unordered_set<int>& childrenPIDs) {

}

std::unordered_set<int> SimOS::getFamily(int PID) {
    std::unordered_set<int> allPIDs;

    for (auto child : allProcesses[PID].getChildrenPIDs()) {
        getFamilyHelper(allPIDs, child);
    }

    return allPIDs;
}

void SimOS::getFamilyHelper(std::unordered_set<int>& allPIDs, int PID) {
    if (allProcesses[PID].getChildrenPIDs().empty()) {
        allPIDs.insert(PID);

        return;
    }

    for (auto child : allProcesses[PID].getChildrenPIDs()) {
        getFamilyHelper(allPIDs, child);
    }

    allPIDs.insert(PID);
}

void SimOS::NoCPUCheck() const {
    if (currentCPU == NO_PROCESS) {
        throw std::logic_error("No running process");
    }
}

void SimOS::InvalidDiskCheck(int diskNumber) const {
    if (diskNumber >= disks.size()) {
        throw std::out_of_range("Invalid disk number");
    }
}

void SimOS::fillCPU() {
    if (currentCPU == NO_PROCESS && !readyQueue.empty()) {
        currentCPU = readyQueue.front();
        readyQueue.pop_front();

        allProcesses[currentCPU].setState("running");
    }
}