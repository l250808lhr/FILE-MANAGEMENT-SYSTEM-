#pragma once
#include "Node.h"
#include <iostream>
#include <direct.h>
using namespace std;

class Folder : public Node {
public:
    Node* children[100];
    int   childCount;

    Folder(string n, string p, Node* par) : Node(n, p, par) {
        childCount = 0;
        for (int i = 0; i < 100; i++) children[i] = nullptr;
        _mkdir(path.c_str()); // Real folder disk pe banao
    }

    ~Folder() {
        for (int i = 0; i < childCount; i++) delete children[i];
    }

    string getType() override { return "DIR"; }

    void open() override {
        cout << "Opened folder: " << name << "\n";
    }

    void deleteNode() override {
        for (int i = 0; i < childCount; i++)
            children[i]->deleteNode();
        _rmdir(path.c_str()); // Disk se delete
    }

    void addChild(Node* node) {
        if (childCount >= 100) { cout << "Folder full!\n"; return; }
        children[childCount] = node;
        childCount++;
    }

    Node* findChild(string n) {
        for (int i = 0; i < childCount; i++)
            if (children[i]->name == n) return children[i];
        return nullptr;
    }

    bool removeChild(string n) {
        for (int i = 0; i < childCount; i++) {
            if (children[i]->name == n) {
                children[i]->deleteNode();
                delete children[i];
                for (int j = i; j < childCount-1; j++)
                    children[j] = children[j+1];
                children[childCount-1] = nullptr;
                childCount--;
                return true;
            }
        }
        return false;
    }

    void ls() {
        if (childCount == 0) { cout << "(empty)\n"; return; }
        for (int i = 0; i < childCount; i++)
            cout << "  [" << children[i]->getType() << "] "
                 << children[i]->name << "\n";
    }

    string search(string target, string currentPath) {
        for (int i = 0; i < childCount; i++) {
            string fullPath = currentPath + "/" + children[i]->name;
            if (children[i]->name == target) return fullPath;
            if (children[i]->getType() == "DIR") {
                string result = ((Folder*)children[i])->search(target, fullPath);
                if (result != "") return result;
            }
        }
        return "";
    }

    bool nameExists(string n) {
        for (int i = 0; i < childCount; i++) {
            if (children[i]->name == n) return true;
            if (children[i]->getType() == "DIR")
                if (((Folder*)children[i])->nameExists(n)) return true;
        }
        return false;
    }
};
