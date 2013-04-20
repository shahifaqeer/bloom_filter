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
    char* filepath;
    char filepath2[20] = "";

    if(argc<2) {
        fprintf(stderr, "ERROR: No word file specified\n");
        return EXIT_FAILURE;
    }

    if(!(bloom=bloom_create(BLOOMSIZE, NUMHASH, sax_hash, sdbm_hash))) {
        fprintf(stderr, "ERROR: Could not create bloom filter\n");
        return EXIT_FAILURE;
    }

    if(!(fp=fopen(argv[1], "r"))) {
        fprintf(stderr, "ERROR: Could not open file %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    /* set block - read names from file argv[1] and add to filter */
    while(fgets(line, 1024, fp)) {
        if((p=strchr(line, '\r'))) *p='\0';
        if((p=strchr(line, '\n'))) *p='\0';

        bloom_add(bloom, line);
    }

    fclose(fp);

    /* TODO Dump twice - filter.bin and filter_version.bin */
    filepath = "bloomfilter/filter.bin";
    if (!bloom_dump(bloom, filepath)) {
        fprintf(stderr, "ERROR: Could not dump bloom filter %s\n", filepath);
    }

    sprintf(filepath2, "%s%d.bin", "bloomfilter/filter_", (int)time(NULL));
    if (!bloom_dump(bloom, filepath2)) {
        fprintf(stderr, "ERROR: Could not dump bloom filter %s\n", filepath2);
    }

    /* check block - enter name to check */
    while(fgets(line, 1024, stdin)) {
        if((p=strchr(line, '\r'))) *p='\0';
        if((p=strchr(line, '\n'))) *p='\0';

        /* Domain name can have ".", "/", ":" and "-"
         * p=strtok(line, " \t,.;:\r\n?!-/()");
         * Divide line on symbols to check each word*/
        p=strtok(line, " \t,;\r\n?!()");
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
