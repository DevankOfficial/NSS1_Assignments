#include "common_utils.hh"

string hashPassword(const string& password) {
    const int iterations = 10000; // Adjust the number of iterations as needed

    // Generate a random salt
    unsigned char salt[SALT_SIZE];
    if (RAND_bytes(salt, SALT_SIZE) != 1) {
        std::cerr << "Error generating random salt\n";
        exit(EXIT_FAILURE);
    }

    // Derive the key using PBKDF2-HMAC-SHA256
    unsigned char key[SHA256_DIGEST_LENGTH];
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), salt, SALT_SIZE, iterations, EVP_sha256(), SHA256_DIGEST_LENGTH, key) != 1) {
        std::cerr << "Error deriving key using PBKDF2\n";
        exit(EXIT_FAILURE);
    }

    // Concatenate salt and derived key
    string hashedPassword(reinterpret_cast<char*>(salt), SALT_SIZE);
    hashedPassword += string(reinterpret_cast<char*>(key), SHA256_DIGEST_LENGTH);

    return hashedPassword;
}