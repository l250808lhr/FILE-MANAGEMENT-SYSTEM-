#pragma once
#include <string>
using namespace std;

class Node {
public:
    string name;
    string path; // real disk path
    Node* parent;

    Node(string n, string p, Node* par) {
        name   = n;
        path   = p;
        parent = par;
    }

    virtual void open()       = 0;
    virtual void deleteNode() = 0;
    virtual string getType()  = 0;

    virtual ~Node() {}
};
