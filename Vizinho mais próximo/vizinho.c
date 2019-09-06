#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

typedef struct
{
    int qtdCidades;
    int **matrizDistancia;
    int *vetSolucao;
    int distTotal;
    int *vetSolucaoStar;

} Dados;

int i,j;

void lerCoordenadas(int** matrizCoordenadas, int qtdCidades, FILE* arq2, int aux){
    for (i = 0; i < qtdCidades; i++)
        {
            matrizCoordenadas[i] = (int *)malloc(2 * sizeof(int));
            fscanf(arq2, "%d", &aux);

            for (j = 0; j < 2; j++)
            {
                fscanf(arq2, "%d", &matrizCoordenadas[i][j]);
            }
        }
}

void preencheMatrizDistancia(int** matrizCoordenadas, int qtdCidades, int* selecionada, int** matrizDistancia, int x, int y){
    for (i = 0; i < qtdCidades; i++)
        {
            matrizDistancia[i] = (int *)malloc(qtdCidades * sizeof(int));
            selecionada[i] = 0;
            for (j = 0; j < qtdCidades; j++)
            {
                x = matrizCoordenadas[i][0] - matrizCoordenadas[j][0];
                y = matrizCoordenadas[i][1] - matrizCoordenadas[j][1];

                matrizDistancia[i][j] = sqrt(pow(x, 2) + pow(y, 2));
            }
        }
}

void trocaPosicoes(Dados dados, int cidade1, int cidade2, int aux)
{
    for (i = 0; i < dados.qtdCidades; i++)
    {
        dados.vetSolucao[i] = dados.vetSolucaoStar[i];
    }
    
    for (i = cidade1, j = cidade2; i < (cidade2/2), j > ((cidade2/2)+1); i++, j--)
    {
        aux = dados.vetSolucao[i];
        dados.vetSolucao[i] = dados.vetSolucao[j];
        dados.vetSolucao[j] = aux;
    }
    
    dados.distTotal = 0;

    for (i = 0; i < dados.qtdCidades-1; i++)
    {
        dados.distTotal += dados.matrizDistancia[i][i+1];
    }
    dados.distTotal += dados.matrizDistancia[dados.vetSolucao[0]][dados.vetSolucao[dados.qtdCidades-1]];
}

int doisOptFI(Dados dados, int aux, int FOStar){
    for (i = 0; i < dados.qtdCidades-3; i++)
        {
            for (j = i+3; j < dados.qtdCidades; j++)
            {
                trocaPosicoes(dados,i+1,j-1,aux);
                if (dados.distTotal < FOStar)
                {
                    FOStar = dados.distTotal;
                    return FOStar;
                }
            } 
        }
        return FOStar;
}

int main(int argc, char *argv[ ])
{

    FILE *arq;
    FILE *arq2;
    Dados dados;
    int aux, inicio, x, y, k, distancia, solucaoOtima, c = 0, FOStar = 999999;
    int **matrizCoordenadas;
    int *selecionada;
    clock_t comeco, fim;
    double tempo;

    srand(time(NULL));
    arq = fopen(argv[1], "r"); 
    arq2 = fopen(argv[2], "r");

    if (arq == NULL || arq2 == NULL)
    {
        printf("Erro ao ler arquivo \n");
    }
    else
    {
        // lê o arquivo com a quantidade de cidades e a solução ótima
        fscanf(arq, "%d", &dados.qtdCidades);
        fscanf(arq, "%d", &solucaoOtima);

        dados.matrizDistancia = (int **)malloc(dados.qtdCidades * sizeof(int *));
        matrizCoordenadas = (int **)malloc(dados.qtdCidades * sizeof(int *));
        dados.vetSolucao = (int *)malloc(dados.qtdCidades * sizeof(int));
        selecionada = (int *)malloc(dados.qtdCidades * sizeof(int));
        dados.vetSolucaoStar = (int *)malloc(dados.qtdCidades * sizeof(int));
        dados.distTotal = 0;

        // lê as coordenadas e preenche a matriz de distância entre as cidades
        lerCoordenadas(matrizCoordenadas,dados.qtdCidades,arq2,aux);
        preencheMatrizDistancia(matrizCoordenadas,dados.qtdCidades,selecionada,dados.matrizDistancia,x,y);

        comeco = clock();
        // for que percorre todas as cidades em busca da melhor solução
        for (int l = 0; l < dados.qtdCidades; l++){
            inicio = l;
            dados.vetSolucao[0] = inicio;
            selecionada[inicio] = 1;

            // faz a busca pela solução atráves do vizinho mais próximo
            k = 0;
            for (i = 0; i < dados.qtdCidades - 1; i++)
            {
                distancia = 1000000;
                for (j = 0; j < dados.qtdCidades; j++)
                {
                    if (dados.matrizDistancia[dados.vetSolucao[k]][j] < distancia && dados.vetSolucao[k] != j && selecionada[j] == 0)
                        {
                            distancia = dados.matrizDistancia[dados.vetSolucao[k]][j];
                            dados.vetSolucao[k + 1] = j;
                            aux = j;
                        }
                }
                dados.distTotal += distancia;
                selecionada[aux] = 1;
                k++;
            }
            // distancia entre a última e a primeira cidade
            dados.distTotal += dados.matrizDistancia[dados.vetSolucao[0]][dados.vetSolucao[dados.qtdCidades-1]];

            if (dados.distTotal < FOStar)
            {
                FOStar = dados.distTotal;
                for (i = 0; i < dados.qtdCidades; i++)
                {
                    dados.vetSolucaoStar[i] = dados.vetSolucao[i];
                }
            }

            for (i = 0; i < dados.qtdCidades; i++)
            {
                selecionada[i] = 0;
                dados.vetSolucao[i] = 0;
            }
            dados.distTotal = 0;
        }

        FOStar = doisOptFI(dados,aux,FOStar);
        
        fim = clock();
        tempo = (double)(fim - inicio)/(double)CLOCKS_PER_SEC;
    }
    printf("***********************************************************************\n");
    printf("Arquivo: %s\n", argv[2]);
    printf("Solucao encontrada: %d\n", FOStar);
    printf("Solucao Otima: %d \n", solucaoOtima);
    printf("Tempo: %f\n", tempo);

    fclose(arq);
    fclose(arq2);
    free(matrizCoordenadas);
    free(selecionada);
    free(dados.matrizDistancia);
    free(dados.vetSolucao);

    return 0;
}