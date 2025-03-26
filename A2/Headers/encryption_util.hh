#ifndef ENCRYPTION_UTIL_HH
#define ENCRYPTION_UTIL_HH

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

using namespace std;

#define KEY_SIZE 32
#define HMAC_SIZE 64
#define IV_SIZE 32
#define BUFFER_SIZE 1024
#define SSH_PORT 22
#define EMP_PORT 50000
#define SERVER_IP "127.0.0.1"
#define KEY_ERR_SIGNAL reinterpret_cast<unsigned char*>(const_cast<char*>("\0"))

unsigned char* generate_key();

void generate_hmac(const char *key, const char *data, unsigned char* result, unsigned int *result_len);
bool validate_hmac(const char *key, const char *data, const unsigned char *received_hmac, unsigned int received_hmac_len);

void encrypt(const char* plaintext, size_t plaintext_len, const unsigned char* key, char* iv, char* ciphertext);
void decrypt(const char* ciphertext, size_t ciphertext_len, const unsigned char* key, const char* iv, char* decryptedtext);

void send_ssh_file(int serv_socket, string text);
string recv_ssh_file(int clie_socket);

#endif