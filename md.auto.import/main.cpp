#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <string>
using namespace std;

int main() {
    // nhập tên thư mục muốn mở
    string folderName;
    cout << "Nhap ten thu muc hinh anh: ";
    getline(cin, folderName);
    
    ofstream fout("readme.md", ios::app);
    if (!fout.is_open()) {
        cout << "Khong the mo file readme.md" << endl;
        return 0;
    }

    // mở folder và xem tên các file, lưu vào vector fileNames
    vector <string> fileNames;
    
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(folderName.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            string fileName = ent->d_name;
            if (fileName != "." && fileName != "..") {
                fileNames.push_back(fileName);
            }
        }
        closedir(dir);
    } else {
        cout << "Khong the mo thu muc " << folderName << endl;
        return 0;
    }

    // ghi vào file readme.md theo format để import
    for (int i = 0; i < fileNames.size(); i++) {
        fout << "<img src=\"" << folderName << "/" << fileNames[i] << "\" width=\"100%\"/>" << endl;
    }
    fout.close();
    cout << "Da ghi vao file readme.md" << endl;
    return 225;







}