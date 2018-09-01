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

int NotExpanded[16] = {
     2,  3,  6,  7, 10 ,11, 14, 15,
    18, 19, 22, 23, 26, 27, 30, 31
};

int main()
{
	int tab[96][64];
	FILE * file;
	file = fopen("M1","w");
	for(int i=0;i<96;i++)
	{
		for(int j=0;j<64;j++)
		{
			tab[i][j]=0;
		}
	}
	for(int i=0;i<64;i++)
	{
		tab[i][PermutationInitial[i]-1]=1;
	}
	int tmp [32][64];
    for(int i=32;i<64;i++)
	{
		for(int j=0;j<64;j++)
		{
			tmp[i-32][j]=tab[i][j];
		}
	}	
    for(int i=32;i<80;i++)
	{
		for(int j=0;j<64;j++)
		{
			tab[i][j]=tmp[DesExpansion[i-32]-1][j];
		}
	}
	for(int i=80;i<96;i++)
	{
		for(int j=0;j<64;j++)
		{
			tab[i][j]=tmp[NotExpanded[i-80]-1][j];
		}
	}	
	if(file!=NULL)
    {
	    for(int i=0;i<96;i++)
		{
			for(int j=0;j<64;j++)
			{
				fprintf(file,"%d",tab[i][j]);
			}
			fprintf(file,"\n");
		}
    }
	return EXIT_SUCCESS;
}
