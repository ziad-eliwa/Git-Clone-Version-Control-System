#include "gitobjects.h"
#include "helpers.h"
#include "object_store.h"
#include "vector.h"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>

ObjectStore::ObjectStore(std::string sp) {
  std::filesystem::path store(sp);
  storePath = store.string();
  if (!std::filesystem::exists(store)) {
    if (!std::filesystem::create_directories(store))
      throw std::runtime_error("failed to create directory");
  }
};

void ObjectStore::store(GitObject *obj) {
  if (Commit *c = dynamic_cast<Commit *>(obj)) {
    std::cout << "HERE" << std::endl; // TODO: recurse
  }
  if (Tree *t = dynamic_cast<Tree *>(obj)) {
    for (auto &entry : t->getEntries()) {
      GitObject *obj = retrieve(entry.getHash());
      store(obj);
    }
  }
  std::ofstream object(storePath + "/" + obj->getHash(), std::ios::binary);
  object << obj->serialize();
  object.flush();
  object.close();
};

GitObject *ObjectStore::store(std::string path) {
  GitObject *ret;
  if (!std::filesystem::is_directory(path)) {
    std::ifstream file(path, std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    Blob *b = new Blob(content);
    store(b);
    ret = b;
  } else {
    Tree *t = new Tree();
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
      GitObject *obj = store(entry.path());
      if (Tree *st = dynamic_cast<Tree *>(obj)) {
        t->addSubTree(entry.path(), *st);
      } else if (Blob *b = dynamic_cast<Blob *>(obj)) {
        t->addBlob(entry.path(), *b);
      }
    }
    store(t);
    ret = t;
  }
  std::ofstream object(storePath + "/" + ret->getHash(), std::ios::binary);
  object << ret->serialize();
  object.flush();
  object.close();
  return ret;
}

GitObject *ObjectStore::retrieve(std::string hash) {
  std::string filePath(storePath + "/" + hash);
  std::ifstream file(filePath, std::ios::binary);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());

  Vector<std::string> lines = split(content, '\n');
  if (lines.empty()) {
    // TODO: handle hash not existing.
    return nullptr;
  }
  Vector<std::string> header = split(lines[0], ' ');
  if (header[0] == "blob") {
    int ln = content.find('\n'); // skip first line
    Blob *b = new Blob(content.substr(ln + 1));
    return b;
  } else if (header[0] == "tree") {
    int entries = std::stoi(header[1]);

    Tree *t = new Tree();
    for (int i = 1; i <= entries; i++) {
      Vector<std::string> entry = split(lines[i], ' ');
      GitObject *obj = retrieve(entry[2]);
      if (Tree *st = dynamic_cast<Tree *>(obj)) {
        t->addSubTree(entry[1], *st);
      } else if (Blob *b = dynamic_cast<Blob *>(obj)) {
        t->addBlob(entry[1], *b);
      }
    }
    return t;
  } else if (header[0] == "commit") {
    int entries = std::stoi(header[1]);
    Commit* cmt = new Commit();

    for (int i = 1; i <= entries; i++) {
      Vector<std::string> entry = split(lines[i], ' ');
      if(entry[0] == "author") {
          cmt->setAuthor(entry[1]);        
      } else if(entry[0] == "timestamp") {
        cmt->setTimeStamp(entry[1]);
      } else if(entry[0] == "message") {
        std::string msg;
        for(int i = 1; i < entry.size(); i++) msg += entry[i] +" ";
        cmt->addMessage(msg);
      } else if(entry[0] == "tree") {
        cmt->addTreeHash(entry[1]);
      } else if (entry[0] == "parent") {
        cmt->addParentHash(entry[1]);
      }
    }
    return cmt;
  }
  return nullptr;
}

std::string ObjectStore::retrieveLog(std::string lastHash) {
  GitObject * obj = retrieve(lastHash);
  if(Commit *cmt = dynamic_cast<Commit *>(obj)){
    std::string result = lastHash + " " + cmt->getAuthor() + " " + cmt->getMessage() + cmt->getTimeStamp() + "\n";
    Vector<std::string> parents = cmt->getParentHashes(); 
    for (int i = 0; i < parents.size(); ++i) {
      result += retrieveLog(parents[i]);
    }
    return result;
  } else {
    throw std::runtime_error("Not a commit hash");
  }
}