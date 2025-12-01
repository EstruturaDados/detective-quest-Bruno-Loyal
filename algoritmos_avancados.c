#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição do tamanho máximo da Tabela Hash de Suspeitos
#define HASH_SIZE 10

// ------------------------------------------------
// 1. Estruturas de Dados
// ------------------------------------------------

// Estrutura para os cômodos da Mansão (Árvore Binária - Mapa)
typedef struct Sala {
    char nome[50];
    char pista_estatica[100]; // Pista que será coletada
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// Estrutura para os nós da Árvore Binária de Busca (BST de Pistas)
typedef struct PistaNode {
    char pista[100];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

// Estrutura para o Item da Tabela Hash (Lista Encadeada)
// Associa uma Pista (Chave) a um Suspeito (Valor)
typedef struct HashItem {
    char chave_pista[100];
    char valor_suspeito[50];
    struct HashItem *proximo;
} HashItem;

// Estrutura para a Tabela Hash (Array de ponteiros para HashItem)
typedef struct HashTable {
    HashItem *tabela[HASH_SIZE];
} HashTable;


// ------------------------------------------------
// 2. Funções de Criação e Mapa (Árvore Binária)
// ------------------------------------------------

/**
 * @brief Cria dinamicamente um cômodo (nó da árvore do mapa) com nome e pista.
 * @param nome_sala Nome do cômodo.
 * @param conteudo_pista Conteúdo da pista associada à sala.
 * @return Sala* Ponteiro para a nova sala criada.
 */
Sala* criarSala(char *nome_sala, char *conteudo_pista) {
    Sala* novaSala = (Sala*)malloc(sizeof(Sala));
    if (novaSala == NULL) {
        perror("Erro ao alocar memória para a sala");
        exit(1);
    }
    strcpy(novaSala->nome, nome_sala);
    strcpy(novaSala->pista_estatica, conteudo_pista);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}


// ------------------------------------------------
// 3. Funções da Tabela Hash (Pista -> Suspeito)
// ------------------------------------------------

// Função de Hash simples: Soma dos caracteres e Módulo
unsigned int hash(char *chave) {
    unsigned int valor = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        valor += chave[i];
    }
    return valor % HASH_SIZE;
}

/**
 * @brief Insere a associação Pista/Suspeito na Tabela Hash.
 * Usa encadeamento (chaining) para tratar colisões.
 * @param ht Ponteiro para a Tabela Hash.
 * @param pista Pista (Chave).
 * @param suspeito Suspeito (Valor).
 */
void inserirNaHash(HashTable *ht, char *pista, char *suspeito) {
    unsigned int indice = hash(pista);

    // Cria um novo item
    HashItem *novoItem = (HashItem*)malloc(sizeof(HashItem));
    if (novoItem == NULL) {
        perror("Erro ao alocar memória para o item hash");
        exit(1);
    }
    strcpy(novoItem->chave_pista, pista);
    strcpy(novoItem->valor_suspeito, suspeito);
    novoItem->proximo = NULL;

    // Inserção no início da lista encadeada (encadeamento)
    novoItem->proximo = ht->tabela[indice];
    ht->tabela[indice] = novoItem;
}

/**
 * @brief Consulta a Tabela Hash para encontrar o suspeito associado a uma pista.
 * @param ht Ponteiro para a Tabela Hash.
 * @param pista Pista (Chave) a ser procurada.
 * @return char* O nome do suspeito, ou "DESCONHECIDO" se a pista não estiver na Hash.
 */
char* encontrarSuspeito(HashTable *ht, char *pista) {
    unsigned int indice = hash(pista);
    HashItem *atual = ht->tabela[indice];

    while (atual != NULL) {
        if (strcmp(atual->chave_pista, pista) == 0) {
            return atual->valor_suspeito; // Suspeito encontrado
        }
        atual = atual->proximo;
    }

    return "DESCONHECIDO"; // Pista não aponta para nenhum suspeito mapeado
}


// ------------------------------------------------
// 4. Funções da BST (Pistas Coletadas)
// ------------------------------------------------

// Cria um nó para a BST de pistas
PistaNode* criarPistaNode(char *conteudo_pista) {
    PistaNode* novoNode = (PistaNode*)malloc(sizeof(PistaNode));
    if (novoNode == NULL) {
        perror("Erro ao alocar memória para o nó de pista");
        exit(1);
    }
    strcpy(novoNode->pista, conteudo_pista);
    novoNode->esquerda = NULL;
    novoNode->direita = NULL;
    return novoNode;
}

/**
 * @brief Insere uma nova pista na Árvore Binária de Busca (BST).
 * @param raiz Raiz atual da BST.
 * @param pista_coletada Pista a ser inserida.
 * @return PistaNode* Raiz da BST (atualizada ou original).
 */
PistaNode* inserirPista(PistaNode* raiz, char *pista_coletada) {
    if (raiz == NULL) {
        printf("\n[PISTA ENCONTRADA]: \"%s\" adicionada ao dossiê.\n", pista_coletada);
        return criarPistaNode(pista_coletada);
    }

    // Compara para decidir se vai para esquerda (<0) ou direita (>0)
    int comparacao = strcmp(pista_coletada, raiz->pista);

    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista_coletada);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, pista_coletada);
    }
    // Se for igual, a pista já foi coletada (não insere de novo)

    return raiz;
}

/**
 * @brief Imprime as pistas na BST em ordem alfabética (Inorder).
 * @param raiz Raiz da BST.
 */
void exibirPistas(PistaNode* raiz) {
    if (raiz != NULL) {
        exibirPistas(raiz->esquerda);
        printf("- %s\n", raiz->pista);
        exibirPistas(raiz->direita);
    }
}


// ------------------------------------------------
// 5. Funções de Exploração e Lógica Final
// ------------------------------------------------

/**
 * @brief Navega pela árvore de cômodos, coleta pistas e atualiza a BST.
 * @param mapa_raiz Raiz do mapa (Hall de Entrada).
 * @param pistas_raiz Ponteiro para o ponteiro da raiz da BST de pistas.
 */
void explorarSalas(Sala *mapa_raiz, PistaNode **pistas_raiz) {
    Sala *atual = mapa_raiz;
    char escolha;

    printf("\n--- EXPLORAÇÃO INICIADA: DETECTIVE QUEST ---\n");

    while (1) {
        printf("\n==========================================\n");
        printf("VOCÊ ESTÁ EM: **%s**\n", atual->nome);

        // Verifica se há pista para coletar e insere na BST
        if (strcmp(atual->pista_estatica, "") != 0) {
            printf("[ALERTA]: Pista detectada! Coletando...\n");
            *pistas_raiz = inserirPista(*pistas_raiz, atual->pista_estatica);
            // Limpa a pista da sala para que não seja coletada de novo
            strcpy(atual->pista_estatica, ""); 
        }

        // Verifica caminhos disponíveis
        char caminhos[100] = "";
        if (atual->esquerda) {
            strcat(caminhos, " [e] Esquerda");
        }
        if (atual->direita) {
            strcat(caminhos, " [d] Direita");
        }

        if (caminhos[0] == '\0') {
            printf("\nFIM DE LINHA: Este cômodo não tem mais caminhos.\n");
        }
        
        printf("Caminhos disponíveis:%s | [s] Sair da exploração\n", caminhos);
        printf("Sua escolha (e/d/s): ");
        scanf(" %c", &escolha);

        if (escolha == 's' || escolha == 'S') {
            printf("\n--- O detetive optou por encerrar a investigação na mansão ---\n");
            break;
        }

        Sala *proxima = NULL;
        if ((escolha == 'e' || escolha == 'E') && atual->esquerda) {
            proxima = atual->esquerda;
        } else if ((escolha == 'd' || escolha == 'D') && atual->direita) {
            proxima = atual->direita;
        } else {
            printf("Escolha inválida ou caminho bloqueado. Tente novamente.\n");
        }

        if (proxima != NULL) {
            atual = proxima;
        }
    }
}

/**
 * @brief Conta as pistas coletadas que apontam para o suspeito acusado.
 * @param raiz Raiz da BST de pistas.
 * @param ht Tabela Hash com as associações Pista/Suspeito.
 * @param acusado O nome do suspeito acusado pelo jogador.
 * @param contador Ponteiro para a contagem (usado para passar a contagem por referência).
 */
void contarPistasPorSuspeito(PistaNode *raiz, HashTable *ht, char *acusado, int *contador) {
    if (raiz != NULL) {
        // 1. Visita o nó da esquerda
        contarPistasPorSuspeito(raiz->esquerda, ht, acusado, contador);

        // 2. Processa o nó atual: Verifica se a pista aponta para o acusado
        char *suspeito_associado = encontrarSuspeito(ht, raiz->pista);
        if (strcmp(suspeito_associado, acusado) == 0) {
            (*contador)++;
            printf("  -> [VÁLIDA]: \"%s\" associa-se a %s.\n", raiz->pista, acusado);
        } else if (strcmp(suspeito_associado, "DESCONHECIDO") != 0) {
             printf("  -> [IRRELEVANTE]: \"%s\" associa-se a %s.\n", raiz->pista, suspeito_associado);
        } else {
             printf("  -> [SEM ASSOCIAÇÃO]: \"%s\" não está mapeada para nenhum suspeito conhecido.\n", raiz->pista);
        }

        // 3. Visita o nó da direita
        contarPistasPorSuspeito(raiz->direita, ht, acusado, contador);
    }
}

/**
 * @brief Conduz a fase de julgamento final e verifica as evidências.
 * @param pistas_raiz Raiz da BST com todas as pistas coletadas.
 * @param ht Tabela Hash de associações.
 */
void verificarSuspeitoFinal(PistaNode *pistas_raiz, HashTable *ht) {
    char acusado[50];
    int pistas_validas = 0;

    printf("\n\n###########################################\n");
    printf("--- FASE FINAL: ACUSAÇÃO DO CULPADO ---\n");
    printf("###########################################\n");

    if (pistas_raiz == NULL) {
        printf("Não há pistas coletadas. O caso não pode ser concluído.\n");
        return;
    }

    printf("\nSuspeitos principais: 'Mordomo', 'Cozinheira', 'Jardineiro'\n");
    printf("Acusação: Por favor, digite o nome do culpado: ");
    scanf(" %49s", acusado); // Limita a 49 caracteres para evitar buffer overflow

    printf("\n--- VERIFICAÇÃO DE EVIDÊNCIAS CONTRA %s ---\n", acusado);
    contarPistasPorSuspeito(pistas_raiz, ht, acusado, &pistas_validas);
    
    printf("\nRESUMO: %s está relacionado(a) a %d pistas válidas.\n", acusado, pistas_validas);

    // Requisito de verificação: pelo menos duas pistas
    if (pistas_validas >= 2) {
        printf("\n=> SUCESSO! HÁ EVIDÊNCIAS SUFICIENTES PARA PRENDER %s! CASO RESOLVIDO.\n", acusado);
    } else {
        printf("\n=> FRACASSO! EVIDÊNCIAS INSUFICIENTES. %s FOI LIBERADO(A). A BUSCA CONTINUA.\n", acusado);
    }
}


// ------------------------------------------------
// 6. Funções de Limpeza de Memória
// ------------------------------------------------

void liberarMapa(Sala *raiz) {
    if (raiz != NULL) {
        liberarMapa(raiz->esquerda);
        liberarMapa(raiz->direita);
        free(raiz);
    }
}

void liberarPistas(PistaNode *raiz) {
    if (raiz != NULL) {
        liberarPistas(raiz->esquerda);
        liberarPistas(raiz->direita);
        free(raiz);
    }
}

void liberarHash(HashTable *ht) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashItem *atual = ht->tabela[i];
        HashItem *proximo;
        while (atual != NULL) {
            proximo = atual->proximo;
            free(atual);
            atual = proximo;
        }
        ht->tabela[i] = NULL;
    }
}


// ------------------------------------------------
// 7. Função Principal (main)
// ------------------------------------------------

int main() {
    // Inicialização da BST de Pistas e da Tabela Hash
    PistaNode *dossie_pistas = NULL;
    HashTable hash_suspeitos = { .tabela = {NULL} };

    // --- MONTAGEM DO MAPA DA MANSÃO (Árvore Binária) ---
    // Nível 0 (Raiz)
    Sala *hall_entrada = criarSala("Hall de Entrada", "Um relógio de bolso com as iniciais 'M.S.' foi derrubado na entrada.");

    // Nível 1
    Sala *sala_estar = criarSala("Sala de Estar", "");
    Sala *jardim_inverno = criarSala("Jardim de Inverno", "Havia terra recém-cavada sob o vaso de palmeira.");

    hall_entrada->esquerda = sala_estar;
    hall_entrada->direita = jardim_inverno;

    // Nível 2
    Sala *cozinha = criarSala("Cozinha", "Uma colher de prata foi abandonada na pia, suja de molho.");
    Sala *biblioteca = criarSala("Biblioteca", "A chave do armário de licores foi deixada entre os livros de botânica.");
    Sala *escritorio = criarSala("Escritório", "");
    Sala *piscina = criarSala("Área da Piscina", "Encontrei um boné de jardineiro flutuando na borda.");

    sala_estar->esquerda = cozinha;
    sala_estar->direita = biblioteca;
    jardim_inverno->esquerda = escritorio;
    jardim_inverno->direita = piscina;

    // Nível 3 (Nós-folha com pistas importantes)
    cozinha->esquerda = criarSala("Despensa", "Uma embalagem de remédio para alergia estava escondida sob um saco de farinha.");
    cozinha->direita = criarSala("Porão", "Um pedaço de pano manchado de vinho tinto foi deixado perto da escada.");
    biblioteca->esquerda = criarSala("Quarto de Hóspedes", "");
    biblioteca->direita = criarSala("Sótão", "Um diário menciona um encontro secreto no jardim.");


    // --- MONTAGEM DA TABELA HASH (Associa Pista -> Suspeito) ---
    // Pistas associadas ao Mordomo
    inserirNaHash(&hash_suspeitos, "Um relógio de bolso com as iniciais 'M.S.' foi derrubado na entrada.", "Mordomo");
    inserirNaHash(&hash_suspeitos, "A chave do armário de licores foi deixada entre os livros de botânica.", "Mordomo");

    // Pistas associadas ao Jardineiro
    inserirNaHash(&hash_suspeitos, "Havia terra recém-cavada sob o vaso de palmeira.", "Jardineiro");
    inserirNaHash(&hash_suspeitos, "Encontrei um boné de jardineiro flutuando na borda.", "Jardineiro");
    inserirNaHash(&hash_suspeitos, "Um diário menciona um encontro secreto no jardim.", "Jardineiro");

    // Pistas associadas à Cozinheira
    inserirNaHash(&hash_suspeitos, "Uma colher de prata foi abandonada na pia, suja de molho.", "Cozinheira");
    inserirNaHash(&hash_suspeitos, "Uma embalagem de remédio para alergia estava escondida sob um saco de farinha.", "Cozinheira");


    // --- INÍCIO DO JOGO ---
    explorarSalas(hall_entrada, &dossie_pistas);

    printf("\n--- PISTAS COLETADAS (Em Ordem Alfabética) ---\n");
    exibirPistas(dossie_pistas);
    printf("------------------------------------------\n");

    // --- VERIFICAÇÃO FINAL ---
    verificarSuspeitoFinal(dossie_pistas, &hash_suspeitos);


    // --- LIMPEZA DE MEMÓRIA ---
    liberarMapa(hall_entrada);
    liberarPistas(dossie_pistas);
    liberarHash(&hash_suspeitos);

    return 0;
}