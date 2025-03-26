#include <iostream>
#include <fstream>
#include <unistd.h>

#include "../utils/acl_file_data.hh"

using namespace std;

//! \brief Only takes input as fget <filename> <n>
int main(int argc, char* argv[]){
    if (argc!=3){
        cerr<<"Incorrect usage!\nCorrect usage: fget <filename> <n>"<<endl;
        return 1;
    }

    const char* file_path = argv[1];
    int n = stoi(argv[2]);
    char buffer[1024];
    uid_t o_uid = getuid();
    if (geteuid()==0 && setuid(0) != 0){
        cerr<<"Error switching to root"<<endl;
        return 1;
    }

    FILE* fp = fopen(file_path, "r");    
    fgets(buffer, n+1, fp);
    cout<<buffer<<endl;
    fclose(fp);

    if (setuid(o_uid) != 0){
        cerr<<"Error switching back to original user"<<endl;
        return 1;
    }

    return 0;
}