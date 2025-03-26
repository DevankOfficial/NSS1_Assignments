#ifndef ACL_FILE_DATA_HH
#define ACL_FILE_DATA_HH

#include <cstring>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <unordered_map>
#include <pwd.h>
#include <grp.h>
#include <vector>
using namespace std;

enum PERMS{
    NO_ACCESS = 0,
    EXECUTE = 1,
    WRITE = 2,
    READ = 4
};

struct permission{
    bool execute, read, write;
    permission(bool e, bool r, bool w):
        execute(e), read(r), write(w)
    {}
    permission():
        execute(NO_ACCESS), read(NO_ACCESS), write(NO_ACCESS)
    {}

    string aclPerms(){
        string p = (string)(read ? "r": "-") + (write ? "w": "-") + (execute ? "x": "-");
        return p;
    }
    void display(){
        cout<<(read ? "r": "-");
        cout<<(write ? "w": "-");
        cout<<(execute ? "x": "-")<<endl;
    }
    void displayOctal(){
        int octal = (read ? READ: 0) + (write ? WRITE: 0) + (execute ? EXECUTE: 0);
        cout<<octal;
    }
};

class acl{
public:
unordered_map<string, permission> acls;
    acl(){

    }
    void addPerm(const string& userName, bool exe, bool read, bool write){
        acls[userName] = permission(exe, read, write);
    }
    bool hasPerm(const string& userName, bool exe, bool read, bool write){
        if (acls.find(userName)!=acls.end()){
            const permission& p = acls[userName];
            return (p.execute >= exe) && (p.read >= read) && (p.write >= write);
        }
        return false;
    }
};

class file_data{
    string name;
    acl _acl;
    bool isDir;

public:
    file_data(string u, bool d): name(u), isDir(d) {}

    permission getPerm(){
        return _acl.acls[name];
    }

    // Not part of getacl, or setacl programs only OOP
    void setACL(const acl& acl_new){
        _acl = acl_new;
    }
    acl getACL(){
        return _acl;
    }
    void clearAcl(){
        _acl = acl();
        name="";
        isDir = false;
    }
};

bool check_acl_permission(file_data& f, string& username, int perm_requiered){
    if (f.getACL().acls.find(username)!=f.getACL().acls.end()){
        const permission& p = f.getACL().acls[username];
        switch(perm_requiered){
            case EXECUTE: return p.execute;
            case WRITE: return p.write;
            case READ: return p.read;
        }
    }
    return false;
}
 
void modify_acl(file_data& f, const string new_acl, const char* file_path){
    f.clearAcl();

    size_t start=0, end=0;
    while (end != std::string::npos){
        end = new_acl.find(",", start);
        string token = new_acl.substr(start, end-start);

        size_t colonPos = token.find(":");
        if (colonPos != std::string::npos){
            string user = token.substr(0, colonPos);
            string perms = token.substr(colonPos+1);
            bool r = (perms.find('r')!=std::string::npos),
                w = (perms.find('w')!=std::string::npos),
                x = (perms.find('x')!=std::string::npos);
            f.getACL().addPerm(user, x, r, w);
        }
        start = end+1;
    }

    ofstream out_file(file_path, ios::binary);
    if (!out_file){
        cerr<<"Error: Unable to open file for writing!"<<endl;
        return ;
    }
    out_file.write((const char*) &f, sizeof(file_data));
    out_file.close();
}

void print_acl(file_data& f, const char* file_name){
    struct stat filestat;
    string username, grp_name;
    if (stat(file_name, &filestat)!=0){
        username = getpwuid(filestat.st_uid)->pw_name;
        grp_name = getgrgid(filestat.st_gid)->gr_name;
    }else{
        cerr<<"Failed to get file info"<<endl;
        return;
    }
    cout<<"user::"<<f.getACL().acls[username].aclPerms()<<endl;
    cout<<"group::"<<f.getACL().acls[grp_name].aclPerms()<<endl;
    cout<<"other::"<<f.getACL().acls[username].aclPerms()<<endl;
}

uid_t getFileOwnerId(const char* filename){
    struct stat fileInfo;
    if (stat(filename, &fileInfo)!=0){
        return static_cast<uid_t>(-1);
    }
    return fileInfo.st_uid;
}

bool isIn(vector<string>& ar, string c){
    return find(ar.begin(), ar.end(), c) != ar.end();
}



#endif