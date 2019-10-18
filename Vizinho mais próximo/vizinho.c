#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

typedef struct
{
    int qtdCidades;
    float **matrizDistancia;
    int *vetSolucao;
    float distTotal;
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

void preencheMatrizDistancia(int** matrizCoordenadas, int qtdCidades, int* selecionada, float** matrizDistancia, float x, float y){
    for (i = 0; i < qtdCidades; i++)
        {
            matrizDistancia[i] = (float *)malloc(qtdCidades * sizeof(int));
            selecionada[i] = 0;
            for (j = 0; j < qtdCidades; j++)
            {
                x = matrizCoordenadas[i][0] - matrizCoordenadas[j][0];
                y = matrizCoordenadas[i][1] - matrizCoordenadas[j][1];

                matrizDistancia[i][j] = sqrt(pow(x, 2) + pow(y, 2));
            }
        }
}

float vizinhoMaisProximo(int inicio, int l, Dados dados, int* selecionada, float FOStar){
    int distancia, k, aux;
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

    FOStar = dados.distTotal;
    for (i = 0; i < dados.qtdCidades; i++)
    {
        dados.vetSolucaoStar[i] = dados.vetSolucao[i];
    }

    for (i = 0; i < dados.qtdCidades; i++)
    {
        selecionada[i] = 0;
        dados.vetSolucao[i] = 0;
    }
    dados.distTotal = 0;

    return FOStar;
}

float vizinhoGRASP(int inicio, int l, Dados dados, int* selecionada, float FOStar, float alfa){
    int distancia, k, aux, distanciaMaior = -1, aux2, index, escolhido;
    int* candidatos;
    inicio = l;
    index = 1;
    dados.vetSolucao[0] = inicio;
    selecionada[inicio] = 1;

    srand(time(NULL));

    candidatos = (int *)malloc(dados.qtdCidades * sizeof(int));

    dados.distTotal = 0;

    // faz a busca pela solução atráves do vizinho mais próximo
    k = 0;
    for (i = 0; i < dados.qtdCidades - 1; i++)
    {
        distancia = 1000000;
        distanciaMaior = -1;
        for (j = 0; j < dados.qtdCidades; j++)
        {
            if (dados.matrizDistancia[dados.vetSolucao[k]][j] < distancia && dados.vetSolucao[k] != j && selecionada[j] == 0)
            {
                distancia = dados.matrizDistancia[dados.vetSolucao[k]][j];
                aux = j;
            }
            if (dados.matrizDistancia[dados.vetSolucao[k]][j] > distanciaMaior && dados.vetSolucao[k] != j && selecionada[j] == 0)
            {
                distanciaMaior = dados.matrizDistancia[dados.vetSolucao[k]][j];
                aux2 = j;
            }
        }

        candidatos[0] = aux;

        distanciaMaior /= 4;

        if (distanciaMaior < distancia)
        {
            distanciaMaior *= 4;
        }

        for (j = 0; j < dados.qtdCidades; ++j)
        {
            if ((distancia + alfa*(distanciaMaior - distancia)) > dados.matrizDistancia[dados.vetSolucao[k]][j] && selecionada[j] == 0 && j != aux)
            {
                candidatos[index] = j;
                index++;
            }
        }

        escolhido = rand() % index;

        dados.distTotal += dados.matrizDistancia[dados.vetSolucao[k]][candidatos[escolhido]];
        dados.vetSolucao[k+1] = candidatos[escolhido];

        selecionada[candidatos[escolhido]] = 1;
        k++;
        for (j = 0; j < dados.qtdCidades; j++)
        {
            candidatos[j] = 0;
        }
        index = 1;
    }

    // distancia entre a última e a primeira cidade
    dados.distTotal += dados.matrizDistancia[dados.vetSolucao[0]][dados.vetSolucao[dados.qtdCidades-1]];

    FOStar = dados.distTotal;
    for (i = 0; i < dados.qtdCidades; i++)
    {
        dados.vetSolucaoStar[i] = dados.vetSolucao[i];
    }

    for (i = 0; i < dados.qtdCidades; i++)
    {
        selecionada[i] = 0;
        dados.vetSolucao[i] = 0;
    }
    dados.distTotal = 0;

    return FOStar;
}

float doisOptBest(Dados dados, float FOStar){
        //2opt - bestimprovements
        int *solucaoAtual = (int *)malloc(dados.qtdCidades * sizeof(int));
        float FOvizinho, FOatual = FOStar, FOmelhorvizinho = FOStar;
        for (i = 0; i < dados.qtdCidades; i++)
        {
          solucaoAtual[i] = dados.vetSolucaoStar[i]; //salvando melhor vizinho
        }
        int ref1, ref2;
        int melhoria = 0;
        float retirada, inserida;
        while(melhoria==0){
            melhoria = 1;
            for(int i=1; i<dados.qtdCidades-2; i++){
                for(int j=i+1; j<dados.qtdCidades-2; j++){
                    retirada = dados.matrizDistancia[solucaoAtual[i-1]][solucaoAtual[i]] + dados.matrizDistancia[solucaoAtual[j]][solucaoAtual[j+1]];
                    inserida = dados.matrizDistancia[solucaoAtual[i]][solucaoAtual[j+1]] + dados.matrizDistancia[solucaoAtual[i-1]][solucaoAtual[j]];
                    FOvizinho = FOatual - retirada + inserida;
                    if(FOvizinho<FOmelhorvizinho){
                        melhoria = 0;
                        FOmelhorvizinho = FOvizinho;
                        ref1 = i;
                        ref2 = j;
//                        i = dados.qtdCidades;
//                        j = dados.qtdCidades;
                    }
                }
            }

            if(melhoria==0){
                int aux2 = 0;
                int n_trocas = ceil((ref2-ref1)/2);
                for(int aux2=0; aux2<=n_trocas;aux2++){
                    int aux = solucaoAtual[ref1+aux2];
                    solucaoAtual[ref1+aux2] = solucaoAtual[ref2-aux2];
                    solucaoAtual[ref2-aux2] = aux;
                }
                FOatual = FOmelhorvizinho;
            }
        }

        FOStar = FOatual;
        for (i = 0; i < dados.qtdCidades; i++)
        {
            dados.vetSolucaoStar[i] = solucaoAtual[i];

        }
        return FOStar;
}

float doisOptFirst(Dados dados, float FOStar){
        //2opt - bestimprovements
        int *solucaoAtual = (int *)malloc(dados.qtdCidades * sizeof(int));
        float FOvizinho, FOatual = FOStar, FOmelhorvizinho = FOStar;
        for (i = 0; i < dados.qtdCidades; i++)
        {
          solucaoAtual[i] = dados.vetSolucaoStar[i]; //salvando melhor vizinho
        }
        int ref1, ref2;
        int melhoria = 0;
        float retirada, inserida;
        while(melhoria==0){
            melhoria = 1;
            for(int i=1; i<dados.qtdCidades-2; i++){
                for(int j=i+1; j<dados.qtdCidades-2; j++){
                    retirada = dados.matrizDistancia[solucaoAtual[i-1]][solucaoAtual[i]] + dados.matrizDistancia[solucaoAtual[j]][solucaoAtual[j+1]];
                    inserida = dados.matrizDistancia[solucaoAtual[i]][solucaoAtual[j+1]] + dados.matrizDistancia[solucaoAtual[i-1]][solucaoAtual[j]];
                    FOvizinho = FOatual - retirada + inserida;
                    if(FOvizinho<FOmelhorvizinho){
                        melhoria = 0;
                        FOmelhorvizinho = FOvizinho;
                        ref1 = i;
                        ref2 = j;
                        i = dados.qtdCidades;
                        j = dados.qtdCidades;
                    }
                }
            }

            if(melhoria == 0){
                int aux2 = 0;
                int n_trocas = ceil((ref2-ref1)/2);
                for(int aux2=0; aux2<=n_trocas;aux2++){
                    int aux = solucaoAtual[ref1+aux2];
                    solucaoAtual[ref1+aux2] = solucaoAtual[ref2-aux2];
                    solucaoAtual[ref2-aux2] = aux;
                }
                FOatual = FOmelhorvizinho;
            }
        }

        FOStar = FOatual;
        for (i = 0; i < dados.qtdCidades; i++)
        {
            dados.vetSolucaoStar[i] = solucaoAtual[i];

        }
        return FOStar;
}

float orOptFirst (Dados dados, float FOStar){
    int *solucaoAtual = (int *)malloc(dados.qtdCidades * sizeof(int));
    float FOvizinho = 0, FOatual = FOStar, FOmelhorvizinho = FOStar;
    int melhoria = 0, retirada, inserida, ref1, ref2, aux;
    for (i = 0; i < dados.qtdCidades; i++)
    {
        solucaoAtual[i] = dados.vetSolucaoStar[i]; //salvando melhor vizinho
    }

    while(melhoria == 0)
    {
        melhoria = 1;
        for (int i = 1; i < dados.qtdCidades-1; ++i)
        {
            for (int j = i+1; j < dados.qtdCidades; ++j)
            {
                aux = solucaoAtual[i];
                solucaoAtual[i] = solucaoAtual[j];
                solucaoAtual[j] = aux;

                for (int k = 0; k < dados.qtdCidades - 1; k++)
                {
                    FOvizinho += dados.matrizDistancia[solucaoAtual[k]][solucaoAtual[k+1]];
                }

                FOvizinho += dados.matrizDistancia[solucaoAtual[0]][solucaoAtual[dados.qtdCidades-1]];


                if(FOvizinho < FOmelhorvizinho)
                {
                        melhoria = 0;
                        FOmelhorvizinho = FOvizinho;
                        i = dados.qtdCidades;
                        j = dados.qtdCidades;
                } else {
                        aux = solucaoAtual[i];
                        solucaoAtual[i] = solucaoAtual[j];
                        solucaoAtual[j] = aux;
                }
                FOvizinho = 0;
            }
        }
        if(melhoria == 0)
        {
            FOatual = FOmelhorvizinho;
        }
    }

    FOStar = FOatual;
    for (i = 0; i < dados.qtdCidades; i++)
    {
        dados.vetSolucaoStar[i] = solucaoAtual[i];
    }
    return FOStar;
}

float orOptBest (Dados dados, float FOStar){
    int *solucaoAtual = (int *)malloc(dados.qtdCidades * sizeof(int));
    float FOvizinho = 0, FOatual = FOStar, FOmelhorvizinho = FOStar;
    int melhoria = 0, retirada, inserida, ref1, ref2, aux;
    for (i = 0; i < dados.qtdCidades; i++)
    {
        solucaoAtual[i] = dados.vetSolucaoStar[i]; //salvando melhor vizinho
    }

    while(melhoria == 0)
    {
        melhoria = 1;
        for (int i = 1; i < dados.qtdCidades-1; ++i)
        {
            for (int j = i+1; j < dados.qtdCidades; ++j)
            {
                aux = solucaoAtual[i];
                solucaoAtual[i] = solucaoAtual[j];
                solucaoAtual[j] = aux;

                for (int k = 0; k < dados.qtdCidades - 1; k++)
                {
                    FOvizinho += dados.matrizDistancia[solucaoAtual[k]][solucaoAtual[k+1]];
                }

                FOvizinho += dados.matrizDistancia[solucaoAtual[0]][solucaoAtual[dados.qtdCidades-1]];


                if(FOvizinho < FOmelhorvizinho)
                {
                        melhoria = 0;
                        FOmelhorvizinho = FOvizinho;
                } else {
                        aux = solucaoAtual[i];
                        solucaoAtual[i] = solucaoAtual[j];
                        solucaoAtual[j] = aux;
                }
                FOvizinho = 0;
            }
        }
        if(melhoria == 0)
        {
            FOatual = FOmelhorvizinho;
        }
    }

    FOStar = FOatual;
    for (i = 0; i < dados.qtdCidades; i++)
    {
        dados.vetSolucaoStar[i] = solucaoAtual[i];
    }
    return FOStar;
}

int perturbacao(Dados dados)
{
    int num1, num2, num3, num4, num5, num6, aux, FOPerturbacao = 0;
    srand(time(NULL));

    num1 = rand() % dados.qtdCidades;
    num2 = rand() % dados.qtdCidades;
    num3 = rand() % dados.qtdCidades;
    num4 = rand() % dados.qtdCidades;
    num5 = rand() % dados.qtdCidades;
    num6 = rand() % dados.qtdCidades;

    aux = dados.vetSolucaoStar[num1];
    dados.vetSolucaoStar[num1] = dados.vetSolucaoStar[num4];
    dados.vetSolucaoStar[num4] = aux;

    aux = dados.vetSolucaoStar[num2];
    dados.vetSolucaoStar[num2] = dados.vetSolucaoStar[num6];
    dados.vetSolucaoStar[num6] = aux;

    aux = dados.vetSolucaoStar[num3];
    dados.vetSolucaoStar[num3] = dados.vetSolucaoStar[num5];
    dados.vetSolucaoStar[num5] = aux;

    for (int i = 0; i < dados.qtdCidades - 1; ++i)
    {
        FOPerturbacao += dados.matrizDistancia[dados.vetSolucaoStar[i]][dados.vetSolucaoStar[i+1]];
    }
    FOPerturbacao += dados.matrizDistancia[dados.vetSolucaoStar[0]][dados.vetSolucaoStar[dados.qtdCidades-1]];

    return FOPerturbacao;
}

int criterioAceitacao(int FOdeS,int FOPerturbacao, int FOBuscaLocal)
{
    if (FOdeS < FOBuscaLocal)
    {
        FOBuscaLocal = FOdeS;
    }

    if (FOPerturbacao < FOBuscaLocal)
    {
        FOBuscaLocal = FOPerturbacao;
    }

    return FOBuscaLocal;
}

int main(int argc, char *argv[ ])
{

    FILE *arq;
    FILE *arq2;
    Dados dados;
    int aux, inicio,  k, c = 0, iter = 0;
    float distancia, FOStar = 99999999, solucaoOtima,x, y, FOStarMulti = 9999999, alfa = 0.1, FOdeS, FOPerturbacao;
    int *melhorSolucaoMulti;
    int **matrizCoordenadas;
    int *solucaoUm;
    int *solucaoDois;
    int *selecionada;
    clock_t comeco, fim;
    double tempo;

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
        fscanf(arq, "%f", &solucaoOtima);

        dados.matrizDistancia = (float **)malloc(dados.qtdCidades * sizeof(int *));
        matrizCoordenadas = (int **)malloc(dados.qtdCidades * sizeof(int *));
        dados.vetSolucao = (int *)malloc(dados.qtdCidades * sizeof(int));
        selecionada = (int *)malloc(dados.qtdCidades * sizeof(int));
        dados.vetSolucaoStar = (int *)malloc(dados.qtdCidades * sizeof(int));
        melhorSolucaoMulti = (int *)malloc(dados.qtdCidades * sizeof(int));
        solucaoUm = (int *)malloc(dados.qtdCidades * sizeof(int));
        solucaoDois = (int *)malloc(dados.qtdCidades * sizeof(int));
        dados.distTotal = 0;

        // lê as coordenadas e preenche a matriz de distância entre as cidades
        lerCoordenadas(matrizCoordenadas,dados.qtdCidades,arq2,aux);
        preencheMatrizDistancia(matrizCoordenadas,dados.qtdCidades,selecionada,dados.matrizDistancia,x,y);

        comeco = clock();
        // for que percorre todas as cidades em busca da melhor solução
        for (int l = 0; l < dados.qtdCidades; l++){

            FOStar = vizinhoGRASP(inicio,l,dados,selecionada,FOStar,alfa);

            FOStar = 0;

            for (int i = 0; i < dados.qtdCidades - 1; ++i)
            {
                FOStar += dados.matrizDistancia[dados.vetSolucaoStar[i]][dados.vetSolucaoStar[i+1]];
            }

            FOStar += dados.matrizDistancia[dados.vetSolucaoStar[0]][dados.vetSolucaoStar[dados.qtdCidades-1]];

            FOStar = doisOptBest(dados,FOStar);
            //FOStar = orOptBest(dados,FOStar);

            if(FOStar < FOStarMulti)
            {
                FOStarMulti = FOStar;
                for(i = 0; i < dados.qtdCidades; i++)
                {
                    melhorSolucaoMulti[i] = dados.vetSolucaoStar[i];
                    solucaoUm[i] = dados.vetSolucaoStar[i];
                }
            }

            //ILS
           FOdeS = FOStarMulti;

            while(iter < 10)
            {
                FOPerturbacao = perturbacao(dados);
                FOStarMulti = doisOptBest(dados,FOPerturbacao);
                FOStarMulti = criterioAceitacao(FOdeS,FOPerturbacao,FOStarMulti);
                iter++;
            }
        }

        /*FOStarMulti = 0;

        for (int i = 0; i < dados.qtdCidades - 1; ++i)
        {
            FOStarMulti += dados.matrizDistancia[melhorSolucaoMulti[i]][melhorSolucaoMulti[i+1]];
        }

        FOStarMulti += dados.matrizDistancia[melhorSolucaoMulti[0]][melhorSolucaoMulti[dados.qtdCidades-1]];*/
       
        fim = clock();
        tempo = (double)(fim - inicio)/(double)CLOCKS_PER_SEC;
    }
    printf("***********************************************************************\n");
    printf("Arquivo: %s\n", argv[2]);
    printf("Solucao encontrada: %f\n", FOStarMulti);
    printf("Solucao Otima: %f \n", solucaoOtima);
    printf("GAP: %f \n", (100*(FOStarMulti-solucaoOtima)/solucaoOtima));
    printf("Tempo: %f\n", tempo);
    
    fclose(arq);
    fclose(arq2);
    free(matrizCoordenadas);
    free(selecionada);
    free(dados.matrizDistancia);
    free(dados.vetSolucao);

    return 0;
}
