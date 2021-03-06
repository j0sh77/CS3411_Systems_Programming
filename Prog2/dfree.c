#include <unistd.h>
#include "dfile.h"
#include <sys/types.h>
#include <stdlib.h>
/**
 * Authors: Riley Hirn and Josh Johnson
 *
 * Free space in an open file.
 *
 * @param df A pointer to the DFile structure for the file.
 * @ param freeptr A file pointer to the free block (the data part).
 *
 * @return -1 if an error, 0 otherwise
 */
int dfree(DFile* df, off_t freeptr) {
	int i;
	int err;
	int fd = df->fileD;
	int extra = sizeof(int) + sizeof(off_t);
	int size;
	int nul = 0;
	off_t hold;


	df->numNodes++;



	if(df->numNodes == df->numNodesAv - 1) {
		df->numNodesAv *= 2;
		//df->sizes = (int*)realloc(df->sizes, sizeof(int) * df->numNodesAv);
		df->nodes = (off_t*)realloc(df->nodes, sizeof(int) * df->numNodesAv);
		if(df->sizes == NULL || df->nodes == NULL) return -1;
		for(i = (df->numNodes); i < df->numNodesAv; i++)
			df->sizes[i] = -1;
	}


	err = lseek(fd, freeptr - sizeof(int), SEEK_SET);//size of free 
	if(err == -1)	return -1;

	//read size of freed node
	err = read(fd, &size, sizeof(int));
	if(err == -1)	return -1;

	size *= -1;


	//find the position to add the freed node (insert at i+1)
	//move smaller nodes
	for(i = (df->numNodes) - 2; i >= 0; i--) {
		if(df->sizes[i] < size) {
			df->nodes[i+1] = df->nodes[i];
			df->sizes[i+1] = df->sizes[i];
		} else {
			break;
		}
	}


	df->nodes[i + 1] = freeptr - extra;
	df->sizes[i + 1] = size;

//print(df, 0);
//printf("i: %d\n", i);
	//write next pointer of previous node
	if(i + 1 != 0) {
		err = lseek(fd, df->nodes[i] + extra + df->sizes[i] + sizeof(int), SEEK_SET);
		if(err == -1)	return -1;

		err = write(fd, &(df->nodes[i + 1]), sizeof(off_t));
		if(err == -1)	return -1;
	} else {

		err = lseek(fd, 0, SEEK_SET);
		if(err == -1)	return -1;

		err = write(fd, &(df->nodes[i+1]), sizeof(off_t));
		if(err == -1)	return -1;
	}

	//write prev pointer for freed node
	err = lseek(fd, df->nodes[i + 1], SEEK_SET);
	if(err == -1)	return -1;

 	if(i + 1 == 0) {
		err = write(fd, &nul, sizeof(off_t));
		if(err == -1)	return -1;

	} else {
		err = write(fd, &(df->nodes[i]), sizeof(off_t));
		if(err == -1)	return -1;
	}


	err = write(fd, &size, sizeof(int));
	if(err == -1)	return -1;

	err = lseek(fd, size, SEEK_CUR);
	if(err == -1)	return -1;


	//write second size for freed node
	err = write(fd, &size, sizeof(int));
	if(err == -1)	return -1;

	//write next pointer of freed node
	if(i + 1 == df->numNodes) {


		err = write(fd, &nul, sizeof(off_t));
		if(err == -1)	return -1;
	} else {


		err = write(fd, &(df->nodes[i + 2]), sizeof(off_t));
		if(err == -1)	return -1;
	}


	//write prev pointer of next node
	if(i + 2 < df->numNodes) {
		err = lseek(fd, df->nodes[i + 2], SEEK_SET);
		if(err == -1)	return -1;
		err = write(fd, &(df->nodes[i+1]), sizeof(off_t));
		if(err == -1)	return -1;
	} 

	return 0;
}
