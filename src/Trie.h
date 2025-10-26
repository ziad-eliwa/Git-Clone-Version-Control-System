//
// Created by Ali Ahmed  on 26/10/2025.
//

#ifndef DISTRIBUTED_VERSION_CONTROL_SYSTEM_TRIE_H
#define DISTRIBUTED_VERSION_CONTROL_SYSTEM_TRIE_H
#include <string>
using namespace std;

class Trie {

    public:
    Trie();
    ~Trie();

    void insert(Trie* ,string);
    bool search(Trie* ,string);
    private:
    Trie* root;
    bool isleaf;
    Trie* children[26];

};


#endif //DISTRIBUTED_VERSION_CONTROL_SYSTEM_TRIE_H