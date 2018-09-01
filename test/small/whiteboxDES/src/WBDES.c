#include "WBDES.h"
#include "tboxes.c"
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// 64 bits input -> 96 bits output (M1)
void before_rounds(unsigned int *in, unsigned int *out)
{
    unsigned int temp1[24][8];
    unsigned int temp2[24];
    
    // Fill temp2[24] with 0
    for(int ii = 0; ii < 24; ii++)
    {
        temp2[ii] = 0;
    }        
    
    // Sub-matrices
    for(int ii = 0; ii < 24; ii++)
    {
        for(int jj = 0; jj < 8; jj++)
        {
            temp1[ii][jj] = M1_Ltables[ii][jj][in[jj]];
        }
    }
    
    // Xor part
    for(int ii = 0; ii < 24; ii++)
    {
        for(int jj = 0; jj < 8; jj++)
        {
            temp2[ii] = xorTables[((temp2[ii] << 4) + temp1[ii][jj])];
        }
    }
    
    // Fill the output
    for(int ii = 0; ii < 12; ii++)
    {
        out[ii] = (temp2[2*ii] << 4) + temp2[2*ii +1];
    }
}



// 96 bits input -> 96 bits output (Tr, M2)
void rounds(unsigned int *in, unsigned int *out, int round)
{
    //
    // 1. Non-linear part
    //
    
    unsigned int temp[12];
    
    for(int ii = 0; ii < 8; ii++)
    {
        temp[ii] = NonLinearTBoxes[round][ii][in[ii]];
    }
    for(int ii = 0; ii < 4; ii++)
    {
        temp[ii+8] = NonLinearTBoxes[round][ii][in[ii+8]];
    }
    
    //
    // 2. AT part
    //
    
    unsigned int temp1[24][12];
    unsigned int temp2[24];
    
    // Fill temp2[24] with 0
    for(int ii = 0; ii < 24; ii++)
    {
        temp2[ii] = 0;
    }
    
    // Sub-matrices
    for(int ii = 0; ii < 24; ii++)
    {
        for(int jj = 0; jj < 12; jj++)
        {
            temp1[ii][jj] = M1_Ltables[ii][jj][temp[jj]];
        }
    }
    
    // Xor part
    for(int ii = 0; ii < 24; ii++)
    {
        for(int jj = 0; jj < 12; jj++)
        {
            temp2[ii] = xorTables[((temp2[ii] << 4) + temp1[ii][jj])];
        }
    }
    
    // Fill the output
    for(int ii = 0; ii < 12; ii++)
    {
        out[ii] = (temp2[2*ii] << 4) + temp2[2*ii +1];
    }
}



// 96 bits input -> 64 bits output (M3)
void end_rounds(unsigned int *in, unsigned int *out)
{
    unsigned int temp1[16][12];
    unsigned int temp2[16];
    
    // Fill temp2[16] with 0
    for(int ii = 0; ii < 16; ii++)
    {
        temp2[ii] = 0;
    }
    
    // Sub-matrices
    for(int ii = 0; ii < 16; ii++)
    {
        for(int jj = 0; jj < 12; jj++)
        {
            temp1[ii][jj] = M1_Ltables[ii][jj][in[jj]];
        }
    }
    
    // Xor part
    for(int ii = 0; ii < 16; ii++)
    {
        for(int jj = 0; jj < 12; jj++)
        {
            temp2[ii] = xorTables[((temp2[ii] << 4) + temp1[ii][jj])];
        }
    }
    
    // Fill the output
    for(int ii = 0; ii < 8; ii++)
    {
        out[ii] = (temp2[2*ii] << 4) + temp2[2*ii +1];
    }
}
