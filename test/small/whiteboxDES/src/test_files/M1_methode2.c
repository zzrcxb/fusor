#include <stdio.h>
#include <stdlib.h>

int PermutationInitial[64] = {
    58, 50, 42, 34, 26, 18, 10,  2,
    60, 52, 44, 36, 28, 20, 12,  4,
    62, 54, 46, 38, 30, 22, 14,  6,
    64, 56, 48, 40, 32, 24, 16,  8,
    57, 49, 41, 33, 25, 17,  9,  1,
    59, 51, 43, 35, 27, 19, 11,  3,
    61, 53, 45, 37, 29, 21, 13,  5,
    63, 55, 47, 39, 31, 23, 15,  7
};

int DesExpansion[48] = {
    32,  1,  2,  3,  4,  5,  4,  5,
     6,  7,  8,  9,  8,  9, 10, 11,
    12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21,
    22, 23, 24, 25, 24, 25, 26, 27,
    28, 29, 28, 29, 30, 31, 32,  1
};

int NotDuplicated[16] = {
     2,  3,  6,  7, 10 ,11, 14, 15,
    18, 19, 22, 23, 26, 27, 30, 31
};


int main()
{

    // permutation matrix
    char M1[96][64];
    char M1_permutation[96][64];
    char M1_expansion[96][96];
        
    // init to zeros
    memset(M1_permutation, 0, sizeof(M1_permutation[96][64]) * 96 * 64);
    memset(M1_expansion, 0, sizeof(M1_expansion[96][96]) * 96 * 96);
    memset(M1, 0, sizeof(M1[96][64]) * 96 * 64);

    //
    // permutation matrix
    //

    // permutation
    for(int ii = 0; ii < 64; ii++)
    {
	M1_permutation[ii][PermutationInitial[ii] - 1] = 1;
    }

    //
    // expansion matrix
    //

    // identity
    for(int ii = 0; ii < 32; ii++)
    {
	M1_expansion[ii][ii] = 1;
    }

    // Not Duplicated bits
    for(int ii = 0; ii < 16; ii++)
    {
	M1_expansion[ii + 80][NotDuplicated[ii] + 32 - 1] = 1;
    }

    // expansion
    for(int ii = 32; ii < 80; ii++)
    {
	M1_expansion[ii][DesExpansion[ii - 32] + 32 - 1] = 1;
    }
    
    // M1 = M1_expansion*M1_permutation

    int temp = 0;

    for(int ii = 0; ii < 96; ii++)
    {
	for(int jj = 0; jj < 64; jj++)
	{
	    //    char M1[96][64];
//    char M1_permutation[96][64];
	    //  char M1_expansion[96][96];

	    // M1[ii][jj] = M1_expansion[ii][0 -> 96] *
	    //              M1_permutation[0 -> 96][jj]
	    for(int kk = 0; kk < 96; kk++)
	    {
		if(M1_expansion[ii][kk] == 1 && 
		    M1_permutation[kk][jj] == 1)
		   temp ++;
	    }

	    if(temp % 2 == 1){
		M1[ii][jj] = 1;
		temp = 0;
	    }
	}
    }

    // writing in file
    FILE* file = fopen("M1_methode2.txt", "w");
    if(file == NULL)
	exit(EXIT_FAILURE);

    for(int ii = 0; ii < 96; ii++)
    {
	for(int jj = 0; jj < 64; jj++)
	{
	    fprintf(file, "%d", M1[ii][jj]);
	}
	fprintf(file, "\n");
    }

    fclose(file);

/*
    printf("\n\n permutation matrix \n\n");
    for(int ii = 0; ii < 96; ii++)
    {
	for(int jj = 0; jj < 64; jj++)
	{
	    printf("%d", M1_permutation[ii][jj]);
	}
	printf("\n");
    }
    printf("\n\n expansion matrix \n\n");
    for(int ii = 0; ii < 96; ii++)
    {
	for(int jj = 0; jj < 96; jj++)
	{
	    printf("%d", M1_expansion[ii][jj]);
	}
	printf("\n");
    }
    printf("\n\n final matrix \n\n");
    for(int ii = 0; ii < 96; ii++)
    {
	for(int jj = 0; jj < 64; jj++)
	{
	    printf("%d", M1[ii][jj]);
	}
	printf("\n");
    }
*/

    //
    return EXIT_SUCCESS;
}
