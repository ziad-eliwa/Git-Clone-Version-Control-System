//
// Created by Ali Ahmed  on 19/11/2025.
//

#pragma once
#include "hashmap.h"
#include "vector.h"
#include "object_store.h"
#include <fstream>
#include <filesystem>

class Index {

private:
    ObjectStore &objectStore;
    std::string filePath;
    HashMap<std::string,std::string> fileContents;




public:
    Index(std::string filePath, ObjectStore &objectStore) : objectStore(objectStore) {
        this->filePath = (filePath+"/index");
        load();
    }
    void add(std::string path) {
        blob b = objectStore.storeBlob( filePath);
        fileContents.set(path, b.getHash());
        save();
    }
    void save() {
        std::ofstream file;
        file.open(this->filePath);
        for (auto [path, hash] : fileContents) {
            file << path << " " <<hash<<std::endl;
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
