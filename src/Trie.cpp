#include "Trie.h"
using namespace std;
Trie::Trie() {
    isleaf = false;
    root = nullptr;
    for (int i=0; i<26; i++) {
children[i] = nullptr;
    }
}
void Trie::insert(Trie* rootnode,string word) {
    Trie* current = rootnode;
    for (int i=0; i<word.length(); i++) {
      if (current->children[word[i]-'a'] == nullptr) {
          current->children[word[i]-'a'] = new Trie();
      }
        current = current->children[word[i]-'a'];

    }
    current->isleaf = true;
}
bool Trie::search(Trie* rootnode,string word) {
if (rootnode == nullptr) {
    return false;
}
    Trie* current = rootnode;
    for (int i=0; i<word.length(); i++) {
        if (current->children[word[i]-'a']==nullptr) {
return false;
        }
        current = current->children[word[i]-'a'];
    }
    return current->isleaf;
}