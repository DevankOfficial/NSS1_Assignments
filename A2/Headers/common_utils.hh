#ifndef COMMON_UTILS_HH
#define COMMON_UTILS_HH

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

#define SALT_SIZE 16
#define KDC_PORT 5000
#define CHAT_PORT 6000


string pwd_hashing(const string& password);

bool performNSAuthentication(int socket, const User& user);
string generateNonce();

void sendIRCMessage(int socket, const string& message);
string receiveIRCMessage(int socket);

#endif