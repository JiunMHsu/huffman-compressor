
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

char *formatFileName(string s)
{
    char fName[length(s) + 1];
    for (int i = 0; i < length(s); i++)
    {
        fName[i] = s[i];
    }
    fName[length(s) + 1] = '\0';
    return fName;
}

// initiate the table array (filling blank spaces)
void tableInit(HuffmanTable table[])
{
    for (int i = 0; i < 256; i++)
    {
        table[i] = {0, ""};
    }
}

void countOccurrences(string fName, HuffmanTable table[])
{
    FILE *f = fopen(formatFileName(fName), "r+b");
    char c = read<char>(f);
    while (!feof(f))
    {
        // increase occurrency
        table[c].n += 1; // ver si funca
        c = read<char>(f);
    }
    fclose(f);
}

// criteria:
// by char frequency min -> max
// for same freq, sort by ASCII min -> max
int cmpHtInfo(HuffmanTreeInfo *a, HuffmanTreeInfo *b) // revisar si funca bien
{
    // int ret = -1;
    // ret = ((a->n > b->n) || (a->n == b->n && a->c > b->c)) ? 1 : ret;
    return ((a->n > b->n) || (a->n == b->n && a->c > b->c)) ? 1 : -1;
}

// add nodes and sort
void createCharList(List<HuffmanTreeInfo *> &charList, HuffmanTable table[])
{
    for (int i = 0; i < 256; i++)
    {
        long freq = table[i].n;
        if (freq > 0)
        {
            // 'i' represents ASCII code
            HuffmanTreeInfo *htInfo = huffmanTreeInfo(i, freq, NULL, NULL);
            listAdd<HuffmanTreeInfo *>(charList, htInfo);
        }
    }
    listSort<HuffmanTreeInfo *>(charList, cmpHtInfo);
}

// converts list to tree and returns the root pointer
HuffmanTreeInfo *createHuffmanTree(List<HuffmanTreeInfo *> &charList)
{
    // at the end of the iteration
    // it should remain only one node in the list
    // which contains the whole huffman tree
    for (int i = 1; listSize(charList) > 1; i++)
    {
        // get the first two info
        HuffmanTreeInfo *infoA = listRemoveFirst<HuffmanTreeInfo *>(charList);
        HuffmanTreeInfo *infoB = listRemoveFirst<HuffmanTreeInfo *>(charList);

        // create new htInfo to point them
        HuffmanTreeInfo *newInfo = huffmanTreeInfo(256 + i, infoA->n + infoB->n, infoA, infoB);

        // insert to the list as a node
        listOrderedInsert<HuffmanTreeInfo *>(charList, newInfo, cmpHtInfo);
    }

    // returns the huffman tree root by removing the last node remaining
    // because the root should point the tree instead of the list
    return listRemoveFirst<HuffmanTreeInfo *>(charList);
}

// iterate the tree and store the huffman code
void encode(HuffmanTreeInfo *root, HuffmanTable table[])
{
    HuffmanTree ht = huffmanTree(root);

    while (huffmanTreeHasNext(ht))
    {
        string hfCode;
        HuffmanTreeInfo *leaf = huffmanTreeNext(ht, hfCode);

        table[leaf->c].code = hfCode;
    }
}

// wouldn't be necessary by using TAD Array
// arraySize<HuffmanTable>(table)
int countLeafs(HuffmanTable table[])
{
    int a;
    for (int i = 0; i < 256; i++)
    {
        a = (table[i].n > 0) ? a + 1 : a;
    }
    return a;
}

void generateCompressedFile(string fName, HuffmanTable table[])
{
    // creating new file
    string hufName = fName + ".huf";
    FILE *fHuffman = fopen(formatFileName(hufName), "w+b");

    // setting bit writer
    BitWriter hufBW = bitWriter(fHuffman);

    // writing tree's info
    // so it can be rebuilt in decompression
    uchar t = (uchar)countLeafs(table);
    write<uchar>(fHuffman, t);

    // char register structure
    // { char, huf code length, huf code }
    // 1 byte each one, 3 bytes in total
    for (int i = 0; i < 256; i++)
    {
        if (table[i].n > 0)
        {
            write<uchar>(fHuffman, (uchar)i); // ASCII

            string hufCode = table[i].code;
            write<uchar>(fHuffman, (uchar)length(hufCode)); // huffman code length

            for (int n = 0; n < length(hufCode); n++) // huffman code
            {
                bitWriterWrite(hufBW, charToInt(hufCode[n]));
            }
            bitWriterFlush(hufBW); // completes the byte, so the hf code can be written
        }
    }

    // reading original file
    FILE *fOriginal = fopen(formatFileName(fName), "r+b");

    // write the huf file char by char
    char c = read<char>(fOriginal);
    while (!feof(fOriginal))
    {
        string hufCode = table[c].code;
        for (int i = 0; i < length(hufCode); i++)
        {
            bitWriterWrite(hufBW, charToInt(hufCode[i]));
        }
        c = read<char>(fOriginal);
    }
    bitWriterFlush(hufBW); // completes the last byte before closing file

    fclose(fOriginal);
    fclose(fHuffman);
}
