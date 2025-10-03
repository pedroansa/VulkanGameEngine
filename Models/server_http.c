#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

// Wrappers para chamadas de socket seguindo convenção do livro
int Socket(int domain, int type, int protocol) {
    int sockfd = socket(domain, type, protocol);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
    return sockfd;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (bind(sockfd, addr, addrlen) < 0) {
        perror("bind");
        exit(1);
    }
}

void Listen(int sockfd, int backlog) {
    if (listen(sockfd, backlog) < 0) {
        perror("listen");
        exit(1);
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int connfd = accept(sockfd, addr, addrlen);
    if (connfd < 0) {
        perror("accept");
        exit(1);
    }
    return connfd;
}

pid_t Fork() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    return pid;
}

void Close(int fd) {
    if (close(fd) < 0) {
        perror("close");
        exit(1);
    }
}

ssize_t Read(int fd, void *buf, size_t count) {
    ssize_t n = read(fd, buf, count);
    if (n < 0) {
        perror("read");
        exit(1);
    }
    return n;
}

ssize_t Write(int fd, const void *buf, size_t count) {
    ssize_t n = write(fd, buf, count);
    if (n < 0) {
        perror("write");
        exit(1);
    }
    return n;
}

// Função para processar a requisição HTTP
void handle_request(int connfd, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    char *response_200 = 
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 91\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html><head><title>MC833</title></head><body><h1>MC833 TCP Concorrente</h1></body></html>";
    
    char *response_404 = "HTTP/1.0 404 Not Found\r\n\r\n";
    char *response_400 = "HTTP/1.0 400 Bad Request\r\n\r\n";

    // Ler a primeira linha da requisição
    ssize_t bytes_read = Read(connfd, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        
        // Extrair a primeira linha (até o \n)
        char *first_line = strtok(buffer, "\n");
        if (first_line) {
            // Imprimir informações da conexão e requisição
            printf("Conexão de %s:%d - Requisição: %s\n", 
                   inet_ntoa(client_addr.sin_addr), 
                   ntohs(client_addr.sin_port),
                   first_line);
            
            // Verificar se é GET / HTTP/1.0 ou GET / HTTP/1.1
            if (strstr(first_line, "GET / HTTP/1.0") != NULL || 
                strstr(first_line, "GET / HTTP/1.1") != NULL) {
                // Enviar resposta 200 OK
                Write(connfd, response_200, strlen(response_200));
            } 
            // Verificar se é outro método (POST, PUT, etc.)
            else if (strncmp(first_line, "GET ", 4) != 0) {
                // Enviar resposta 400 Bad Request para outros métodos
                Write(connfd, response_400, strlen(response_400));
            }
            else {
                // Enviar resposta 404 Not Found para outros caminhos
                Write(connfd, response_404, strlen(response_404));
            }
        } else {
            // Requisição mal formada
            Write(connfd, response_400, strlen(response_400));
        }
    } else {
        // Erro na leitura
        Write(connfd, response_400, strlen(response_400));
    }
    
    Close(connfd);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, connfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <porta>\n", argv[0]);
        exit(1);
    }
    
    int port = atoi(argv[1]);
    
    // Criar socket
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    // Configurar endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    // Bind
    Bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    // Listen
    Listen(sockfd, 5);
    
    printf("Servidor HTTP concorrente ouvindo na porta %d\n", port);
    
    while (1) {
        client_len = sizeof(client_addr);
        connfd = Accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
        
        // Criar processo filho para lidar com a conexão
        pid_t pid = Fork();
        if (pid == 0) {
            // Processo filho
            Close(sockfd); // Fechar socket de escuta no filho
            handle_request(connfd, client_addr);
        } else {
            // Processo pai
            Close(connfd); // Fechar socket conectado no pai
        }
    }
    
    Close(sockfd);
    return 0;
}