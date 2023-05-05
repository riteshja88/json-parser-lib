#define ALPHABET_SIZE 256

/*--Node in the Trie--*/
typedef struct trie_t {
    struct trie_t *children[ALPHABET_SIZE];
    bool end_of_word;
	const void *obj; /* NULL if end_of_word == false */
} trie_t;

int trie_new(trie_t ** const trie);

int trie_insert(trie_t * const trie,
				const char * const word,
				const unsigned int word_len,
				const void * const obj);

int trie_search(const trie_t * const trie,
				const char * const word,
				const unsigned int word_len,
				const trie_t ** const result,
				const void ** const obj);

void trie_print(const trie_t * const trie,
				char prefix[],
				const unsigned int prefix_len);
