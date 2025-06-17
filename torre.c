#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Definições de constantes
#define MAX_DISCOS 10
#define TORRES 3

// Estrutura da torre
typedef struct {
    int discos[MAX_DISCOS];
    int topo;
} Torre;

// Estrutura do histórico
typedef struct Historico {
    char jogador[50];
    int movimentos;
    int num_discos;
    char data[30];
    struct Historico* prox;
} Historico;

// Variáveis globais
Torre torres[TORRES];
Historico* listaHistorico = NULL;
int movimentos = 0, num_discos = 0;
char nomeJogador[50];

// Limpar buffer do teclado
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Inicializa torres com os discos
void inicializarTorres(int n) {
    num_discos = n;
    movimentos = 0;
    for (int i = 0; i < TORRES; i++)
        torres[i].topo = -1;
    for (int i = 0; i < n; i++)
        torres[0].discos[i] = n - i;
    torres[0].topo = n - 1;
}

// Coloca disco na torre
void push(int torre, int disco) {
    torres[torre].discos[++torres[torre].topo] = disco;
}

// Remove disco da torre
int pop(int torre) {
    if (torres[torre].topo == -1) return -1;
    return torres[torre].discos[torres[torre].topo--];
}

// Consulta topo da torre
int peek(int torre) {
    if (torres[torre].topo == -1) return -1;
    return torres[torre].discos[torres[torre].topo];
}

// Mostra graficamente as torres
void exibirTorres() {
    int maxLargura = num_discos * 2 - 1, espacoEntre = 8;
    for (int nivel = num_discos - 1; nivel >= 0; nivel--) {
        for (int t = 0; t < TORRES; t++) {
            int disco = (nivel <= torres[t].topo) ? torres[t].discos[nivel] : -1;
            int largura = (disco > 0 ? disco * 2 - 1 : 1);
            int espacos = (maxLargura - largura) / 2;
            for (int i = 0; i < espacos; i++) printf(" ");
            if (disco == -1) printf("|");
            else for (int i = 0; i < largura; i++) printf("=");
            for (int i = 0; i < espacos; i++) printf(" ");
            for (int i = 0; i < espacoEntre; i++) printf(" ");
        }
        printf("\n");
    }
    for (int t = 0; t < TORRES; t++) {
        for (int i = 0; i < maxLargura; i++) printf("-");
        for (int i = 0; i < espacoEntre; i++) printf(" ");
    }
    printf("\n");
    for (int t = 0; t < TORRES; t++) {
        int esq = (maxLargura - 1) / 2;
        for (int i = 0; i < esq; i++) printf(" ");
        printf("%c", 'A' + t);
        for (int i = 0; i < esq; i++) printf(" ");
        for (int i = 0; i < espacoEntre; i++) printf(" ");
    }
    printf("\n");
}

// Salva histórico da partida
void salvarHistorico() {
    FILE* arquivo = fopen("historico.txt", "a");
    if (!arquivo) { printf("Erro ao abrir historico.\n"); return; }

    time_t agora = time(NULL);
    char data[30];
    strftime(data, sizeof(data), "%d/%m/%Y %H:%M:%S", localtime(&agora));

    fprintf(arquivo, "Jogador: %s | Discos: %d | Movimentos: %d | Data: %s\n",
        nomeJogador, num_discos, movimentos, data);
    fclose(arquivo);

    Historico* novo = malloc(sizeof(Historico));
    if (!novo) { printf("Erro de memoria.\n"); exit(1); }
    strcpy(novo->jogador, nomeJogador);
    novo->num_discos = num_discos;
    novo->movimentos = movimentos;
    strcpy(novo->data, data);
    novo->prox = listaHistorico;
    listaHistorico = novo;
}

// Exibe histórico de partidas
void exibirHistorico() {
    if (!listaHistorico) { printf("\nNenhum historico disponivel.\n"); return; }
    printf("\n=== Historico de Partidas ===\n");
    for (Historico* aux = listaHistorico; aux; aux = aux->prox)
        printf("Jogador: %s | Discos: %d | Movimentos: %d | Data: %s\n",
            aux->jogador, aux->num_discos, aux->movimentos, aux->data);
}

// Busca por nome ou data no histórico
void buscarHistorico() {
    char bj[50] = "", bd[30] = "";
    printf("Nome do jogador (ou ENTER): ");
    fgets(bj, sizeof(bj), stdin);
    bj[strcspn(bj, "\n")] = 0;
    printf("Data (dd/mm/aaaa, ou ENTER): ");
    fgets(bd, sizeof(bd), stdin);
    bd[strcspn(bd, "\n")] = 0;

    printf("\n=== Resultados da Busca ===\n");
    int found = 0;
    for (Historico* aux = listaHistorico; aux; aux = aux->prox) {
        if ((strlen(bj)==0 || strstr(aux->jogador, bj)) &&
            (strlen(bd)==0 || strstr(aux->data, bd))) {
            printf("Jogador: %s | Discos: %d | Movimentos: %d | Data: %s\n",
                aux->jogador, aux->num_discos, aux->movimentos, aux->data);
            found = 1;
        }
    }
    if (!found) printf("Nenhum resultado encontrado.\n");
}

// Converte letra da torre em índice
int indiceTorre(char c) {
    c = toupper(c);
    if (c=='A') return 0;
    if (c=='B') return 1;
    if (c=='C') return 2;
    return -1;
}

// Move disco entre torres
int moverDisco(char origem, char destino) {
    int iO = indiceTorre(origem), iD = indiceTorre(destino);
    if (iO<0 || iD<0 || torres[iO].topo<0) {
        printf("Movimento invalido!\n");
        return 0;
    }
    int dO = peek(iO), dD = peek(iD);
    if (dD != -1 && dD < dO) {
        printf("Movimento invalido! Disco maior sobre menor.\n");
        return 0;
    }
    pop(iO); push(iD, dO);
    movimentos++;
    return 1;
}

// Verifica se jogo foi concluído
int jogoConcluido() {
    return torres[2].topo == num_discos - 1;
}

// Função principal do jogo
void jogar() {
    printf("Digite seu nome: ");
    fgets(nomeJogador, sizeof(nomeJogador), stdin);
    nomeJogador[strcspn(nomeJogador, "\n")] = 0;

    do {
        printf("Numero de discos (3 a %d): ", MAX_DISCOS);
        scanf("%d", &num_discos);
        limparBuffer();
    } while (num_discos < 3 || num_discos > MAX_DISCOS);

    inicializarTorres(num_discos);

    while (1) {
        exibirTorres();
        if (jogoConcluido()) {
            printf("Parabéns, %s! Completo em %d movimentos.\n", nomeJogador, movimentos);
            salvarHistorico();
            break;
        }

        char origem_input[10], destino_input[10];
        char origem, destino;

        opcao_origem:
        printf("Mover de (A/B/C/R=reiniciar/M=menu): ");
        if (!fgets(origem_input, sizeof(origem_input), stdin)) continue;
        origem = toupper(origem_input[0]);
        if (origem=='R') { inicializarTorres(num_discos); exibirTorres(); goto opcao_origem; }
        if (origem=='M') return;
        if (strchr("ABC", origem)==NULL) { printf("Entrada invalida.\n"); goto opcao_origem; }

        opcao_destino:
        printf("Para (A/B/C/R=reiniciar/M=menu): ");
        if (!fgets(destino_input, sizeof(destino_input), stdin)) goto opcao_destino;
        destino = toupper(destino_input[0]);
        if (destino=='R') { inicializarTorres(num_discos); exibirTorres(); goto opcao_origem; }
        if (destino=='M') return;
        if (strchr("ABC", destino)==NULL) { printf("Entrada invalida.\n"); goto opcao_destino; }

        if (!moverDisco(origem, destino))
            printf("Tente outro movimento.\n");
    }
}

// Função principal
int main() {
    int opcao;
    do {
        printf("\n=== Torre de Hanoi ===\n1. Jogar\n2. Ver historico\n3. Buscar historico\n4. Sair\nEscolha: ");
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao) {
            case 1: jogar(); break;
            case 2: exibirHistorico(); break;
            case 3: buscarHistorico(); break;
            case 4: printf("Saindo...\n"); break;
            default: printf("Opção inválida!\n");
        }
    } while (opcao != 4);

    // Libera memória do histórico
    while (listaHistorico) {
        Historico* temp = listaHistorico;
        listaHistorico = listaHistorico->prox;
        free(temp);
    }
    return 0;
}