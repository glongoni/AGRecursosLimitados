#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int*** graph;
    int* resourcesUperLimits;
    int* resourcesLowerLimits;
    int** verticesCosts;

    int numberOfVertices;
    int numberOfResources;
    int numberOfArrows;


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

        //Linhas das arestas
        int filePointer = 3 + numberOfVertices - 1; //Auxiliar para percorrer o arquivo
        for(int i = 0; i < numberOfArrows; i++)
        {
            filePointer++;

            QList<QByteArray> nextLine = everyThing.at(filePointer).split(' ');

            int verticeA = QString(nextLine.at(1)).toInt();
            int verticeB = QString(nextLine.at(2)).toInt();
            graph[verticeA][verticeB][0] = QString(nextLine.at(3)).toInt(); //Custo da aresta

            //Consumo de cada recurso na aresta
            for(int j = 0; j < numberOfResources; j++)
            {
                graph[verticeA][verticeB][j+1] = QString(nextLine.at(j+4)).toInt();
            }
        }
    }
    else
    {
        qDebug() << "Modo de uso: \\AGRecursosFinitos <instancia>.txt";
    }

    return a.exec();
}
