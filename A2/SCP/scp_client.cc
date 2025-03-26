#include "../Headers/encryption_util.hh"
using namespace std;

int main(int argc, char* argv[]){
    if (argc != 2) {
        cerr<<"Usage: "<<argv[0]<<"<local_file>\n";
        exit(1);
    }
    string local_file = argv[1];

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

    printf("Client Started\n");
    int pid = fork();
    if (pid == 0) { // Child process (encrypt and send to parent)
        close(pipefd[0]); 
        ifstream inputFile(local_file);
        if (!inputFile.is_open()) {
            cerr<< "Error opening to Read the file." <<endl;
            exit(EXIT_FAILURE);
        }
        // string plaintext;
        char buffer[BUFFER_SIZE];
        string encryptedText;
        unsigned char* key = generate_key(), *hmac, *iv=generate_key();
        
        while (!inputFile.eof()) {
            inputFile.read(buffer, BUFFER_SIZE);
            // plaintext.append(buffer, static_cast<size_t>(inputFile.gcount()));     
            encrypt(buffer, BUFFER_SIZE, (const unsigned char*)key, (char*)iv, (char*)encryptedText.c_str());
            generate_hmac((const char*)key, buffer, hmac, (unsigned int*)HMAC_SIZE);

            write(pipefd[1], encryptedText.c_str(), encryptedText.size() + 1);
            write(pipefd[1], key, KEY_SIZE + 1);
            write(pipefd[1], hmac, HMAC_SIZE + 1);
            write(pipefd[1], iv, IV_SIZE + 1);
            // plaintext.clear();
        }
        printf("File Encrypted\n");

        close(pipefd[1]); 
    } else { // Parent process
        close(pipefd[1]); 
        int status;
        waitpid(pid, &status, 0);

        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            perror("socket");
            return 1;
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(SSH_PORT); 
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        if (connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            perror("connect");
            close(serverSocket);
            return 1;
        }
        printf("Connected to Server\n");

        char buffer[BUFFER_SIZE];
        int i=1;
        while(select(pipefd[1] + 1, &readfds, nullptr, nullptr, &timeout)>0){
            read(pipefd[0], buffer, BUFFER_SIZE);
            string encryptedText(buffer);
            read(pipefd[0], buffer, KEY_SIZE);
            string key(buffer);
            read(pipefd[0], buffer, HMAC_SIZE);
            string hmac(buffer);
            read(pipefd[0], buffer, IV_SIZE);
            string iv(buffer);
            close(pipefd[0]);

            // Send encryptedText, key, HMAC, and IV to the server
            send_ssh_file(serverSocket, encryptedText);
            send(serverSocket, key.c_str(), KEY_SIZE + 1, 0);
            send(serverSocket, hmac.c_str(), HMAC_SIZE + 1, 0);
            send(serverSocket, iv.c_str(), IV_SIZE + 1, 0);
            printf("Encrypted text %d sent\n", i); i++;
        }
        close(serverSocket);
    }
}