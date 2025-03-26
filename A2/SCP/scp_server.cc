#include "../Headers/encryption_util.hh"
using namespace std;

int main(int argc, char* argv[]){
    if (argc != 2) {
        cerr<<"Usage: "<<argv[0]<<"<local_file> \n";
        exit(1);
    }
    const char* local_file = argv[1];

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(pipefd[0], &readfds); 
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    
    printf("Server Started\n");
    int pid = fork();
    if (pid == 0) { // Child process
        close(pipefd[0]);

        int serverSocket, clientSocket;
        struct sockaddr_in serverAddr, clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            perror("socket");
            return 1;
        }
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(SSH_PORT); 
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("bind");
            close(serverSocket);
            return 1;
        }
        if (listen(serverSocket, 5) == -1) {
            perror("listen");
            close(serverSocket);
            return 1;
        }
        printf("Listening for the client\n");
        sleep(2);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("accept");
            close(serverSocket);
            return 1;
        }
        printf("Connected to Client\n");

        int i=1;
        string key, hmac, iv, recievedText;
        while(accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen)>0){
            // Recieve encryptedText, key, HMAC, and IV to the server
            recievedText = recv_ssh_file(clientSocket);
            recv(serverSocket, &key, KEY_SIZE + 1, 0);
            recv(serverSocket, &hmac, HMAC_SIZE + 1, 0);
            recv(serverSocket, &iv, IV_SIZE + 1, 0);
            printf("Encrypted text %d Recieved!\n", i);

            write(pipefd[1], recievedText.c_str(), recievedText.size() + 1);
            write(pipefd[1], key.c_str(), KEY_SIZE + 1);
            write(pipefd[1], hmac.c_str(), HMAC_SIZE + 1);
            write(pipefd[1], iv.c_str(), IV_SIZE + 1);
            i++;
        }

        close(pipefd[1]); 
    } else { // Parent process
        close(pipefd[1]); 
        int status;
        waitpid(pid, &status, 0);

        ofstream outputFile(local_file);
        if (!outputFile.is_open()) {
            cerr << "Error opening to Write to the file." << endl;
            exit(EXIT_FAILURE);
        }

        char buffer[BUFFER_SIZE];
        int i=1;
        while(select(pipefd[0] + 1, &readfds, nullptr, nullptr, &timeout)>0){
            read(pipefd[0], buffer, BUFFER_SIZE);
            string encryptedText(buffer);
            read(pipefd[0], buffer, KEY_SIZE);
            string key(buffer);
            read(pipefd[0], buffer, HMAC_SIZE);
            string hmac(buffer);
            read(pipefd[0], buffer, IV_SIZE);
            string iv(buffer);
            close(pipefd[0]); 

            char* decrypted_text;
            decrypt(encryptedText.c_str(), encryptedText.size(), (const unsigned char*)key.c_str(), iv.c_str(), decrypted_text);
            if (!validate_hmac(key.c_str(), decrypted_text, (const unsigned char*)hmac.c_str(), hmac.size())){
                perror("Invalid HMAC detected!\n");
                exit(EXIT_FAILURE);
            }
            printf("HMAC %d Validated!\n", i++); 

            outputFile << decrypted_text;
        }
        outputFile.close();
        printf("Decrypted text successfully Written to file!\n");
    }
}