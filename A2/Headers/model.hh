#ifndef MODEL_HH
#define MODEL_HH

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <string>
#include <vector>
using namespace std;

class irc_user{
    size_t id;
    string name, nickname;
    string password;
    string sessionKey;
    bool connected, kdc_certified;

public:
    void connect();
    bool is_connected() const {return connected;}

    void kon_kon_h(vector<irc_user> users); // who
    void write_all(); // write_all
    void write_to(size_t u_id); // write to one
    void create_group();
    void group_invite();
    void group_invite_accept();

    void request_pubkey(size_t u_id);
    void send_pubkey(size_t u_id);
    void init_grp_dhxchg();

    void write_grp(size_t g_id);
    void list_user_file();
}

struct irc_group{
    size_t id;
    string name;
    vector<irc_user> users;
}


#endif