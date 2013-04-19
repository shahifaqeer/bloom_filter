#include<limits.h>
#include<stdarg.h>
#include<stdio.h>
#include<string.h>
#include<time.h>

#include"bloom.h"

#define SETBIT(a, n) (a[n/CHAR_BIT] |= (1<<(n%CHAR_BIT)))
#define GETBIT(a, n) (a[n/CHAR_BIT] & (1<<(n%CHAR_BIT)))


BLOOM *bloom_create(size_t size, size_t nfuncs, ...)
{
	BLOOM *bloom;
	va_list l;
	int n;
	
	if(!(bloom=malloc(sizeof(BLOOM)))) return NULL;
	if(!(bloom->a=calloc((size+CHAR_BIT-1)/CHAR_BIT, sizeof(char)))) {
		free(bloom);
		return NULL;
	}
	if(!(bloom->funcs=(hashfunc_t*)malloc(nfuncs*sizeof(hashfunc_t)))) {
		free(bloom->a);
		free(bloom);
		return NULL;
	}

	va_start(l, nfuncs);
	for(n=0; n<nfuncs; ++n) {
		bloom->funcs[n]=va_arg(l, hashfunc_t);
	}
	va_end(l);

	bloom->nfuncs=nfuncs;
	bloom->asize=size;

	return bloom;
}

int bloom_download(BLOOM* bloom, FILE* fptr)
{
    int res, nread;
    res = system("curl -s -f -z filter.bin -O http://sites.noise.gatech.edu/~sarthak/files/bloomfilter/filter.bin");
    /* printf("Result is %d\n", res); */

    /* Read downloaded bloom filter
     * TODO THIS MAY NOT BE THE RIGHT WAY! */
    nread = fread(bloom,sizeof(*bloom),1,fptr);
    printf("Read %d bytes\n", nread);

    return nread;
}

int bloom_destroy(BLOOM *bloom)
{
	free(bloom->a);
	free(bloom->funcs);
	free(bloom);
    printf("Destroy\n");

	return 0;
}

int bloom_check(BLOOM *bloom, const char *s)
{
	size_t n;

	for(n=0; n<bloom->nfuncs; ++n) {
		if(!(GETBIT(bloom->a, bloom->funcs[n](s)%bloom->asize))) return 0;
	}
    printf("%s exists. return 1\n", s);

	return 1;
}

/*
int bloom_add(BLOOM *bloom, const char *s)
{
	size_t n;
    printf("Add %s at time %d\n",s,(int)time(NULL));

	for(n=0; n<bloom->nfuncs; ++n) {
		SETBIT(bloom->a, bloom->funcs[n](s)%bloom->asize);
	}

	return 0;
}

int bloom_dump(BLOOM *bloom, const char* filepath)
{
    int nwrite;
    FILE* fptr;
    bloom->timestamp = (int)time(NULL);

    fptr = fopen(filepath, "wb");

    nwrite = fwrite(bloom, sizeof(*bloom), 1, fptr);
    printf("Wrote %d bytes",nwrite);
    fclose(fptr);

    return res;
}
*/
