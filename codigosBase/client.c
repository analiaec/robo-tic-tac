#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8888


// struct (nossa) que guarda os angulos de cada junta em um dos locais para os quais o braco se movimenta
typedef struct {
    float j1;
    float j2;
    float j3;
    float j4;
    float j5;
    float j6;
} Juntas;


// usado para abrir ou fechar a garra/mao, ao fim de um movimento
#define OPEN 1
#define CLOSE 0

void delay(int delayTime) { // tempo de espera
    usleep(delayTime * 1000);
    
    printf("dando uma minipausa... ");
}

// mover as juntas para os angulos definidos
void movJoints(int sock, char* buffer, float j1, float j2, float j3, float j4, float j5, float j6, int state) {
    sprintf(buffer, "%f,%f,%f,%f,%f,%f,%d", j1, j2, j3, j4, j5, j6, state);
    send(sock, buffer, strlen(buffer), 0);
    int valread = read(sock, buffer, 2048);
    printf("Result: %s\n", buffer);
}


////// nossas funcoes

// funcao para ir ate a posicao inicial (acima do tabuleiro)
void inicio(int sock, char* buffer, Juntas* posinicio) {
    movJoints(sock, buffer, posinicio->j1, posinicio->j2, posinicio->j3, posinicio->j4, posinicio->j5, posinicio->j6, CLOSE); // fechado pq pode estar segurando peca
    
    printf("estou na posicao central\n");
    delay(500);
}

// funcao para pegar uma nova peca
void pegaPeca(int sock, char* buffer, Juntas* levanta, Juntas* peca) {
    movJoints(sock, buffer, levanta->j1, levanta->j2, levanta->j3, levanta->j4, levanta->j5, levanta->j6, OPEN); // vai ate a posicao acima das pecas
    printf("estou a postos para pegar uma peca \n");
    delay(500);

    movJoints(sock, buffer, peca->j1, peca->j2, peca->j3, peca->j4, peca->j5, peca->j6, CLOSE); // fecha na peca
    printf("peguei uma!\n");
    delay(1000);

    movJoints(sock, buffer, levanta->j1, levanta->j2, levanta->j3, levanta->j4, levanta->j5, levanta->j6, CLOSE); // levanta para a posicao intermediaria de novo
    printf("e levantei.\n");
    delay(500);
}

// funcao pra jogar (ir ate uma posicao na matriz/tabuleiro), que recebe os angulos das juntas na posicao da jogada
void jogaPeca(int sock, char* buffer, Juntas* quadrado) {
    movJoints(sock, buffer, quadrado->j1, quadrado->j2, quadrado->j3, quadrado->j4, quadrado->j5, quadrado->j6, OPEN); // vai ate o quadrado escolhido e solta a peca
    
    printf("joguei uma peca em %.2f %.2f %.2f %.2f %.2f %.2f\n", quadrado->j1, quadrado->j2, quadrado->j3, quadrado->j4, quadrado->j5, quadrado->j6);
    delay(1000);
}

// funcao unica unindo funcoes para uma jogada completa: pega a peca, joga e volta ao inicio
void jogadaCompleta(int sock, char* buffer, Juntas* peca, Juntas* levanta, Juntas* posinicio, Juntas* matrizXY) {
    pegaPeca(sock, buffer, levanta, peca);
    inicio(sock, buffer, posinicio);
    jogaPeca(sock, buffer, matrizXY); // ultimo parametro da funcao: matrizXY, eh a struct da posicao em que o braco deve jogar agora
    inicio(sock, buffer, posinicio);
}

///////

void initSocket(int* sock, char* buffer){

    struct sockaddr_in serv_addr;

    // Create a socket
    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(1);
    }

    // Set the server address and port
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_ADDRESS, &serv_addr.sin_addr) <= 0) {
        printf("\n Invalid address/ Address not supported \n");
        exit(1);
    }

    // Connect to the server
    if (connect(*sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Connection Failed \n");
        exit(1);
    }
}


int main(int argc, char *argv[]) {
    
    //variaveis para conexcao com o robo NAO MEXER
    int sock = 0;
    char buffer[2048] = {0};

    //conexao com o robo NAO MEXER
    initSocket(&sock, buffer);

    //Programe aqui ---------------------------------------------------


    // definicao das posicoes das juntas (atualizar sempre AQUI)
    Juntas posinicio = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // centro do tabuleiro (acima)
    Juntas levanta = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // um pouco acima do lugar de pegar pecas
    Juntas peca = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // lugar de pegar pecas
    Juntas matriz00 = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // linha 0 coluna 0 do tabuleiro
    Juntas matriz01 = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // linha 0 coluna 1 do tabuleiro
    Juntas matriz02 = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // linha 0 coluna 2 do tabuleiro
    Juntas matriz10 = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // linha 1 coluna 0 do tabuleiro
    Juntas matriz11 = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // linha 1 coluna 1 do tabuleiro
    Juntas matriz12 = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // linha 1 coluna 2 do tabuleiro
    Juntas matriz20 = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // linha 2 coluna 0 do tabuleiro
    Juntas matriz21 = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // linha 2 coluna 1 do tabuleiro
    Juntas matriz22 = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // linha 2 coluna 2 do tabuleiro


    while (1){
        // teste das funcoes: pega peca e joga no 00
        printf("\ntestando funcoes separadas (em 00): \n\n");
        pegaPeca(sock, buffer, &levanta, &peca);
        inicio(sock, buffer, &posinicio);
        jogaPeca(sock, buffer, &matriz00);
        inicio(sock, buffer, &posinicio);

        // teste de novo, com a funcao unica
        printf("\ntestando jogada completa, para todas as posicoes da matriz: \n\n");
        jogadaCompleta(sock, buffer, &peca, &levanta, &posinicio, &matriz00);
        jogadaCompleta(sock, buffer, &peca, &levanta, &posinicio, &matriz01);
        jogadaCompleta(sock, buffer, &peca, &levanta, &posinicio, &matriz02);
        jogadaCompleta(sock, buffer, &peca, &levanta, &posinicio, &matriz10);
        jogadaCompleta(sock, buffer, &peca, &levanta, &posinicio, &matriz11);
        jogadaCompleta(sock, buffer, &peca, &levanta, &posinicio, &matriz12);
        jogadaCompleta(sock, buffer, &peca, &levanta, &posinicio, &matriz20);
        jogadaCompleta(sock, buffer, &peca, &levanta, &posinicio, &matriz21);
        jogadaCompleta(sock, buffer, &peca, &levanta, &posinicio, &matriz22);

        //// deixa essa parte aqui em cima pra testar la na sexta feira todas as movimentacoes


    }

    //-----------------------------------------------------------------

    // Encerra a conexao NAO MEXER
    close(sock);

    return 0;
}
