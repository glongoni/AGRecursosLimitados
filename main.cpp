#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QList>
#include <time.h>
#include <stdio.h>

#define GENERATION_SIZE 100
#define INFINIT 1073741823 // ~ 2^32

// Estrutura que guarda a instância do problema,
// Sendo que a posição AxBx0 é -1 se não existir aresta entre o vérticeA e o vérticeB
// E é o custo da aresta caso essa exista, e AxBxC com C > 0 é o quanto aquea aresta consome do recurso C
int*** graph;

// Estrutura para guardar os vizinhos de cada vértice, para facilitar na geração de soluções aleatórias
QList<QList<int>* > neighbors;

//Lista de soluções
QList<QList<int> > generation;
QList<uint> generationFitness;

int numberOfVertices;
int numberOfResources;
int numberOfArrows;

int* resourcesUperLimits;
int* resourcesLowerLimits;
int** verticesCosts;

QList<int> createRandomSolution()
{
    QList<int> toRet;

    //Sempre começa com o primeiro vértice
    int currentVertice = 1;

    //Enquanto não for o vértice final
    while(currentVertice < numberOfVertices)
    {
        toRet.append(currentVertice);

        int nextVertice = 0;
        int haveToGoBack = 0;
        do
        {
            haveToGoBack++;
            int nextVerticeIndex = qrand() % neighbors.at(currentVertice)->length();
            nextVertice = neighbors.at(currentVertice)->at(nextVerticeIndex);

        }while(toRet.contains(nextVertice) && haveToGoBack < 10);

        //Encontrou uma aresta válida, segue para o próximo vértice
        if(haveToGoBack < 10)
        {
            currentVertice = nextVertice;
        }
        //Chegou em um vértice em que não há saída sem criar loops, reinicia a geração da solução aleatória
        else
        {
            toRet.clear();
            currentVertice = 1;
        }
    }

    toRet.append(currentVertice);

    return toRet;
}

uint fitness(QList<int> beeing)
{
    uint fitness = 0;

    //Guarda a quantidade de recursos utilizada pela instancia beeing
    int res[numberOfResources + 1];

    //inicialização do uso dos recursos como '0'
    for(int i = 0; i < numberOfResources + 1; i++)
    {
        res[i] = 0;
    }

    for(int i = 0; i < beeing.size() - 1; i++)
    {
        //Incremento do fitness de acordo com o custo
        fitness += graph[beeing.at(i)][beeing.at(i+1)][0];

        //Cálculo de quanto foi gasto dos recursos
        for(int j = 1; j < numberOfResources + 1; j++)
        {
            res[j] += graph[beeing.at(i)][beeing.at(i+1)][j];
        }
    }

    //Verificação se estourou os recursos
    for(int i = 1; i < numberOfResources + 1; i++)
    {
        if(res[i] < resourcesLowerLimits[i] || res[i] > resourcesUperLimits[i])
        {
            //Se estourou os recursos o fitness é multado com infinito pois a solução não é válida
            fitness += INFINIT;
        }
    }

    return fitness;
}

void crossOver(QList<int>* beeingA, QList<int>* beeingB)
{
    QList<int> tabu;
    int verticeToCross = -1;

    do
    {
        tabu.append(verticeToCross);
        int verticeToCross = qrand() % beeingA.size();
    }
    while(!beeingB->contains(beeingA->at(verticeToCross)));


}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //Inicializa o gerador de números randomicos
    qsrand(time(NULL));

    if(argc > 1)
    {
        QFile* f = new QFile(QString(argv[1]));
        if(!f->open(QIODevice::ReadOnly))
        {
            qDebug() << "Arquivo inválido";
            return a.exec();
        }
        QByteArray buffer = f->readAll();
        f->close();

        //Inicio do parser
        QList<QByteArray> everyThing = buffer.split('\n');

        //Primeira Linha
        QList<QByteArray> firstLine = everyThing.at(0).split(' ');

        numberOfVertices = QString(firstLine.at(1)).toInt();
        numberOfArrows =  QString(firstLine.at(2)).toInt();
        numberOfResources =  QString(firstLine.at(3)).toInt();

        //Alocação da matriz de 3 dimensões do grafo
        graph = new int**[numberOfVertices + 1]; //Uma posição a mais para facilitar o parser
        for(int i = 0; i < numberOfVertices + 1; i++)
        {
            graph[i] = new int*[numberOfVertices + 1]; //Uma posição a mais para facilitar o parser
            for(int j = 0; j < numberOfVertices + 1; j++)
            {
                graph[i][j] = new int[numberOfResources + 1]; //Uma posição a mais e a '0' contém o custo da aresta

                //Inicialização, tudo -1
                for(int k = 0; k < numberOfResources + 1; k++)
                {
                    graph[i][j][k] = -1;
                }
            }
        }

        //Segunda linha e terceira linha
        QList<QByteArray> secoundLine = everyThing.at(1).split(' ');
        QList<QByteArray> thirdLine = everyThing.at(2).split(' ');

        //Alocação e preenchimento dos limites dos recursos
        resourcesLowerLimits = new int[numberOfResources];
        resourcesUperLimits =  new int[numberOfResources];

        for(int i = 0; i < numberOfResources; i++)
        {
            resourcesLowerLimits[i] = QString(secoundLine.at(i+1)).toInt();
            resourcesUperLimits[i] = QString(thirdLine.at(i+1)).toInt();
        }

        //Linhas de custos dos vértices
        //Alocação da matriz de custos dos vértices
        verticesCosts = new int*[numberOfVertices];
        for(int i = 0; i < numberOfVertices; i++)
        {
            verticesCosts[i] = new int[numberOfResources];
        }

        //Preenchimento da matriz de custos dos vértices
        for(int i = 0; i < numberOfVertices; i++)
        {
            QList<QByteArray> nextLine = everyThing.at(i + 3).split(' ');
            for(int j = 0; j < numberOfResources; j++)
            {
                verticesCosts[i][j] = QString(nextLine.at(j+1)).toInt();
            }
        }        


        //Inicializa as listas de vizinhos com uma lista vazia para cada vértice, isso facilitará a geração de soluções aleatórias
        //Esta também tem uma posição a mais para facilitar
        for(int i = 0; i < numberOfVertices+1; i++)
        {
            neighbors.append(new QList<int>());
        }

        //Linhas das arestas
        int filePointer = 3 + numberOfVertices - 1; //Auxiliar para percorrer o arquivo
        for(int i = 0; i < numberOfArrows; i++)
        {
            filePointer++;

            QList<QByteArray> nextLine = everyThing.at(filePointer).split(' ');

            int verticeA = QString(nextLine.at(1)).toInt();
            int verticeB = QString(nextLine.at(2)).toInt();
            graph[verticeA][verticeB][0] = QString(nextLine.at(3)).toInt(); //Custo da aresta

            //Coloca o verticeB na lista de vizinhos de A
            neighbors.at(verticeA)->append(verticeB);

            //Consumo de cada recurso na aresta
            for(int j = 0; j < numberOfResources; j++)
            {
                graph[verticeA][verticeB][j+1] = QString(nextLine.at(j+4)).toInt();
            }
        }

        //Gera a primeira geração aleatóriamente
        for(int i = 0; i < GENERATION_SIZE; i++)
        {
            generation.append(createRandomSolution());
        }

        for(int i = 0; i < GENERATION_SIZE; i++)
        {
            qDebug() << fitness(generation.at(i));
        }
    }
    else
    {
        qDebug() << "Modo de uso: \\AGRecursosFinitos <instancia>.txt";
    }

    return a.exec();
}
