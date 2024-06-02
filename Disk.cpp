//Ella Song
#include "Disk.h"

Disk::Disk() {}

void Disk::updateDisk() {
    if (diskIOFile.fileName == "" && diskIOFile.PID == 0 && !diskIOQueue.empty()) {
        diskIOFile = diskIOQueue.front();
        diskIOQueue.pop_front();
    }
}

void Disk::addIOFile(const FileReadRequest& file) {
    diskIOQueue.push_back(file);

    updateDisk();
}

void Disk::removeIOFile() {
    diskIOFile.fileName = "";
    diskIOFile.PID = 0;

    updateDisk();
}

void Disk::terminateChildrenRequests(const std::unordered_set<int>& children) {
    int counter = 0, size = diskIOQueue.size();

    while (counter != size) {
        FileReadRequest front_file = diskIOQueue.front();

        if (children.find(front_file.PID) != children.end()) {
            diskIOQueue.pop_front();
        }
        else {
            diskIOQueue.pop_front();
            diskIOQueue.push_back(front_file);
        }

        counter += 1;
    }


    if (children.find(diskIOFile.PID) != children.end()) {
        removeIOFile();
    }

    updateDisk();
}

FileReadRequest Disk::getIOFile() const {
    return diskIOFile;
}

std::deque<FileReadRequest> Disk::getIOQueue() const {
    return diskIOQueue;
}