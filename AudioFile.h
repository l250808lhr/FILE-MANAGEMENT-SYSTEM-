#pragma once
#include <windows.h>
#include "Node.h"
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

class AudioFile : public Node {
public:
    // Nai audio file record karne ke liye constructor
    AudioFile(string n, string p, Node* par) : Node(n, p, par) {
        cout << "\n--- Audio Recorder ---" << endl;
        cout << "Recording starts in:" << endl;
        cout << "3... "; cout.flush(); Sleep(1000); // 1 second gap
        cout << "2... "; cout.flush(); Sleep(1000);
        cout << "1... "; cout.flush(); Sleep(1000);
        cout << "\nRECORDING... (5 Seconds tak bolein)" << endl;

        /*
           FIXED COMMAND:
           1. Escape quotes (\") use kiye hain device name ke liye.
           2. Aapke screenshot ke mutabiq device name 'Microphone (High Definition Audio Device)' rakha hai.
           3. -t 5 ka matlab hai 5 seconds ki recording.
        */
        string deviceName = "Microphone (High Definition Audio Device)";
        string cmd = "ffmpeg -f dshow -i audio=\"" + deviceName + "\" -t 5 \"" + path + "\" -y -loglevel quiet";

        // Command execute ho rahi hai
        int result = system(cmd.c_str());

        if (result == 0) {
            cout << "SUCCESS: Recording saved as " << name << endl;
        }
        else {
            cout << "ERROR: Recording failed! Check if FFmpeg is in PATH or Mic is busy." << endl;
        }
    }

    // Disk se purani file load karne ke liye
    AudioFile(string n, string p, Node* par, bool load)
        : Node(n, p, par) {
        // Sirf object create hoga, recording nahi
    }

    string getType() override { return "MP3"; }

    // Audio play karne ke liye
    void open() override {
        cout << "Playing: " << name << "..." << endl;
        // ffplay use ho raha hai bagair window kholay (-nodisp)
        string cmd = "ffplay -nodisp -autoexit \"" + path + "\" -loglevel quiet";
        system(cmd.c_str());
        cout << "Playback finished." << endl;
    }

    // File delete karne ke liye
    void deleteNode() override {
        if (remove(path.c_str()) == 0) {
            cout << "File " << name << " deleted from disk." << endl;
        }
        else {
            cout << "Error deleting file." << endl;
        }
    }
};