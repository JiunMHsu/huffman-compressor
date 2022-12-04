
#ifndef _MAIN
#define _MAIN

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

#include "compress.hpp"
#include "decompress.hpp"

using namespace std;

void compress(string fName)
{
	// count frequency
	HuffmanTable table[256];
	tableInit(table); // ver si es necesario
	countOccurrences(fName, table);

	// create a list of all chars
	List<HuffmanTreeInfo *> charList = list<HuffmanTreeInfo *>();
	createCharList(charList, table);

	// convert list to tree
	HuffmanTreeInfo *root = createHuffmanTree(charList);

	// writing down the huffman code
	encode(root, table);

	generateCompressedFile(fName, table);
}

void decompress(string fName)
{
	// getting information for the hf tree
	string table[256]; // check default value (if it's "")
	buildTable(fName, table);

	// restoring the huffman tree
	HuffmanTreeInfo *root = restoreHuffmanTree(table);

	generateOriginalFile(fName, root);
}

int main(/* int argc, char **argv */)
{
	// string fName = "a.txt";
	string fName = "a.txt.huf";

	if (!endsWith(fName, ".huf"))
	{
		compress(fName);
	}
	else
	{
		decompress(fName);
	}

	return 0;
}

#endif
