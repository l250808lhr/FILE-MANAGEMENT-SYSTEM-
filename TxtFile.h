#pragma once
#include "Node.h"
#include <iostream>
#include <fstream>
using namespace std;

class TxtFile : public Node {
public:
    string lines[100];
    int lineCount;

    TxtFile(string n, string p, Node* par) : Node(n, p, par) {
        lineCount = 0;
        loadFromDisk();
    }

    string getType() override { return "TXT"; }

    void loadFromDisk() {
        lineCount = 0;
        ifstream file(path);
        if (!file.is_open()) return;
        string line;
        while (getline(file, line) && lineCount < 100) {
            lines[lineCount] = line;
            lineCount++;
        }
        file.close();
    }

    void saveToDisk() {
        ofstream file(path);
        for (int i = 0; i < lineCount; i++)
            file << lines[i] << "\n";
        file.close();
        cout << "File saved to disk!\n";
    }

    void open() override {
        loadFromDisk();
        int cursor = 0;
        string cmd;

        while (true) {
            cout << "\n==== " << name << ".txt ====\n";
            if (lineCount == 0)
                cout << "(empty - use 'add' to write)\n";
            else {
                for (int i = 0; i < lineCount; i++) {
                    if (i == cursor)
                        cout << "> [" << i+1 << "] " << lines[i] << "\n";
                    else
                        cout << "  [" << i+1 << "] " << lines[i] << "\n";
                }
            }

            cout << "\nup | down | add | edit | close\n>> ";
            cin >> cmd; cin.ignore();

            if (cmd == "close") {
                saveToDisk();
                cout << "File closed.\n";
                break;
            }
            else if (cmd == "up")   { if (cursor > 0) cursor--; }
            else if (cmd == "down") { if (cursor < lineCount-1) cursor++; }
            else if (cmd == "add") {
                if (lineCount >= 100) { cout << "File full!\n"; continue; }
                cout << "New line: ";
                string line; getline(cin, line);
                lines[lineCount] = line;
                lineCount++;
                cursor = lineCount - 1;
            }
            else if (cmd == "edit") {
                if (lineCount == 0) { cout << "No lines!\n"; continue; }
                cout << "New text: ";
                string line; getline(cin, line);
                lines[cursor] = line;
            }
            else cout << "Unknown command!\n";
        }
    }

    void deleteNode() override {
        remove(path.c_str());
        cout << name << ".txt deleted from disk.\n";
    }
};
