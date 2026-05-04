#pragma once
#include "TxtFile.h"
#include <iostream>
#include <fstream>
using namespace std;

class PrivateFile : public TxtFile {
public:
    string passkey;

    // Bilkul nai file banana
    PrivateFile(string n, string p, Node* par) : TxtFile(n, p, par) {
        lineCount = 0;
        cout << "Set passkey: ";
        cin >> passkey; cin.ignore();
        ofstream file(path);
        file << "PASSKEY:" << passkey << "\n";
        file.close();
        cout << "Private file created on disk!\n";
    }

    // Disk se load karna — passkey file se parho
    PrivateFile(string n, string p, Node* par, bool fromDisk) : TxtFile(n, p, par) {
        lineCount = 0;
        passkey   = "";
        ifstream file(path);
        if (!file.is_open()) return;
        string firstLine;
        getline(file, firstLine);
        if (firstLine.size() >= 8 && firstLine.substr(0, 8) == "PASSKEY:")
            passkey = firstLine.substr(8);
        string line;
        while (getline(file, line) && lineCount < 100) {
            lines[lineCount] = line;
            lineCount++;
        }
        file.close();
    }

    string getType() override { return "PRIV"; }

    void loadFromDisk() {
        lineCount = 0;
        ifstream file(path);
        if (!file.is_open()) return;
        string firstLine;
        getline(file, firstLine);
        if (firstLine.size() >= 8 && firstLine.substr(0, 8) == "PASSKEY:")
            passkey = firstLine.substr(8);
        string line;
        while (getline(file, line) && lineCount < 100) {
            lines[lineCount] = line;
            lineCount++;
        }
        file.close();
    }

    void saveToDisk() {
        ofstream file(path);
        file << "PASSKEY:" << passkey << "\n";
        for (int i = 0; i < lineCount; i++)
            file << lines[i] << "\n";
        file.close();
        cout << "File saved to disk!\n";
    }

    bool checkPasskey() {
        cout << "Enter passkey: ";
        string input; cin >> input; cin.ignore();
        return input == passkey;
    }

    void open() override {
        loadFromDisk();
        if (!checkPasskey()) {
            cout << "Wrong passkey! Access denied.\n";
            return;
        }
        TxtFile::open();
    }

    void deleteNode() override {
        loadFromDisk(); // passkey wapis parho
        if (!checkPasskey()) {
            cout << "Wrong passkey! Cannot delete.\n";
            return;
        }
        cout << "Are you sure? (yes/no): ";
        string confirm; cin >> confirm; cin.ignore();
        if (confirm == "yes") {
            remove(path.c_str());
            cout << name << ".priv deleted from disk.\n";
        } else {
            cout << "Deletion cancelled.\n";
        }
    }
};
