
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

// * FUNCIONES DE DEPURACION * //

void huffmanTreeToDebug(HuffmanTreeInfo *root)
{
	// recorrer el arbol usando TAD HuffmanTree
	HuffmanTree ht = huffmanTree(root);

	string cod;
	while (huffmanTreeHasNext(ht))
	{
		HuffmanTreeInfo *x = huffmanTreeNext(ht, cod);
		cout << x->c << ", (" << x->n << "), "
			  << "[" << cod << "]" << endl;
	}
	cout << endl;
	cout << endl;
}

void huffmanTableToDebug(HuffmanTable table[])
{
	for (int i = 0; i < 256; i++)
	{
		if (table[i].n > 0)
		{
			cout << (char)i << ": " << table[i].code << endl;
			cout << "ocurrencias: " << table[i].n << endl;
			cout << "---------------------------------------" << endl;
		}
	}
}

// ****************** //

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

	// * DEBUGGING
	huffmanTreeToDebug(root);

	// registrar el codigo hufman en la tabla
	encode(root, table);

	// * DEBUGGING
	huffmanTableToDebug(table);

	generateCompressedFile(fName, table);
}

void decompress(string fName)
{
	// getting information for the hf tree
	string table[256]; // check default value (if it's "")
	buildTable(fName, table);

	// restoring the huffman tree
	HuffmanTreeInfo *root = restoreHuffmanTree(table);

	restoreFile(fName, root);
}

int main()
{
	string fName = "text.txt";
	// string fName = "hola.txt.huf";

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
