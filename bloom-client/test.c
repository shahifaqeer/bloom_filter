#include<stdio.h>
#include<string.h>
#include<time.h>

#include"bloom.h"
#include"hash-config.h"

int main(int argc, char *argv[])
{
    FILE *fp;
    /* No domain more than 1024 characs */
    char line[1024];
    char *p;
    BLOOM *bloom;

    if(argc<2) {
        fprintf(stderr, "ERROR: No bloom filter file specified\n");
        return EXIT_FAILURE;
    }
    if(bloom_download()) {
        printf("Could not download filter. Use current filter.bin file.\n");
    }
    if(!(fp=fopen(argv[1], "r"))) {
        fprintf(stderr, "ERROR: Could not open filter: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    if(!(bloom=bloom_create(fp, BLOOMSIZE, NUMHASH, sax_hash, sdbm_hash))) {
        fprintf(stderr, "ERROR: Could not create bloom filter\n");
        return EXIT_FAILURE;
    }
    fclose(fp);

    printf("Enter domains to check.\n");

    /* check block - enter name to check */
    while(fgets(line, 1024, stdin)) {
        if((p=strchr(line, '\r'))) *p='\0';
        if((p=strchr(line, '\n'))) *p='\0';

        p=strtok(line, " \t,;\r\n!");
        while(p) {
            if(!bloom_check(bloom, p)) {
                printf("No match for word \"%s\"\n", p);
            }
            p=strtok(NULL, " \t,.;:\r\n?!-/()");
        }
    }

    bloom_destroy(bloom);
    return EXIT_SUCCESS;
}
