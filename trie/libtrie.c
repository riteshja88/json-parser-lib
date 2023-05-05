#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "libtrie.h"

/*--Create new trie node--*/
int trie_new(trie_t ** const trie)
{
    *trie = calloc(1,
				   sizeof(trie_t));
    if (NULL == *trie) {
        // memory allocation failed
        return -1;
    }
    return 0;
}

/*--Insert new word to Trie--*/
int trie_insert(trie_t * const trie,
				const char * const word,
				const unsigned int word_len,
				const void * const obj)
{
    int ret = 0;

    // this is the end of this word; add an end-of-word marker here and we're
    // done.
    if (0 == word_len) {
        trie->end_of_word = true;
		trie->obj = obj;
        return 0;
    }

    const unsigned int index = word[0];

    // this index is outside the alphabet size; indexing this would mean an
    // out-of-bound memory access (bad!). If you introduce a separate map
    // function for indexing, then you could move the out-of-bounds index in
    // there.
    if (ALPHABET_SIZE <= index) {
        return -1;
    }

    // The index does not exist yet, allocate it.
    if (NULL == trie->children[index]) {
        ret = trie_new(&trie->children[index]);
        if (-1 == ret) {
            // creating new trie node failed
            return -1;
        }
    }
    
    // recurse into the child node
    return trie_insert(
        /* trie = */ trie->children[index],
        /* word = */ word + 1,
        /* word_len = */ word_len - 1,
		/* pass-as-is */ obj
    );
}


/*--Search a word in the Trie--*/
int trie_search(const trie_t * const trie,
				const char * const word,
				const unsigned int word_len,
				const trie_t ** const result,
				const void ** const obj)
				
{
    // we found a match
    if (0 == word_len) {
		//assert(true == trie->end_of_word);
		*result = trie;
        *obj = trie->obj;
        return 0;
    }

    const unsigned int index = word[0];

    // This word contains letters outside the alphabet length; it's invalid.
    // Remember to do this to prevent buffer overflows.
    if (ALPHABET_SIZE <= index) {
        return -1;
    }

    // No match
    if (NULL == trie->children[index]) {
        return -1;
    }

    // traverse the trie
    return trie_search(/* trie = */ trie->children[index],
					   /* word = */ word + 1,
					   /* word_len = */ word_len - 1,
					   /* result = */ result,
					   /* obj = */ obj);
}

/*---Return all the related words------*/
void trie_print(const trie_t * const trie,
				char prefix[],
				const unsigned int prefix_len)
{

    // An end-of-word marker means that this is a complete word, print it.
    if (true == trie->end_of_word) {
        printf("%.*s\n", prefix_len, prefix);
    }

    // However, there can be longer words with the same prefix; traverse into
    // those as well.
    for (int i = 0; i < ALPHABET_SIZE; i++) {

        // No words on this character
        if (NULL == trie->children[i]) {
            continue;
        }

        prefix[prefix_len] = i;

        // traverse the print into the child
        trie_print(trie->children[i], prefix, prefix_len + 1);
    }
}
