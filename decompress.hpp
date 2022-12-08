
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

// returns the size in bytes ocuped by the huffman code
// int getSize(int t)
// {
//     int size = 0;
//     while (size < t / 2)
//     {
//         size += 8; // counted in bits
//     }
//     return size / 8; // returns byte
// }

void buildTable(string fName, string table[])
{
    FILE *f = fopen(formatString(fName), "r+b");

    int t = read<uchar>(f);
    int infoSize = 2; // + getSize(t);
    for (int i = 0; i < t; i++)
    {
        BitReader fBr = bitReader(f);
        seek<uchar>(f, (i * infoSize) + 1);
        uchar c = read<uchar>(f); // first byte
        int len = read<uchar>(f); // second byte

        // reading huf code
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

// restore huffman tree and setting leafs
HuffmanTreeInfo *restoreHuffmanTree(string table[])
{
    // create first node (root)
    HuffmanTreeInfo *root = new HuffmanTreeInfo();
    root = huffmanTreeInfo(256, 0, NULL, NULL);

    // i = ASCII code
    for (int i = 0; i < 256; i++)
    {
        if (table[i] != "")
        {
            string code = table[i];
            HuffmanTreeInfo *aux = root;

            for (int n = 0; n < length(code); n++)
            {
                if (code[n] == '0') // code[n] == 0
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
        }
    }

    return root;
}

// cambio tipo de dato a uchar
void restoreFile(string fName, HuffmanTreeInfo *root)
{
    // create empty original file
    string restoredName = substring(fName, 0, lastIndexOf(fName, '.'));
    FILE *fRestored = fopen(formatString(restoredName), "w+b");

    // reading compressed file and setting bit reader
    FILE *fHuffman = fopen(formatString(fName), "r+b");
    BitReader hufBr = bitReader(fHuffman);

    // moving where the content beggins
    int t = read<uchar>(fHuffman);
    // t times info structure (3 bytes): { char, huf code length, huf code }
    // seek<uchar>(fHuffman, (t * (2 + getSize(t))) + 1); // plus 1 (the fisrt byte)

    while (!feof(fHuffman))
    {
        HuffmanTreeInfo *aux = root;
        int bit;

        // navigate the tree according to the obtained bit
        // getting NULL in both next-pointers means it's a leaf (node that contains a char)
        while (aux->left != NULL || aux->right != NULL)
        {
            bit = bitReaderRead(hufBr);
            aux = (bit == 0) ? aux->left : aux->right;
        }

        cout << (char)aux->c << endl;

        // conditional to prevent the last incomplete byte
        if (aux->c < 256)
        {
            write<char>(fRestored, aux->c);
        }
    }

    fclose(fRestored);
    fclose(fHuffman);
}
