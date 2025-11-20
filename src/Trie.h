#ifndef TRIE_H
#define TRIE_H
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


#endif //TRIE_H