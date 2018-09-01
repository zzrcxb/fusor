#include <stdio.h>
#include <stdlib.h>

int main()
{
	int i,j;
	int tab[96][96];
	int Pbox[32] = {
    	16,  7, 20, 21, 29, 12, 28, 17,
    	 1, 15, 23, 26,  5, 18, 31, 10,
    	 2,  8, 24, 14, 32, 27,  3,  9,
    	19, 13, 30,  6, 22, 11,  4, 25
	};
	FILE * file;
	file = fopen("M","w");
	for(i=0;i<32;i++)
	{
		Pbox[i]+=32;
	}
	for(i=0;i<96;i++)
	{
		for(j=0;j<96;j++)
		{
			tab[i][j]=0;
			if( ((i>=0 && i<32) || (i>=64 && i<96)) && i==j)
				tab[i][j]=1; 
		}
	}
	for(i=32;i<64;i++)
	{
		tab[Pbox[i-32]-1][i]=1;
	}
	if(file!=NULL)
        {
		for(i=0;i<96;i++)
		{
			for(j=0;j<96;j++)
			{
				fprintf(file,"%d",tab[i][j]);
			}
			fprintf(file,"\n");
		}
        }
}
