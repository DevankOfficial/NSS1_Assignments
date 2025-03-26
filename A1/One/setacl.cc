#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

#include "../utils/acl_file_data.hh"

using namespace std;

vector<string> sstrtok(const char* in, char delimit){
    vector<string> tokens;
    istringstream stream(in);
    string tok;
    while (getline(stream, tok, delimit)){
        tokens.push_back(tok);
    }
    return tokens;
}

//! \brief Only takes input as setacl <file_path> <new_acl>
//! \note <new_acl> has to follow the format -<type> <u/g/o>:<user/grp/other_name>:<perm>
int main(int argc, char* argv[]){
    if (argc<4){
        cerr<<"Incorrect usage!\nCorrect usage: setacl <file_path> <new_acl> <options..>"<<endl;
        return 1;
    }

    const char* file_path = argv[1];
    const string type = argv[2];
    vector<string> cluster = sstrtok(argv[3], ':');

    // Changing euid
    uid_t o_uid = getuid();
    uid_t owner_id = getFileOwnerId((const char*)file_path);
    if (setuid(owner_id) != 0){
        cerr<<"Error switching to root"<<endl;
        return 1;
    }

    //  Doing tasks
    string group_n;
    int group_id;
    bool isDir;
    struct stat filestat;
    if (stat(file_path, &filestat)==0){
        struct group *g = getgrgid(filestat.st_gid);
        if (g==nullptr){
            cerr<<"Couldn't get owner/group name"<<endl;
            return 1;
        }
        group_n = g->gr_name;
        
        isDir = S_ISDIR(filestat.st_mode);

        cout<<"MySetAcl -"<<endl;
    }else{
        cerr<<"Failed to get file info"<<endl;
        return 1;
    }

    file_data file_data_o(cluster[1], isDir);
    if (check_acl_permission(file_data_o, cluster[1], WRITE)){
        modify_acl(file_data_o, cluster[2], file_path);
        cout<<"ACL updated Successfully!"<<endl;
    }else{
        cerr<<"Permission Denied!"<<endl;
        return 1;
    }


    // Revering euid
    if (setuid(o_uid) != 0){
        cerr<<"Error switching back to original user"<<endl;
        return 1;
    }

    return 0;
}