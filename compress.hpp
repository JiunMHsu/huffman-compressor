
#include <iostream>

#include "biblioteca/funciones/strings.hpp"
#include "biblioteca/funciones/tokens.hpp"
#include "biblioteca/funciones/files.hpp"
#include "biblioteca/tads/Array.hpp"
#include "biblioteca/tads/Map.hpp"
#include "biblioteca/tads/List.hpp"
#include "biblioteca/tads/Stack.hpp"
#include "biblioteca/tads/Queue.hpp"
#include "biblioteca/tads/Coll.hpp"

#include "biblioteca/tads/huffman/HuffmanSetup.hpp"
#include "biblioteca/tads/BitReader.hpp"
#include "biblioteca/tads/BitWriter.hpp"

using namespace std;

typedef unsigned char uchar;

// inicializa los campos del huffman table
void tableInit(HuffmanTable table[])
{
    for (int i = 0; i < 256; i++)
    {
        table[i] = { 0, "" };
    }
}

// incrementa las ocurrencias de cada char
void countOccurrences(string fName, HuffmanTable table[])
{
    FILE* f = fopen(formatString(fName), "r+b");
    char c = read<char>(f);
    while (!feof(f))
    {
        int idx = c; // conversion directa de char a int (toma el valor ASCII)
        // usar directamente el char como argumento del campo index resultara en un warning
        table[idx].n += 1;
        c = read<char>(f);
    }
    fclose(f);
}

// los retornos de esta funcion no reflejan la comparacion perse
// sino que declara unicamente los casos en donde se debe ordenar ( retorno del valor {1} )
// cualquier otro caso retornara {-1} indiferentemente
//
// criterio:
// de menor frecuencia a mayor frecuencia
// para mismas ocurrencias, ordenar alfabeticamente
int cmpHtInfo(HuffmanTreeInfo* a, HuffmanTreeInfo* b) // revisar si funca bien
{
    return ((a->n > b->n) || (a->n == b->n && a->c > b->c)) ? 1 : -1;
}

// crea la lista agregando nodos, y ordena
void createCharList(List<HuffmanTreeInfo*>& charList, HuffmanTable table[])
{
    for (int i = 0; i < 256; i++)
    {
        long freq = table[i].n;
        if (freq > 0)
        {
            // 'i' representa el codigo ASCII
            HuffmanTreeInfo* htInfo = huffmanTreeInfo(i, freq, NULL, NULL);
            listAdd<HuffmanTreeInfo*>(charList, htInfo);
        }
    }
    listSort<HuffmanTreeInfo*>(charList, cmpHtInfo);
}

// convierte la lista en arbol y retorna el puntero a la raiz
HuffmanTreeInfo* createHuffmanTree(List<HuffmanTreeInfo*>& charList)
{
    // al finalizar la iteracion deberia quedar un solo nodo en la lista
    // el cual contendra todo el arbol
    for (int i = 1; listSize(charList) > 1; i++)
    {
        // obtener los primeros dos infos
        HuffmanTreeInfo* infoA = listRemoveFirst<HuffmanTreeInfo*>(charList);
        HuffmanTreeInfo* infoB = listRemoveFirst<HuffmanTreeInfo*>(charList);

        // crear un nuevo htInfo el cual los apunte
        HuffmanTreeInfo* newInfo = huffmanTreeInfo(256 + i, infoA->n + infoB->n, infoB, infoA);

        // insertar a la lista como nodo (respetando el criterio de ordenamiento)
        listOrderedInsert<HuffmanTreeInfo*>(charList, newInfo, cmpHtInfo);
    }

    // retorna la raiz del arbol huffman removiendo el ultimo nodo sobrante
    // ya que el puntero 'raiz' deberia apuntar directo al arbol y no a la lista
    return listRemoveFirst<HuffmanTreeInfo*>(charList);
}

// iterar sobre el arbol y almacenar el codigo
void encode(HuffmanTreeInfo* root, HuffmanTable table[])
{
    HuffmanTree ht = huffmanTree(root);

    while (huffmanTreeHasNext(ht))
    {
        string hfCode;
        HuffmanTreeInfo* leaf = huffmanTreeNext(ht, hfCode);

        table[leaf->c].code = hfCode;
    }
}

int countLeafs(HuffmanTable table[])
{
    int a = 0;
    for (int i = 0; i < 256; i++)
    {
        a = (table[i].n > 0) ? a + 1 : a;
    }
    return a;
}

void generateCompressedFile(string fName, HuffmanTable table[])
{
    // crear nuevo archivo (.huf)
    string hufName = fName + ".huf";
    FILE* fHuffman = fopen(formatString(hufName), "w+b");

    // inicializar el bit writer
    BitWriter hufBW = bitWriter(fHuffman);

    // grabar la informacion del arbol
    uchar t = countLeafs(table); // cantidad de nodos 'char', es decir, cantidad de registros INFO
    write<uchar>(fHuffman, t);

    // estructura de los registros INFO
    // { char, huf code length, huf code }
    // longitud maxima del condigo huffman es la mited del total de chars

    int maxHufCodeLen = ((t % 2) == 0) ? t / 2 : (t + 1) / 2;

    for (int i = 0; i < 256; i++)
    {
        if (table[i].n == 0) continue;

        write<uchar>(fHuffman, (uchar)i);               // registrar el char (ASCII)
        string hufCode = table[i].code;                 // obtener el codigo huffman
        write<uchar>(fHuffman, (uchar)length(hufCode)); // registrar la longitud del codigo

        if (length(hufCode) < maxHufCodeLen)
        {
            // completa el huf code a la longitud maxima
            // forzando que todos los codigod tengan la misma longitud
            // antes de ser grabado por el birWritter
            hufCode = rpad(hufCode, maxHufCodeLen, '0');
        }

        for (int n = 0; n < length(hufCode); n++) // registrar el huffman code
        {
            char bit = hufCode[n]; // '0' / '1'
            bitWriterWrite(hufBW, charToInt(bit));
        }
        bitWriterFlush(hufBW); // completa el byte para que pueda ser grabado en el archivo
    }

    // leer el archivo a comprimir
    FILE* fOriginal = fopen(formatString(fName), "r+b");

    // registrar char por char el archivo huffman
    uchar c = read<uchar>(fOriginal);
    while (!feof(fOriginal))
    {
        int idx = c;
        string hufCode = table[idx].code;
        for (int i = 0; i < length(hufCode); i++)
        {
            char bit = hufCode[i];
            bitWriterWrite(hufBW, charToInt(bit));
        }
        c = read<uchar>(fOriginal);
    }
    bitWriterFlush(hufBW); // completa el ultimo byte antes de cerrar el archivo

    fclose(fOriginal);
    fclose(fHuffman);
}
