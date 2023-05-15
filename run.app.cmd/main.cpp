#include <iostream>
#include <dirent.h>
#include <vector>
#include <String>
#include <windows.h>
#include <map>
#include <tchar.h>
#include <shellapi.h>
#include <shlobj.h>
#include <cstdlib>
#include <fstream>

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

bool isStringSpace(string str)
{
    for (int i = 0; i < str.length(); i++)
    {
        if (str[i] != ' ' || str[i] != '\n' || str[i] != '\t' || str[i] != '\r' || str[i] != '\v' || str[i] != '\f') {
            return false;
        }
    }
    return true;
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
        cout << " Username: " << username << endl;
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
            cout << "Not found path start menu " << listPaths[i] << endl;
        }
    }

    // // list to check
    // cout << "List app found in Start Menu: " << endl;
    // for (auto item = locationAppStart.begin(); item != locationAppStart.end(); item++)
    // {
    //     cout << item->first << endl; //" - " << item->second << endl;
    // }

    // handle case system app như camera, setting, calculator, ...
    // locationAppStart.emplace("camera", "shell:AppsFolder\\Microsoft.WindowsCamera_8wekyb3d8bbwe!App");
    string cmd_getInstallLocation = "powershell.exe -Command \"Get-AppxPackage -User " + string(username) + " | Select InstallLocation | Export-Csv -Path InstallLocation.csv\"";
    string cmd_getPackageFamilyName = "powershell.exe -Command \"Get-AppxPackage -User " + string(username) + " | Select PackageFamilyName | Export-Csv -Path PackageFamilyName.csv\"";

    int result = system(cmd_getInstallLocation.c_str()) && system(cmd_getPackageFamilyName.c_str());
    // int result =  system(cmd_getPackageFamilyName.c_str());

    if (result != 0)
    {
        std::cerr << "Failed to save apps list." << std::endl;
    }

    // read file InstallLocation.txt and PackageFamilyName.txt, then get name and path save to map <K-V>
    map<string, string> locationAppSystem;
    ifstream fileLocation("InstallLocation.csv");
    ifstream filePackage("PackageFamilyName.csv");

    if (!fileLocation || !filePackage)
    {
        cout << "Unable to open file";
    }
    else
    {
        string lineLocation;
        string lineName;

        fileLocation.seekg(0, ios::beg);
        filePackage.seekg(0, ios::beg);

        // ignore 3 first line
        for (int i = 0; i < 2; i++)
        {
            getline(fileLocation, lineLocation);
            getline(filePackage, lineName);
        }

        while (getline(fileLocation, lineLocation) && getline(filePackage, lineName))
        {
            // cout << lineLocation << endl;
            // cout << lineName << endl;
            if (lineLocation == "" || lineName == "" || isStringSpace(lineLocation) || isStringSpace(lineName))
            {
                cout << "Empty line" << endl;
                continue;
            }
            locationAppSystem.emplace(lineName.substr(0, lineName.find(" ")), lineLocation);
            // cout << "When reading: " << lineName.substr(0, lineName.find(" ")) << " - " << lineLocation << endl;
        }
    }
    // close file
    fileLocation.close();
    filePackage.close();

    // // list 
    // for (auto it = locationAppSystem.begin(); it != locationAppSystem.end(); it++)
    // {
    //     cout << "after assign: " << it->first << " - " << it->second << endl;
    // }

    // // test 
    // string test = "C:\\Windows\\PrintDialog";
    // if ((dir = opendir(test.c_str())) != NULL)
    // {
    //     cout << "You can do it" << endl;
    // }
    // else
    // {
    //     cout << "You need permission" << test << endl;
    // }

    map<string, string> appSystem; // to store app system with <name and packageFamilyName with id app>

    // go to install location and find appxmanifest.xml
    for (auto item = locationAppSystem.begin(); item != locationAppSystem.end(); item++)
    {
        if ((dir = opendir((item->second.substr(1, item->second.size() - 2).c_str()))) != NULL)
        {
            cout << "\nOpen folder: " << item->second << endl;
            while ((ent = readdir(dir)) != NULL)
            {
                string filename = ent->d_name;
                if (filename == "AppxManifest.xml" || filename == "appxmanifest.xml")
                {
                    /*  read file manifest
                        1. if not found Application Id= and Excutable= then continue
                        2. if found:
                             name app = name of executable file ignore .exe
                            packageFamilyName = packageFamilyName + ! + Application Id
                    */

                    ifstream fileManifest(item->second.substr(1, item->second.size() - 2) + "\\" + filename);
                    if (!fileManifest)
                    {
                        cout << "Unable to open file";
                    }
                    else
                    {
                        string line;
                        string nameApp;
                        string packageFamilyName;
                        string applicationId;

                        while (getline(fileManifest, line))
                        {
                            if (line.find("Application Id=") != string::npos)
                            {
                                applicationId = line.substr(line.find("Application Id=") + 16);
                                applicationId = applicationId.substr(0, applicationId.find("\""));
                                // cout << "ID: " << applicationId << endl;
                            }

                            if (line.find("Executable=") != string::npos)
                            {
                                nameApp = line.substr(line.find("Executable=") + 11);
                                nameApp = nameApp.substr(0, nameApp.find(".exe"));
                                // cout << "Name: " << nameApp << endl;
                            }

                            if (!applicationId.empty() && !nameApp.empty())
                            {
                                break;
                            }
                        }

                        if (applicationId.empty() || nameApp.empty())
                        {
                            continue;
                        }

                        packageFamilyName = item->first.substr(1, item->first.size() - 2) + "!" + applicationId;
                        appSystem.emplace(nameApp, packageFamilyName);
                    }
                }
            }
            closedir(dir);
        } else {
            cout << "Not found path system " << item->second.c_str() << endl;
        }
    }
    // list to check
    cout << "List app found in System: " << endl;
    for (auto item = appSystem.begin(); item != appSystem.end(); item++)
    {
        cout << item->first  << " - " << item->second << endl;
    }

    // main 
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
            // case app system
            for (auto it = appSystem.begin(); it != appSystem.end(); it++) {
                if (it->first.find(appName) != string::npos) {
                    path = it->second;
                    break;
                }
            }
            string command = "powershell.exe -command \"explorer 'shell:AppsFolder\\" + path + "'\"";
            cout << "command: " << command << endl;
            cout << "command cstr: " << command.c_str() << endl;
            system(command.c_str());
        }
        else
        {
            string command = "powershell.exe -command \"explorer '" + path + "'\"";
            system(command.c_str());
        }
    } while (true);

    return 225;
}
