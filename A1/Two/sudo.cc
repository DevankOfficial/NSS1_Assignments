#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;

//! \brief Only takes input while running as ./sudo command args...
int main(int argc, char* argv[]){
    if (argc<2){
        cerr<<"Wrong Input!"<<endl;
        return 1;
    }
    const char* prog = argv[1]; 

    uid_t o_uid = getuid();
    if (geteuid()==0 && setuid(0) != 0){
        cerr<<"Error switching to root"<<endl;
        return 1;
    }
    if (execvp(prog, argv+1)==-1){
        cerr<<"Error during execution"<<endl;
        return 1;
    }
    if (setuid(o_uid) != 0){
        cerr<<"Error switching back to original user"<<endl;
        return 1;
    }

    return 0;
}