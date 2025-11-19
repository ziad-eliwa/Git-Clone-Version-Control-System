//
// Created by Ali Ahmed  on 19/11/2025.
//


#include "hashmap.h"
#include "vector.h"
#include <fstream>
#include <filesystem>

class Index {

private:
    std::string filePath;
    HashMap<std::string,std::string> fileContents;




public:
    Index(std::string filePath) {
        this->filePath = (filePath+"/index");
    }
    void add(std::string path,std::string hash) {
        fileContents.set(hash, path);
    }
    void save() {
        std::ofstream file;
        file.open(this->filePath);
        for (auto it=fileContents.begin(); it!=fileContents.end(); ++it) {
            file << (*it).key << " " <<(*it).value <<std::endl;

        }
    }
    void load() {
        if (!std::filesystem::exists(filePath)) return;
        std::ifstream in;
        in.open(this->filePath);
        std::string path, hash;
        while (in >> path >> hash) {
            fileContents.set(path, hash);
        }
    }
    HashMap<std::string, std::string>& getStagedFiles() {
        return fileContents;
    }

};
