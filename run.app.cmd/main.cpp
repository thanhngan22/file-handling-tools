#include <iostream>
#include <dirent.h>
#include <vector>
#include <String>
#include <windows.h>
#include <map>
#include <tchar.h>
#include <shellapi.h>

using namespace std;

map<string, string> readFolder(string path)
{
    map<string, string> result;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            string filename = ent->d_name;
            if (filename.find(".lnk") != string::npos)
            {
                result.emplace(filename.substr(0, filename.find(".lnk")), path + "\\" + filename);
            }
        }
        closedir(dir);
    }
    return result;
}

int main()
{
    SetConsoleOutputCP(65001);

    // khởi tạo
    vector<string> listPaths;
    listPaths.push_back("C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs");
    listPaths.push_back("C:\\Users\\LENOVO\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs");
    listPaths.push_back("C:\\Users\\Public\\Desktop");

    // lấy tất cả file đuôi .lnk lưu map<app name, path>
    map<string, string> locationAppStart;
    DIR *dir;
    struct dirent *ent;
    for (int i = 0; i < listPaths.size(); i++)
    {
        if ((dir = opendir(listPaths[i].c_str())) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                int type = ent->d_type;
                string filename;
                map<string, string> filesFromSubfolder;
                switch (type)
                {
                case DT_DIR:
                    filesFromSubfolder = readFolder(listPaths[i] + "\\" + ent->d_name);
                    locationAppStart.insert(filesFromSubfolder.begin(), filesFromSubfolder.end());
                    break;

                default:
                    filename = ent->d_name;
                    if (filename.find(".lnk") != string::npos)
                    {
                        locationAppStart.emplace(filename.substr(0, filename.find(".lnk")), listPaths[i] + "\\" + filename);
                    }
                    break;
                }
            }
            closedir(dir);
        }
    }

    // xử lý case system app như camera, setting, calculator, ...
    // locationAppStart.emplace("camera", "shell:AppsFolder\\Microsoft.WindowsCamera_8wekyb3d8bbwe!App");
    system("powershell Get-AppxPackage | Select Name, PackageFamilyName > appInfo.txt");
 


    // list to check
    // for (auto item = locationAppStart.begin(); item != locationAppStart.end(); item++)
    // {
    //     cout << item->first << endl; //" - " << item->second << endl;
    // }

    do
    {
        cout << " Nhập tên ứng dụng muốn mở: ";
        string appName;
        getline(cin, appName);

        // Xử lý
        string path;
        auto item = locationAppStart.find(appName);
        if (item != locationAppStart.end())
        {
            path = item->second;
        }
        if (path.empty())
        {
            for (auto item = locationAppStart.begin(); item != locationAppStart.end(); item++)
            {
                if (item->first.find(appName) != string::npos)
                {
                    path = locationAppStart[item->first];
                    // cout << "path: " << path << endl;
                    break;
                }
            }
        }

        if (path.empty())
        {
            cout << "Không tìm thấy ứng dụng" << endl;
            return 22;
        }
        else
        {
            string command = "powershell.exe -command \"explorer '" + path + "'\"";
            system(command.c_str());
        }
    } while (true);

    return 225;
}
