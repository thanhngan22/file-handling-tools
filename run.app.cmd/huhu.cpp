#include <bits/stdc++.h>
#include <dirent.h>
using namespace std;

int main() {
    ifstream fin("InstallLocation.csv");
    string line;
    // ignore two line
    getline(fin, line);
    getline(fin, line);
    while (getline(fin, line)) {
        // ignore  " and "
        line = line.substr(1, line.size() - 2);
        DIR *dir = opendir(line.c_str());
        if (dir) {
            cout << "can open " << line << endl;
            closedir(dir);
        } else {
            cout << "can't open " << line << endl;
        }
    }
    return 22;
}