#include <iostream>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/acl.h>

using namespace std;

void checkAccess(const char* fileName, uid_t user_id, mode_t mode) {
    // Check DAC (Permission bits)
    struct stat fileStat;
    stat(fileName, &fileStat);

    mode_t fileMode = fileStat.st_mode;
    if (((fileMode & mode) == mode) && (user_id == fileStat.st_uid)) {
        cout << "Access granted based on DAC." << endl;
    }

    // Check ACLs
    acl_t acl = acl_get_file(fileName, ACL_TYPE_ACCESS);
    if (acl == NULL) {
        // No ACLs found, access denied
        cout << "Access denied due to missing ACLs." << endl;
    }

    acl_entry_t acl_entry;
    ssize_t acl_entries = acl_get_entry(acl, ACL_FIRST_ENTRY, &acl_entry);
    while (acl_entries > 0) {
        uid_t acl_user_id;
        if (acl_get_tag_type(acl_entry, (acl_tag_t*)&acl_user_id) == ACL_USER &&
            acl_user_id == user_id) {
            acl_permset_t acl_permset;
            if (acl_get_permset(acl_entry, &acl_permset) == 0) {
                if (((acl_get_permset((acl_entry_t)acl_permset, (__acl_permset_ext**)ACL_READ) == 0) && (mode & S_IRUSR)) ||
                    (acl_get_permset((acl_entry_t)acl_permset, (__acl_permset_ext**)ACL_WRITE) == 0 && (mode & S_IWUSR))) {
                    cout << "Access granted based on ACL for user." << endl;
                    acl_free(acl);
                }
            }
        }

        acl_entries = acl_get_entry(acl, ACL_NEXT_ENTRY, &acl_entry);
    }

    cout << "Access denied based on both DAC and ACLs." << endl;
    acl_free(acl);
}

//! \brief Only takes input as checker <filename> <mode>
int main(int argc, char* argv[]) {
    if (argc!=3){
        cerr<<"Wrong input"<<endl;
    }
    uid_t user_id = getuid();
    const char* f = argv[1];
    mode_t mode = (mode_t)stoi(argv[2]);

    checkAccess(f, user_id, mode);

    return 0;
}