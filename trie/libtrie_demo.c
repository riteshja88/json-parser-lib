#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libtrie.h"

/*------Demonstrate purposes uses text file called dictionary -------*/

int main()
{
    int ret = 0;
    trie_t * root = NULL;
    const trie_t * trie = NULL;
    char word[100] = {0};

    // Create a root trie
    ret = trie_new(&root);
    if (-1 == ret) {
        fprintf(stderr,
				"Could not create trie\n");
        exit(1);
    }

    // open the dictionary file
    FILE *fp = fopen("dictionary.txt",
					 "r");
    if (NULL == fp) {
        fprintf(stderr,
				"Error while opening dictionary file");
        exit(1);
    }

    // insert all the words from the dictionary
    while (1 == fscanf(fp,
					   "%s\n",
					   word)) {
        ret = trie_insert(root,
						  word,
						  strlen(word),
						  NULL);
        if (-1 == ret) {
            fprintf(stderr,
					"Could not insert word into trie\n");
            exit(1);
        }
    }

    while (1) {
        printf("Enter keyword: ");
        if (1 != scanf("%s",
					   word)) {
            break;
        }

        printf("\n==========================================================\n");
        printf("\n********************* Possible Words ********************\n");

		const void *obj = NULL;
        ret = trie_search(root,
						  word,
						  strlen(word),
						  &trie,
						  &obj);
        if (-1 == ret) {
            printf("No results\n");
            continue;
        }

        trie_print(trie, word, strlen(word));

        printf("\n==========================================================\n");
    }
}
