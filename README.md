
# Huffman Compressor

## Descripción

Ejercicio Integrador de la asignatura "Algoritmos y Estructura de Datos" de la UTN-FRBA (ISI, Primer Nivel).
</br>
Dicha práctica consiste en el desarrollo de un compresor de archivos de texto implementando el algoritmo de Huffman (Codificación Huffman).

## Documentación

El Algoritmo de Huffman hace uso de un árbol binario para codificar los caracteres del archivo de texto y reducir el espacio ocupado por este.

### Flujo del Proceso de Compresión

* Inicialización de la tabla huffman
* Lectura del archivo a comprimir y conteo de las ocurrencias de los caracteres. (La información será registrada en la tabla a medida que se vaya leyendo el archivo).
* Creación de la Lista de Caracteres. Dicha Lista se creará registrando, por nodo, un caracter ASCII y sus ocurrencias (junto a dos punteros). *Véase la estructura HuffmanTreeInfo*
* Ordenamiento de la lista. El `CharList` se ordenará de manera que el ASCII con mayor ocurrencia precede al que tenga menor ocurrencia. Y, a igual ocurrencia, se guiará por el orden numérico (Tabla ASCII Extendido).
* Conversión de la lista a árbol binario.
* Codificación de los caracteres. Se iterará el árbol huffman y se registrará el código en la tabla (campo string). *La iteración del árbol binario y la lógica del algoritmo se encuentra fuera del alcance del presente proyecto*
* Generar el archivo comprimido. El archivo comprimido contará con la estensión '.huf' y la siguiente extructura:
</br>
[ t (1 byte) ] [ char (1 byte) | longitud del código huffman (1 byte) | código huffman (n bytes) ] [ longitud del archivo original (4 bytes) ] [ texto codificado (m bytes) ].
