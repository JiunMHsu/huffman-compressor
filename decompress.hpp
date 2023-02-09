
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

// retorna el tamanio en bytes, ocupado por el huffman code
int getSize(int t)
{
    int size = 0;
    while (size < t / 2)
    {
        size += 8; // cuenta en bits
    }
    return size / 8; // retorna bytes
}

void buildTable(string fName, string table[])
{
    FILE* f = fopen(formatString(fName), "r+b");

    int t = read<uchar>(f);
    int infoSize = 2 + getSize(t);
    for (int i = 0; i < t; i++)
    {
        // Se inicializa un reader para cada registro
        BitReader fBr = bitReader(f);

        // posicionamiento directo en el comienzo de cada registro
        seek<uchar>(f, (i * infoSize) + 1);

        // lectura de los primeros dos campos
        uchar c = read<uchar>(f); // primer byte (char)
        int len = read<uchar>(f); // segundo byte (longitud del huf code)

        // lectura del huffman code
        string bitStr = "";
        for (int i = 0; i < len; i++)
        {
            int bit = bitReaderRead(fBr);
            bitStr = bitStr + intToString(bit);
        }
        int idx = c;
        table[idx] = bitStr;
    }
    fclose(f);
}

// reconstruir el arbol huffman
HuffmanTreeInfo* restoreHuffmanTree(string table[])
{
    // crear primer nodo (raiz)
    HuffmanTreeInfo* root = new HuffmanTreeInfo();
    root = huffmanTreeInfo(256, 0, NULL, NULL);

    // i = ASCII code
    for (int i = 0; i < 256; i++)
    {
        if (table[i] == "") continue;

        string code = table[i];
        HuffmanTreeInfo* aux = root;

        for (int n = 0; n < length(code); n++)
        {
            if (code[n] == '0')
            {
                // crea el nodo si no esta creado
                aux->left = (aux->left == NULL) ? huffmanTreeInfo(256, 0, NULL, NULL) : aux->left;
                aux = aux->left;
            }
            else // (el caso '1')
            {
                aux->right = (aux->right == NULL) ? huffmanTreeInfo(256, 0, NULL, NULL) : aux->right;
                aux = aux->right;
            }
        }

        aux->c = i; // asignar char a la hoja
    }

    return root;
}

void restoreFile(string fName, HuffmanTreeInfo* root)
{
    // crear archivo 'restaurado' vacio
    string restoredName = "restored-" + substring(fName, 0, lastIndexOf(fName, '.'));
    FILE* fRestored = fopen(formatString(restoredName), "w+b");

    // abrir archivo comprimido e inicializar el bit reader
    FILE* fHuffman = fopen(formatString(fName), "r+b");
    BitReader hufBr = bitReader(fHuffman);

    // posicionar al comienzo del contenido comprimido
    int t = read<uchar>(fHuffman);

    // lectura de la longitud registrada (archivo orginal)
    // t veces el tamanio de los registros, mas 1 que es el primer byte
    seek<uchar>(fHuffman, (t * (2 + getSize(t))) + 1);
    unsigned int originalFLenght = read<unsigned int>(fHuffman);

    while (originalFLenght > 0)
    {
        HuffmanTreeInfo* aux = root;
        int bit;

        // moverse en el arbol segun el bit obtenido
        //
        // encontrarse con NULL en ambos punteros a los hijos
        // significa que es una hoja (nodo el cual contiene un char)
        while (aux->left != NULL || aux->right != NULL)
        {
            bit = bitReaderRead(hufBr);
            if (bit == -1) break; // manejo de final de archivo
            aux = (bit == 0) ? aux->left : aux->right;
        }

        // condicional para evitar el ultimo byte incompleto
        if (aux->c > 0 && aux->c < 256)
        {
            write<uchar>(fRestored, aux->c);
            originalFLenght--;
        }
    }

    fclose(fRestored);
    fclose(fHuffman);
}
