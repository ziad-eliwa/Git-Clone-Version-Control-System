#pragma once
#include <cstdint>
#include <sstream>
#include <string>
#include <ctime>
#include "hashmap.h"
#include "vector.h"

class gitObject{
    public:
        enum Type {BLOB,TREE,COMMIT};
        virtual Type getType() = 0;
        virtual std::string serialize() const = 0;
        virtual std::string computeHash() = 0;
        virtual ~gitObject() = default;
};

class blob : public gitObject{
    private:
        std::string content;
        std::string hash;
    public:
        blob(std::string &data) : content(data){
            hash = computeHash();
        }

        Type getType(){
            return Type::BLOB;
        }
        
        std::string computeHash(){
            return Murmur3_32(serialize().c_str());
        }

        std::string serialize() const override {
            return "blob " + std::to_string(content.length()) + '\0' + content; 
        }

        std::string getContent(){
            return content;
        }
        std::string getHash(){
            return hash;
        }   
};

class commit : public gitObject{
    private:
        std::string hash;
        std::string treeHash;
        std::string author;
        time_t timestamp;
        Vector<std::string> parentHashes;
        std::string message;

    public:

        commit(std::string msg, std::string author, std::string treehash) : message(msg), treeHash(treeHash), author(author){
            timestamp = time_t(nullptr);
            hash = computeHash();
        }

        Type getType(){
            return Type::COMMIT;
        }
        
        std::string computeHash(){
            return Murmur3_32(serialize());
        }

        std::string serialize(){
            std::stringstream ss;

            ss << "tree " << treeHash << "\n";
            for(std::string* i = parentHashes.begin(); i != parentHashes.end(); i++)
                ss << *i << " ";
            ss << "author " << author << " " << timestamp << "\n";
            ss << "\n" << message;
            std::string content = ss.str();
            return "commit " + std::to_string(content.size()) + '\0' + content;  
        }  

        std::string getHash() const { return hash; }
        std::string getTreeHash() const { return treeHash; }
        std::string getMessage() const { return message; }
        
};

class Merkletree : public gitObject{
    private:
        struct treeEntry{
            std::string mode;
            std::string type;
            std::string hash;
            std::string name;

            treeEntry(const std::string& m, const std::string& t, 
                    const std::string& h, const std::string& n) : mode(m), type(t), hash(h), name(n) {}
            // For sorting entries
            bool operator<(const treeEntry& other) const {
                return name < other.name;
            }
            std::string serialize() const {
                return mode + " " + name + '\0' + hash;
            }
        };
        Vector<treeEntry> entries;
        std::string hash;
    
    public:
        Merkletree() = default;

        void addEntry(const treeEntry& entry)
        {
            entries.push_back(entry);
            // sorting entries to ensure consistent hashing
            entries.sort(entries.begin(), entries.end());
            hash = computeHash();
        }

        Type getType() const { return Type::TREE; }
    
        std::string serialize(){
            std::string result;
            for(treeEntry* i = entries.begin(); i != entries.end() ; i++)
            {
                result += (*i).serialize();
            }
            return "tree " + std::to_string(result.size()) + '\0' + result;
        }
        
        std::string computeHash() const  {
            return Murmur3_32(serialize());
        }
        
        std::string getHash() const { return hash; }
        const Vector<treeEntry>& getEntries() const { return entries; }

};