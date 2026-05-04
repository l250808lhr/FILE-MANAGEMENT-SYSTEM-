#pragma once
#include "Folder.h"
#include "TxtFile.h"
#include "PrivateFile.h"
#include "AudioFile.h"
#include "ZipFile.h"
#include <iostream>
#include <fstream>
#include <io.h>
#include <direct.h>
using namespace std;

class CommandManager {
public:
    Folder* root;
    Folder* current;

    CommandManager() {
        _mkdir("root");
        root    = new Folder("root", "root", nullptr);
        current = root;
        loadFromDisk(root); // startup pe saari files load karo
    }

    ~CommandManager() { delete root; }

    // Disk se load — saari existing files/folders RAM mein lao
    void loadFromDisk(Folder* folder) {
        string searchPath = folder->path + "\\*";
        struct _finddata_t fileInfo;
        intptr_t handle = _findfirst(searchPath.c_str(), &fileInfo);
        if (handle == -1) return;

        do {
            string fname    = fileInfo.name;
            if (fname == "." || fname == "..") continue;
            string fullPath = folder->path + "\\" + fname;

            // Folder
            if (fileInfo.attrib & _A_SUBDIR) {
                Folder* f = new Folder(fname, fullPath, folder);
                folder->addChild(f);
                loadFromDisk(f); // recursively andar bhi scan karo
            }
            // .txt file
            else if (fname.size() > 4 &&
                     fname.substr(fname.size()-4) == ".txt") {
                string n = fname.substr(0, fname.size()-4);
                folder->addChild(new TxtFile(n, fullPath, folder));
            }
            // .priv file — passkey file se parho
            else if (fname.size() > 5 &&
                     fname.substr(fname.size()-5) == ".priv") {
                string n = fname.substr(0, fname.size()-5);
                folder->addChild(new PrivateFile(n, fullPath, folder, true));
            }
            // .mp3 file
            else if (fname.size() > 4 &&
                     fname.substr(fname.size()-4) == ".mp3") {
                string n = fname.substr(0, fname.size()-4);
                folder->addChild(new AudioFile(n, fullPath, folder, true));
            }
            // .zip file
            else if (fname.size() > 4 &&
                     fname.substr(fname.size()-4) == ".zip") {
                string n = fname.substr(0, fname.size()-4);
                folder->addChild(new ZipFile(n, fullPath, folder, "UNK"));
            }

        } while (_findnext(handle, &fileInfo) == 0);

        _findclose(handle);
    }

    void run() {
        cout << "==============================\n";
        cout << "   File Management System\n";
        cout << "==============================\n";
        cout << "ls | mkdir | touch | cd | open | rm | rename | search | exit\n";

        string cmd;
        while (true) {
            cout << "\n[" << current->name << "] $ ";
            cin >> cmd; cin.ignore();

            if      (cmd == "ls")     doLs();
            else if (cmd == "mkdir")  doMkdir();
            else if (cmd == "touch")  doTouch();
            else if (cmd == "cd")     doCd();
            else if (cmd == "open")   doOpen();
            else if (cmd == "rm")     doRm();
            else if (cmd == "rename") doRename();
            else if (cmd == "search") doSearch();
            else if (cmd == "exit")   { cout << "Goodbye!\n"; break; }
            else cout << "Unknown command!\n";
        }
    }

    void doLs() {
        cout << "\n--- " << current->name << " ---\n";
        current->ls();
    }

    void doMkdir() {
        cout << "Folder name: ";
        string name; getline(cin, name);
        if (name == "root" || root->nameExists(name)) {
            cout << "Name already exists!\n"; return;
        }
        string diskPath = current->path + "\\" + name;
        current->addChild(new Folder(name, diskPath, current));
        cout << "Folder '" << name << "' created.\n";
    }

    void doTouch() {
        cout << "File type (txt / mp3 / priv / zip): ";
        string type; getline(cin, type);
        if (type == "zip") { doZip(); return; }

        cout << "File name: ";
        string name; getline(cin, name);
        if (name == "root" || root->nameExists(name)) {
            cout << "Name already exists!\n"; return;
        }

        if (type == "txt") {
            string dp = current->path + "\\" + name + ".txt";
            ofstream f(dp); f.close();
            current->addChild(new TxtFile(name, dp, current));
            cout << "Text file '" << name << "' created.\n";
        }
        else if (type == "mp3") {
            string dp = current->path + "\\" + name + ".mp3";
            current->addChild(new AudioFile(name, dp, current));
        }
        else if (type == "priv") {
            string dp = current->path + "\\" + name + ".priv";
            current->addChild(new PrivateFile(name, dp, current));
        }
        else cout << "Unknown type! Use: txt, mp3, priv, zip\n";
    }

    void doZip() {
        if (current->childCount == 0) {
            cout << "No files to zip!\n"; return;
        }
        cout << "Which node to zip? (name): ";
        string target; getline(cin, target);

        Node* found = current->findChild(target);
        if (!found)                    { cout << target << " not found!\n"; return; }
        if (found->getType() == "ZIP") { cout << "Cannot zip a zip!\n"; return; }

        string zipName  = target + "-zip";
        if (root->nameExists(zipName)) { cout << "Zip already exists!\n"; return; }

        string diskPath = current->path + "\\" + zipName + ".zip";
        current->addChild(new ZipFile(zipName, diskPath, current,
                                      found->getType(), found->path));
    }

    void doCd() {
        cout << "Name (.. to go back): ";
        string name; getline(cin, name);

        if (name == "..") {
            if (current == root) cout << "Already at root!\n";
            else current = (Folder*)current->parent;
            return;
        }

        Node* found = current->findChild(name);
        if (!found)                    { cout << name << " not found!\n"; return; }
        if (found->getType() != "DIR") { cout << name << " is a file! Use 'open'.\n"; return; }
        current = (Folder*)found;
        cout << "Opened: " << name << "\n";
    }

    void doOpen() {
        cout << "Name: ";
        string name; getline(cin, name);
        Node* found = current->findChild(name);
        if (!found) { cout << name << " not found!\n"; return; }
        found->open();
    }

    void doRm() {
        cout << "Name to delete: ";
        string name; getline(cin, name);
        if (name == "root") { cout << "Cannot delete root!\n"; return; }
        Node* found = current->findChild(name);
        if (!found) { cout << name << " not found!\n"; return; }
        current->removeChild(name);
        cout << "'" << name << "' deleted.\n";
    }

    void doRename() {
        cout << "Current name: ";
        string oldName; getline(cin, oldName);
        cout << "New name: ";
        string newName; getline(cin, newName);

        Node* found = current->findChild(oldName);
        if (!found)                    { cout << oldName << " not found!\n"; return; }
        if (root->nameExists(newName)) { cout << "Name already exists!\n"; return; }

        string newPath = current->path + "\\" + newName;
        rename(found->path.c_str(), newPath.c_str());
        found->name = newName;
        found->path = newPath;
        cout << "Renamed to '" << newName << "'.\n";
    }

    void doSearch() {
        cout << "Search name: ";
        string name; getline(cin, name);
        string result = current->search(name, current->name);
        if (result == "") cout << name << " not found!\n";
        else              cout << "Found at: " << result << "\n";
    }
};
