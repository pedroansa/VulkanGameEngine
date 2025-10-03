#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFFER_SIZE 4096

// Wrappers para chamadas de socket
int Socket(int domain, int type, int protocol) {
    int sockfd = socket(domain, type, protocol);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }
    return sockfd;
}

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (connect(sockfd, addr, addrlen) < 0) {
        perror("connect");
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

void Close(int fd) {
    if (close(fd) < 0) {
        perror("close");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr, local_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];
    
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP_servidor> <porta>\n", argv[0]);
        exit(1);
    }
    
    char *server_ip = argv[1];
    int port = atoi(argv[2]);
    
    // Criar socket
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    
    // Configurar endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    // Converter IP para formato numérico
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(1);
    }
    
    // Conectar ao servidor
    Connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    
    // Obter informações do socket local (cliente)
    addr_len = sizeof(local_addr);
    if (getsockname(sockfd, (struct sockaddr *)&local_addr, &addr_len) < 0) {
        perror("getsockname");
        exit(1);
    }
    
    // Obter informações do socket remoto (servidor) usando getpeername
    struct sockaddr_in remote_addr;
    socklen_t remote_len = sizeof(remote_addr);
    if (getpeername(sockfd, (struct sockaddr *)&remote_addr, &remote_len) < 0) {
        perror("getpeername");
        exit(1);
    }
    
    // Imprimir informações de conexão
    printf("=== Informações de Conexão ===\n");
    printf("Cliente (local): %s:%d\n", 
           inet_ntoa(local_addr.sin_addr), ntohs(local_addr.sin_port));
    printf("Servidor (remoto): %s:%d\n", 
           inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
    printf("==============================\n\n");
    
    // Preparar e enviar requisição HTTP
    char *request = "GET / HTTP/1.0\r\nHost: teste\r\n\r\n";
    printf("Enviando requisição:\n%s\n", request);
    
    Write(sockfd, request, strlen(request));
    
    // Ler e imprimir a resposta integralmente
    printf("=== Resposta do Servidor ===\n");
    ssize_t total_bytes = 0;
    ssize_t bytes_read;
    
    while ((bytes_read = Read(sockfd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
        total_bytes += bytes_read;
    }
    
    printf("\n=== Fim da Resposta ===\n");
    printf("Total de bytes recebidos: %zd\n", total_bytes);
    
    Close(sockfd);
    return 0;
}