#include <iostream>
#include <dirent.h>
#include <vector>
#include <String>
#include <windows.h>
#include <map>
#include <tchar.h>
#include <shellapi.h>
#include <shlobj.h>

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

    // init path to apps location (default path of windows)
    vector<string> listPaths;
    listPaths.push_back("C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs");

    

    // get username
    char *username = getenv("USERNAME");

    if (username == NULL)
    {
        cout << "Failed to get username from environment variable." << endl;
    }
    else
    {
        string username_str(username);
        listPaths.push_back("C:\\Users\\" + username_str + "\\AppData\\Roaming\\Microsoft\\Windows\\Start Menu\\Programs");
    }

    // get inf of shortcut app in start menu -> map<name app, path>
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
        else
        {
            cout << "Not found path " << listPaths[i] << endl;
        }
    }

    // handle case system app như camera, setting, calculator, ...
    // locationAppStart.emplace("camera", "shell:AppsFolder\\Microsoft.WindowsCamera_8wekyb3d8bbwe!App");
    string cmd_getAppPackage = "powershell Get-AppxPackage -User " + string(username) + " | Select Name, PackageFamilyName > app.txt";
    system("powershell Get-AppxPackage | Select Name, PackageFamilyName > appInfo.txt");

    // list to check
    for (auto item = locationAppStart.begin(); item != locationAppStart.end(); item++)
    {
        cout << item->first << endl; //" - " << item->second << endl;
    }

    do
    {
        cout << " Enter name of an app: ";
        string appName;
        getline(cin, appName);

        // handle
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
            cout << "Not found " << appName << " in your computer." << endl;
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
