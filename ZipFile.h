#pragma once
#include "Node.h"
#include <iostream>
#include <fstream>
using namespace std;

class ZipFile : public Node {
public:
    string originalType; // TXT, MP3, PRIV, DIR
    string sourcePath;

    // Nai zip banana — source file copy karke zip banao
    ZipFile(string n, string p, Node* par, string origType, string srcPath)
        : Node(n, p, par) {
        originalType = origType;
        sourcePath   = srcPath;

        ifstream src(srcPath, ios::binary);
        ofstream dst(p,       ios::binary);

        if (src.is_open() && dst.is_open()) {
            dst << src.rdbuf();
            src.close();
            dst.close();
            cout << name << ".zip created on disk!\n";
        } else {
            cout << "Zip failed!\n";
        }
    }

    // Disk se load karna (pehle se bani zip)
    ZipFile(string n, string p, Node* par, string origType)
        : Node(n, p, par) {
        originalType = origType;
        sourcePath   = "";
    }

    string getType() override { return "ZIP"; }

    void open() override {
        cout << "Zipped files cannot be opened directly.\n";
        cout << "Do you want to unzip? (yes/no): ";
        string ans; cin >> ans; cin.ignore();
        if (ans == "yes") {
            // Parent folder path nikalo
            string parentPath = path.substr(0, path.find_last_of("\\"));
            unzip(parentPath);
        } else {
            cout << "Cancelled.\n";
        }
    }

    // Unzip — original file wapis banao with "-unzipped" naam
    void unzip(string destFolder) {
        // naam: myfile-zip-unzipped.txt (original extension ke saath)
        string ext = "";
        if      (originalType == "TXT")  ext = ".txt";
        else if (originalType == "MP3")  ext = ".mp3";
        else if (originalType == "PRIV") ext = ".priv";

        string unzipName = name + "-unzipped" + ext;
        string destPath  = destFolder + "\\" + unzipName;

        ifstream src(path,     ios::binary);
        ofstream dst(destPath, ios::binary);

        if (src.is_open() && dst.is_open()) {
            dst << src.rdbuf();
            src.close();
            dst.close();
            cout << "Unzipped as: " << unzipName << " on disk!\n";
        } else {
            cout << "Unzip failed!\n";
        }
    }

    void deleteNode() override {
        remove(path.c_str());
        cout << name << ".zip deleted from disk.\n";
    }
};
