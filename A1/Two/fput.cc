#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cstring>

#include "../utils/acl_file_data.hh"

using namespace std;

//! \brief Only takes input as fput <filename> <string_to_write>
int main(int argc, char* argv[]){
    if (argc!=3){
        cerr<<"Incorrect usage!\nCorrect usage: fput <filename> <string_to_write>"<<endl;
        return 1;
    }

    const char* file_path = argv[1];
    const char* input = argv[2];
    uid_t o_uid = getuid();

    if (geteuid()==0 && setuid(0) != 0){
        cerr<<"Error switching to root"<<endl;
        return 1;
    }

    FILE* fp = fopen(file_path, "a");    
    fputs(input, fp);
    fclose(fp);

    if (setuid(o_uid) != 0){
        cerr<<"Error switching back to original user"<<endl;
        return 1;
    }

    return 0;
}