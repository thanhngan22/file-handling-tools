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
                result.emplace(filename.substr(0, filename.find(".lnk")), path);
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
                        locationAppStart.emplace(filename.substr(0, filename.find(".lnk")), listPaths[i]);
                    }
                    break;
                }
            }
            closedir(dir);
        }
    }

    // list to check
    // for (auto item = locationAppStart.begin(); item != locationAppStart.end(); item++) {
    //     cout << item->first <<
    //     " - " << item->second << endl;
    // }

    cout << " Nhập tên ứng dụng muốn mở: ";
    string appName;
    getline(cin, appName);

    // Xử lý
    string path = locationAppStart[appName];
    if (path.empty())
    {
        for (auto item = locationAppStart.begin(); item != locationAppStart.end(); item++)
        {
            if (item->first.find(appName) != string::npos)
            {
                path = item->second;
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



        system("powershell.exe -command \"explorer 'C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Word.lnk'\"; ");

        system("pause");
    }

    return 225;
}



/*

        Đường dẫn tới file shortcut Word
        string word_shortcut = "C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Word.lnk";

        Chuỗi lệnh PowerShell
        string ps_command = "start-process \"" + word_shortcut + "\"";
        string ps_command = "powershell.exe -command 'cd \\\"" + path + "\\\"; start-process \\\"" + appName + "\\\" -Verb RunAs'";
        string ps_command = "powershell.exe -command 'cd \\\"" + path + "\\\"; ls'";
        string ps_command = "powershell.exe -command 'ls'";

        string ps_command = "powershell.exe -command Start-Process \\\"" + word_shortcut + "\\\"";

        Chuyển đổi chuỗi lệnh sang kiểu LPWSTR
        LPSTR cmd = new char[ps_command.size() + 1];
        strcpy(cmd, ps_command.c_str());

        Khởi tạo cấu trúc STARTUPINFO và PROCESS_INFORMATION
        STARTUPINFO si = {};
        PROCESS_INFORMATION pi = {};
        si.cb = sizeof(si);

        Tạo tiến trình mới bằng hàm CreateProcess
        if (CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
        {
            Đợi tiến trình con hoàn thành và giải phóng tài nguyên
            WaitForSingleObject(pi.hProcess, INFINITE);
            cout << "Done" << endl;
            cout << "enter any key to exit" << endl;
            getchar();
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
        else
        {
            cout << "Error launching PowerShell cmdlet\n";
            return 1;
        }




*/