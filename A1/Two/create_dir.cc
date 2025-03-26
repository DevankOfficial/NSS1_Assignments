#include <iostream>
#include <fstream>
#include <unistd.h>
#include <filesystem>

#include "../utils/acl_file_data.hh"

using namespace std;

// //! \brief checks to see if parent directory has write permission
// bool canMkdir(filesystem::file_status& status){
//     return filesystem::is_directory(status) && 
//         ((status.permissions() & filesystem::perms::owner_write)!=filesystem::perms::none);
// }

//! \brief Only takes input as create_dir <dir_name>
int main(int argc, char* argv[]){
    if (argc!=2){
        cerr<<"Incorrect usage!\nCorrect usage: create_dir <dir_name>"<<endl;
        return 1;
    }

    const char* dir_path = argv[1];
    uid_t o_uid = getuid();
    // filesystem::file_status status = filesystem::status(dir_path);

    if (geteuid()==0 && setuid(0) != 0){
        cerr<<"Error switching to root"<<endl;
        return 1;
    }

    if ( mkdir(dir_path, 0777) == -1){
        cerr<<"Error creating directory"<<endl;
    }

    if (setuid(o_uid) != 0){
        cerr<<"Error switching back to original user"<<endl;
        return 1;
    }

    return 0;
}