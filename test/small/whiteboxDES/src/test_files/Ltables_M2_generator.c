#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int Pbox[32] = {
    16,  7, 20, 21, 29, 12, 28, 17,
    1, 15, 23, 26,  5, 18, 31, 10,
    2,  8, 24, 14, 32, 27,  3,  9,
    19, 13, 30,  6, 22, 11,  4, 25
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
    // vars
    int M2_permutation[96][96];
    int M2_expansion[96][96];
    int M2[96][96];

    // init zeros
    memset(M2_permutation, 0, sizeof(M2_permutation[96][96]) * 96 * 96);
    memset(M2_expansion, 0, sizeof(M2_expansion[96][96]) * 96 * 96);
    memset(M2, 0, sizeof(M2[96][96]) * 96 * 96);

    // permutation
    for(int ii = 0; ii < 32; ii++)
    {
	M2_permutation[ii + 32][Pbox[ii] + 32 - 1] = 1;
    }

    // xor
    for(int ii = 0; ii < 32; ii++)
    {
	M2_permutation[ii + 32][ii] = 1;
    }

    // R becomes L
    for(int ii = 0; ii < 32; ii++)
    {
	M2_permutation[ii][ii + 64] = 1;
    }

    // expansion identities
    for(int ii = 0; ii < 32; ii++)
    {
	M2_expansion[ii][ii] = 1;
    }

    // expansion
    for(int ii = 32; ii < 80; ii++)
    {
	M2_expansion[ii][DesExpansion[ii - 32] + 32 - 1] = 1;
    }

    // Not Duplicated bits
    for(int ii = 0; ii < 16; ii++)
    {
	M2_expansion[ii + 80][NotDuplicated[ii] + 32 - 1] = 1;
    }
    
    // M2 = M2_expansion*M2_permutation
    int temp = 0;

    for(int ii = 0; ii < 96; ii++)
    {
	for(int jj = 0; jj < 96; jj++)
	{
	    //    char M1[96][64];
//    char M1_permutation[96][64];
	    //  char M1_expansion[96][96];

	    // M1[ii][jj] = M1_expansion[ii][0 -> 96] *
	    //              M1_permutation[0 -> 96][jj]
	    for(int kk = 0; kk < 96; kk++)
	    {
		if(M2_expansion[ii][kk] == 1 && 
		    M2_permutation[kk][jj] == 1)
		   temp ++;
	    }

	    if(temp % 2 == 1){
		M2[ii][jj] = 1;
		temp = 0;
	    }
	}
    }

    // vars
    bool bit_test = 0;
    unsigned int subvector = 0; //4bits!

    // OPEN LTABLE FILE
    FILE* file = fopen("tboxes.c", "w");

    fprintf(stdout, "int M2_Ltables[12][24][256] = {\n");

    // lines of submatrices
    for(int ii = 0; ii < 24; ii++)
    {
	fprintf(stdout, "{\n");
	// cols of submatrices
	for(int jj = 0; jj < 12; jj++)
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
			if(M2[ii * 4 + byte_i][jj * 8 + byte_j] == 1 &&
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
