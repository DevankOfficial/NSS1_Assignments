#include "encryption_util.hh"
#include <string>

unsigned char* generate_key(){
    int urandom_fd = open("/dev/urandom", O_RDONLY);
    if (urandom_fd == -1) {
        perror("Error opening /dev/urandom");
        return KEY_ERR_SIGNAL;
    }

    unsigned char* key = (unsigned char*)malloc(KEY_SIZE);
    if (read(urandom_fd, key, KEY_SIZE) != KEY_SIZE) {
        perror("Error reading from /dev/urandom");
        close(urandom_fd);
        free(key); 
        return KEY_ERR_SIGNAL;
    }
    close(urandom_fd);
    return key;
}

void generate_hmac(const char *key, const char * data, unsigned char* result, unsigned int *result_len) {
    const unsigned char *key_bytes = (const unsigned char *)key;
    const unsigned char *data_bytes = (const unsigned char *)data;
    size_t key_len = strlen(key);
    size_t data_len = strlen(data);
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    HMAC_CTX *hmac_ctx = HMAC_CTX_new();

    HMAC_Init_ex(hmac_ctx, key_bytes, key_len, EVP_sha256(), NULL);
    HMAC_Update(hmac_ctx, data_bytes, data_len);
    HMAC_Final(hmac_ctx, result, result_len);

    HMAC_CTX_free(hmac_ctx);
    EVP_MD_CTX_free(ctx);
}

bool validate_hmac(const char *key, const char *data, const unsigned char *received_hmac, unsigned int received_hmac_len) {
    unsigned char generated_hmac[EVP_MAX_MD_SIZE];
    unsigned int generated_hmac_len;
    
    generate_hmac(key, data, generated_hmac, &generated_hmac_len);

    if (received_hmac_len != generated_hmac_len || memcmp(received_hmac, generated_hmac, generated_hmac_len) != 0) {
        return false;
    }
    return CRYPTO_memcmp(generated_hmac, received_hmac, generated_hmac_len) == 0;
}

void encrypt(const char* plaintext, size_t plaintext_len, const unsigned char* key, char* iv, char* ciphertext) {
    EVP_CIPHER_CTX* ctx;
    int len, ciphertext_len;
    if (!(ctx = EVP_CIPHER_CTX_new())){
        perror("EVP_CIPHER_CTX_new Failed!");
        return ;
    }
    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, (const unsigned char*)iv) != 1){
        perror("EVP_EncryptInit_ex Failed!");
        return ;
    }
    EVP_CIPHER_CTX_set_padding(ctx, 1);
    if (EVP_EncryptUpdate(ctx, (unsigned char*)ciphertext, &len, (const unsigned char*)plaintext, plaintext_len) != 1){
        perror("EVP_EncryptUpdate Failed!");
        return ;
    }

    ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, (unsigned char*)ciphertext + len, &len) != 1){
        perror("EVP_EncryptFinal_ex Failed!");
        return ;
    }

    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
}

void decrypt(const char* ciphertext, size_t ciphertext_len, const unsigned char* key, const char* iv, char* decryptedtext) {
    EVP_CIPHER_CTX* ctx;
    int len, decryptedtext_len = ciphertext_len + EVP_MAX_BLOCK_LENGTH;
    OpenSSL_add_all_algorithms();
    if (!(ctx = EVP_CIPHER_CTX_new())){
        perror("EVP_CIPHER_CTX_new Failed!");
        exit(EXIT_FAILURE);
    }
    
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, (const unsigned char*)iv)){
        perror("EVP_DecryptInit_ex Failed!");
        exit(EXIT_FAILURE);
    }
    EVP_CIPHER_CTX_set_padding(ctx, 1);
    if (!EVP_DecryptUpdate(ctx, (unsigned char*)decryptedtext, &len, (const unsigned char*)ciphertext, ciphertext_len)){
        perror("EVP_DecryptUpdate Failed!");
        exit(EXIT_FAILURE);
    }

    decryptedtext_len = len;
    if (!EVP_DecryptFinal_ex(ctx, (unsigned char*)&decryptedtext + len, &len)){
        perror("EVP_DecryptFinal_ex Failed!");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    decryptedtext_len += len;
    decryptedtext[decryptedtext_len] = '\0';
    EVP_CIPHER_CTX_free(ctx);
    EVP_cleanup(); 
}

void send_ssh_file(int serv_socket, string text) {
    SSL_library_init();
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket"); exit(EXIT_FAILURE);
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(EMP_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("Invalid server address"); exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error connecting to server"); exit(EXIT_FAILURE);
    }

    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_set_fd(ssl, client_socket);
    if (SSL_connect(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Send encrypted text over the SSL connection
    if (SSL_write(ssl, text.c_str(), text.length()) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Clean up
    SSL_shutdown(ssl);
    close(client_socket);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
}

string recv_ssh_file(int clie_socket) {
    SSL_library_init();
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    SSL_set_fd(ssl, clie_socket);
    if (SSL_accept(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE]; 
    int received_bytes = SSL_read(ssl, buffer, sizeof(buffer));
    if (received_bytes <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_shutdown(ssl);
    close(clie_socket);
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    return string(buffer, received_bytes);
}