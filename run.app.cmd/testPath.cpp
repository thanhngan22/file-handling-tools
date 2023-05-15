#include <bits/stdc++.h>
#include <dirent.h>
using namespace std;

int main() {
    ifstream fin("path.txt");
    string path;
    getline(fin, path);
    for (int i = 0; i < path.length(); i++) {
        cout << path[i] << "";
        if (path[i] == ' ') {
            cout << "space";
        }
    }
    cout << "\npath when cstr: " << path.c_str() << endl;
    string newpath = path ;
    DIR *dir;
    if ((dir = opendir(newpath.c_str())) != NULL) {
        cout << "found" << endl;
        // list all file and folder in path
        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
            cout << ent->d_name << endl;
        }
    } else {
        cout << "not found" << endl;
    }
    return 225;
}