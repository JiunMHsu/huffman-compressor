
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
	// contar ocurrencias
	HuffmanTable table[256];
	tableInit(table); // ver si es necesario
	countOccurrences(fName, table);

	// crear lista de chars
	List<HuffmanTreeInfo *> charList = list<HuffmanTreeInfo *>();
	createCharList(charList, table);

	// convertir la lista a arbol
	HuffmanTreeInfo *root = createHuffmanTree(charList);

	// registrar el codigo hufman en la tabla
	encode(root, table);

	generateCompressedFile(fName, table);
}

void decompress(string fName)
{
	// leer la informacion del arbol
	string table[256];
	buildTable(fName, table);

	// reconstruir el arbol huffman
	HuffmanTreeInfo *root = restoreHuffmanTree(table);

	restoreFile(fName, root);
}

int main()
{
	// string fName = "text.txt";
	string fName = "text.txt.huf";

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
