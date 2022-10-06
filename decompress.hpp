
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

void buildTable(string fName, string table[])
{
    FILE *f = fopen(formatString(fName), "r+b");

    int t = read<char>(f); // ver si funca

    for (int i = 0; i < t; i++) // t / t + 1 ??
    {
        char c = read<char>(f);
        int len = read<char>(f); // ver si funca
        uchar code = read<char>(f);

        table[charToInt(c)] = substring(binToString(code), 0, len);
    }

    fclose(f);
}

// restore huffman tree and setting leafs
HuffmanTreeInfo *restoreHuffmanTree(string table[])
{
    // create first node (root)
    HuffmanTreeInfo *root = huffmanTreeInfo(256, 0, NULL, NULL);

    // i = ASCII code
    for (int i = 0; i < 256; i++)
    {
        if (table[i] != "")
        {
            string code = table[i];
            HuffmanTreeInfo *aux = root;

            for (int n = 0; n < length(code); n++)
            {
                if (code[n] == 0)
                {
                    // create if the node isn't created
                    aux->left = (aux->left == NULL) ? huffmanTreeInfo(256, 0, NULL, NULL) : aux->left;
                    aux = aux->left;
                }
                else // (the '1' case)
                {
                    aux->right = (aux->right == NULL) ? huffmanTreeInfo(256, 0, NULL, NULL) : aux->right;
                    aux = aux->right;
                }
            }

            aux->c = i; // setting leaf

            // delete aux ??
        }
    }

    return root;
}

void generateOriginalFile(string fName, HuffmanTreeInfo *root)
{
    // create empty original file
    string originalName = substring(fName, 0, lastIndexOf(fName, '.'));
    FILE *fOriginal = fopen(formatString(originalName), "w+b");

    // reading compressed file and setting bit reader
    FILE *fHuffman = fopen(formatString(fName), "r+b");
    BitReader hufBr = bitReader(fHuffman);

    // moving where the content beggins
    int t = read<char>(fHuffman);
    // t times info structure (3 bytes): { char, huf code length, huf code }
    seek<char>(fHuffman, (t * 3) + 1); // plus 1 (the fisrt byte)

    while (!feof(fHuffman))
    {
        HuffmanTreeInfo *aux = root;
        int bit = bitReaderRead(hufBr);

        // navigate the tree according to the obtained bit
        // getting NULL in both next-pointers means it's a leaf (node that contains a char)
        while (aux->left != NULL || aux->right != NULL)
        {
            aux = (bit == 0) ? aux->left : aux->right;
            bit = bitReaderRead(hufBr);
        }

        // conditional to prevent the last incomplete byte
        if (aux->c < 256)
        {
            write<char>(fOriginal, aux->c);
        }
    }

    fclose(fOriginal);
    fclose(fHuffman);
}