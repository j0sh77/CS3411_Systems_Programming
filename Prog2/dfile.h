# ifndef DFILE_H
# define DFILE_H
#include <sys/types.h>
/*
 * Authors: Riley Hirn and Josh Johnson
 *
*/
typedef struct {
	int numNodes;
	int numNodesAv;
	off_t* nodes;
	int* sizes;
	int fileD;
} DFile;

DFile* dopen(char*, int);
off_t dalloc(DFile*, int);
int dfree(DFile*, off_t);
int dclose(DFile*);
void print(DFile*, off_t);

# endif
