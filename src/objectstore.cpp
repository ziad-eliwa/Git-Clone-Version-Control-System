#include "objectstore.h"
#include "gitobjects.h"
#include "hashmap.h"
#include "helpers.h"
#include "vector.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

ObjectStore::ObjectStore(std::filesystem::path sp) : storePath(sp) {
  if (!std::filesystem::exists(storePath)) {
    if (!std::filesystem::create_directories(storePath))
      throw std::runtime_error("failed to create directory");
  }
};

void ObjectStore::store(GitObject *obj) {
  std::ofstream object(storePath / obj->getHash(), std::ios::binary);
  object << obj->serialize();
  object.flush();
  object.close();
};

GitObject *ObjectStore::store(std::filesystem::path path) {
  GitObject *ret;
  if (!std::filesystem::is_directory(path)) {
    std::string content = readFile(path);

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
  std::ofstream object(storePath / ret->getHash(), std::ios::binary);
  object << ret->serialize();
  object.flush();
  object.close();
  return ret;
}

bool ObjectStore::exists(std::string hash) {
  return std::filesystem::exists(storePath / hash);
}

GitObject *ObjectStore::retrieve(std::string hash) {
  if (!exists(hash))
    return nullptr;

  std::string filePath(storePath / hash);
  std::string content = readFile(filePath);

  Vector<std::string> lines = split(content, '\n');
  if (lines.empty())
    return nullptr;

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
    Commit *cmt = new Commit();

    for (int i = 1; i <= entries; i++) {
      Vector<std::string> entry = split(lines[i], ' ');
      if (entry[0] == "author") {
        cmt->setAuthor(entry[1]);
      } else if (entry[0] == "timestamp") {
        cmt->setTimeStamp(entry[1]);
      } else if (entry[0] == "message") {
        std::string msg;
        for (int i = 1; i < entry.size(); i++)
          msg += entry[i] + " ";
        cmt->addMessage(msg);
      } else if (entry[0] == "tree") {
        cmt->addTreeHash(entry[1]);
      } else if (entry[0] == "parent") {
        cmt->addParentHash(entry[1]);
      }
    }
    return cmt;
  }
  return nullptr;
}

std::string ObjectStore::retrieveLog(std::string commit) {
  GitObject *obj = retrieve(commit);
  if (Commit *cmt = dynamic_cast<Commit *>(obj)) {
    std::string result = commit + " " + cmt->getAuthor() + " " +
                         cmt->getMessage() + " (" + cmt->getTimeStamp() + ")\n";
    for (auto &parent : cmt->getParentHashes())
      result += retrieveLog(parent);

    return result;
  } else {
    throw std::runtime_error("Not a commit hash");
  }
}

void ObjectStore::reconstruct(std::string hash, std::filesystem::path path) {
  GitObject *obj = retrieve(hash);
  if (Tree *tree = dynamic_cast<Tree *>(obj)) {
    if (!std::filesystem::exists(path)) {
      if (!std::filesystem::create_directories(path))
        throw std::runtime_error("failed to create directory");
    }

    for (auto &entry : tree->getEntries()) {
      GitObject *entryObj = retrieve(entry.hash);
      if (Blob *b = dynamic_cast<Blob *>(entryObj)) {
        std::ofstream of(path / entry.name);
        of << b->getContent();
      } else if (Tree *t = dynamic_cast<Tree *>(entryObj)) {
        reconstruct(t->getHash(), path / entry.name);
      }
    }
  }
}
