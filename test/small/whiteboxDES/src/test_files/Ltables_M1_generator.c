#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

    // vars
    bool bit_test = 0;
    unsigned int subvector = 0; //4bits!

    // OPEN LTABLE FILE
    FILE* file = fopen("M1_Ltables.c", "w");

    fprintf(stdout, "int M1_Ltables[8][24][256] = {\n");

    // lines of submatrices
    for(int ii = 0; ii < 24; ii++)
    {
	fprintf(stdout, "{\n");
	// cols of submatrices
	for(int jj = 0; jj < 8; jj++)
	{
	    fprintf(stdout, "{\n");
	    // possible value for the byte
	    for(unsigned int byte = 0; byte < 256; byte++) //8bits!
	    {
		// now let's do the submatrix * subvector
		subvector = 0;
		// lines of submatrix
		for(int byte_i = 0; byte_i < 4; byte_i++)
		{
		    // cols of submatrix
		    for(int byte_j = 0; byte_j < 8; byte_j++)
		    {
			if(M1[ii * 4 + byte_i][jj * 8 + byte_j] == 1 &&
			   (((byte >> (7 - byte_j)) & 1) == 1))
			    bit_test = bit_test == 1 ? 0 : 1;
		    }
		    if(bit_test == 1)
		    {
			subvector += (1 << (3 - byte_i));
			bit_test = 0;
		    }
		}
		fprintf(stdout, "%i, ", subvector);
	    }
	    fprintf(stdout, "}\n");
	}
	fprintf(stdout, "}\n");
    }
    
    // CLOSE LTABLE FILE
    fprintf(stdout, "}\n");

    fclose(file);

    //
    return EXIT_SUCCESS;
}
