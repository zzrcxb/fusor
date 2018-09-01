#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <getopt.h> 

#include "DES.h"
#include "WBDES.h"

//////////////////////////////////////////////////////
//                 GLOBAL VARIABLES                //
////////////////////////////////////////////////////

static FILE * output = NULL;

//////////////////////////////////////////////////////
//                 FUNCTIONS                       //
////////////////////////////////////////////////////

// usage 
static void usage(int status)
{
    if(status == EXIT_SUCCESS)
    {
        fprintf(stdout,"Usage: whitebox [OPTION] FILE\n"
                "Descrypt with Whitebox-DES.\n\n"
                " -o, --output=FILE write result to FILE\n"
                " -h, --help        display this help\n");
    }
    else
    {
        fprintf(stderr, "Try 'desbox --help' for more information.\n");
    }
    exit(status);
}

int main(int argc, char ** argv)
{
    // vars
    FILE * input = NULL;

    //////////////////////////////////////////////////////
    //                 OPTION PARSER                   //
    ////////////////////////////////////////////////////

    int optc = 0;

    const char* short_opts = "hk:o:";

    const struct option long_opts[] = 
        { 
            {"help",           no_argument, NULL, 'h'},
            {"output",   required_argument, NULL, 'o'},
            {NULL,                       0, NULL,   0}
        }; 

    while((optc = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1)
    { 
        switch(optc)
        { 
        case 'h': // help
            usage(EXIT_SUCCESS);
            break;

        case 'o': // output file
            output = fopen(optarg, "w");
            if(output == NULL)
            {
                fprintf(stderr,
                        "Error: don't have permission to write output file");
                exit(EXIT_FAILURE);
            }
            break;

        default : // no arguments
            usage(EXIT_FAILURE); 
        }
    }

    //////////////////////////////////////////////////////
    //                CHECK ARGUMENTS                  //
    ////////////////////////////////////////////////////

    // Check if there is a input file passed as argument
    if(argv[optind] == NULL)
    {
        fprintf(stderr, "Error: Missing input file argument\n");
        usage(EXIT_FAILURE);
    }

    // Check if we can open the input file
    input = fopen(argv[optind], "rb");

    if(input == NULL)
    {
        fprintf(stderr, "Error: can't open input file\n");
        usage(EXIT_FAILURE);
    }

    // default output file if none is specified
    if(output == NULL) 
        output = fopen("output.txt", "w");

    // check if we have write rights
    if(output == NULL)
    {
        fprintf(stderr, "Error: don't have permission to write output file\n");
        exit(EXIT_FAILURE);
    }

    //////////////////////////////////////////////////////
    //                      APP                        //
    ////////////////////////////////////////////////////

    //
    // 16 Rounds of encryption
    //

    size_t amount; // used for fwrite
    uint64_t data;
    uint64_t temp_data;
    
    unsigned int init_input[8];
    unsigned int temp[12];
    unsigned int rounds_output[12];
    unsigned int final_output[8];
    
    while((amount = fread(&data, 1, 8, input)) > 0)
    {
        if(amount != 8)
        {
            data = data << (8 * (8 - amount));
        }
        
	// state1 -> state2 Ltables + M2
        for(int ii = 0; ii < 16; ii++)
            init_input[ii] = (data >> (56 - 8*ii)) & 255;

        //  
        // Apply DES to decrypt
	//

	// M1
        before_rounds(init_input, temp);
        
        for(int ii = 0; ii < 16; ii++)
        {
            rounds(temp, rounds_output, ii);
            for(int jj = 0; jj < 12; jj++)
                temp[jj] = rounds_output[jj];
        }
        
	// M3
        end_rounds(rounds_output, final_output);
        
        // vvvvv CHECK THIS PART vvvvvv
        
        // Copy array in an uint64_t
        for(int ii = 0; ii < 8; ii++)
        {
            data = 0;
            temp_data = init_input[ii];
            temp_data = temp_data << (56 - 8*ii);
            data += temp_data;
        }
        
        if(amount != 8)
        {
            data = data << (8 * (8 - amount));
        }

        // write output
        fwrite(&data, 1, amount, output);
        data = 0;
    }

    fclose(input);
    fclose(output);

    //
    return EXIT_SUCCESS;
}
