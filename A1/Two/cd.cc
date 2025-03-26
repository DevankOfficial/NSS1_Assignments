#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <filesystem>

#include "../utils/acl_file_data.hh"

using namespace std;

//! \brief Only takes input as cd <dir_name>
int main(int argc, char* argv[]){
    if (argc!=2){
        cerr<<"Incorrect usage!\nCorrect usage: cd <dir_name>"<<endl;
        return 1;
    }

    char* dir_path = argv[1];
    uid_t o_uid = getuid();
    if (geteuid()==0 && setuid(0) != 0){
        cerr<<"Error switching to root"<<endl;
        return 1;
    }

    if (chdir(dir_path) != 0){
        cerr<<"Error changing directory"<<endl;
    }
    cout<<"Current Dir -> "<<getcwd(dir_path, 100)<<endl;

    if (setuid(o_uid) != 0){
        cerr<<"Error switching back to original user"<<endl;
        return 1;
    }

    return 0;
}