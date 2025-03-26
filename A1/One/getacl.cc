#include <iostream>
#include <fstream>
#include <algorithm>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <vector>
#include <unistd.h>

#include "../utils/acl_file_data.hh"

using namespace std;

//! \brief Only takes input as getacl <file_path> <options...>
int main(int argc, char* argv[]){
    if (argc<2){
        cerr<<"Incorrect usage!\nCorrect usage: getacl <file_path> <options...>"<<endl;
        return 1;
    }

    const char* file_path = argv[1];
    vector<string> options;
    for (int i=2; i<argc; i++){
        options.emplace_back(argv[i]);
    }

    // Changing Euid
    uid_t o_uid = getuid();
    uid_t owner_id = getFileOwnerId(file_path);
    if (setuid(owner_id) != 0){
        cerr<<"Error switching to root"<<endl;
        return 1;
    }

    // Doing tasks as root
    string username, group_n;
    int userid, group_id;
    bool isDir;
    struct stat filestat;
    if (stat(file_path, &filestat)==0 && !isIn(options, "-c")){
        if (isIn(options, "-n")){
            int userid = filestat.st_uid;
            int group_id = filestat.st_gid;
        }else{
            struct passwd *p = getpwuid(filestat.st_uid);
            struct group *g = getgrgid(filestat.st_gid);
            if (p==nullptr || g==nullptr){
                cerr<<"Couldn't get owner/group name"<<endl;
                return 1;
            }
            username = p->pw_name; group_n = g->gr_name;
        }
        isDir = S_ISDIR(filestat.st_mode);

        cout<<"MyGetAcl -"<<endl;
        cout<<"# file: "<<file_path<<endl;
        if (isIn(options, "-n")){
            cout<<"# owner: "<<userid<<endl;
            cout<<"# group: "<<group_id<<endl;
        }else{
            cout<<"# owner: "<<username<<endl;
            cout<<"# group: "<<group_n<<endl;
        }
    }else{
        cerr<<"Failed to get file info"<<endl;
        return 1;
    }

    file_data file_data_o(username, isDir);
    if (check_acl_permission(file_data_o, username, READ)){
        print_acl(file_data_o, file_path);
    }else{
        cerr<<"Permission Denied!"<<endl;
        return 1;
    }

    // Reverting the userid to original
    if (setuid(o_uid) != 0){
        cerr<<"Error switching back to original user"<<endl;
        return 1;
    }

    return 0;
}