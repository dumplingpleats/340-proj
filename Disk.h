//Ella Song
#ifndef DISK_H
#define DISK_H

#include <iostream>
#include <string>
#include <queue>
#include <unordered_set>

struct FileReadRequest {
    int PID{0};
    std::string fileName{""};
};

class Disk {
public:
    Disk();

    void updateDisk();
    void addIOFile(const FileReadRequest& file);
    void removeIOFile();
    void terminateChildrenRequests(const std::unordered_set<int>& children);

    FileReadRequest getIOFile() const;
    std::deque<FileReadRequest> getIOQueue() const;

private:
    FileReadRequest diskIOFile;
    std::deque<FileReadRequest> diskIOQueue;
};

#endif 