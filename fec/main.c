#include <stdio.h>
#include <inttypes.h>

#include "gf_complete.h"
#include "gf_rand.h"

gf_t g_gf;

uint8_t vandermonde_matrix[8][3] = { {1, 0, 0},
                                   {0, 1, 0},
                                   {0, 0, 1},
                                   {1, 1, 6},
                                   {4, 3, 2},
                                   {5, 2, 2},
                                   {5, 3, 4},
                                   {4, 2, 4}};
uint8_t g_encoding_window_len = 3;

int32_t
encode_symbols (uint8_t *input_symbols, uint32_t num_input_symbols,
                uint8_t *const encoded_symbols, uint32_t *num_output_symbols)
{
    uint32_t num_encoded_symbols = 0;
    uint32_t coeff_row_idx = 0, i = 0, j = 0, sum = 0, product = 0;
    
    for (i = 0; i < num_input_symbols; i++) {
        /* the non repair symbols are simply transmistted as is  */
        if (coeff_row_idx < g_encoding_window_len) {
            encoded_symbols[num_encoded_symbols] = input_symbols[i];
            printf("encoding non repair symbol_no=%d, src=%d, encoded=%d\n",
                   i, input_symbols[i], encoded_symbols[num_encoded_symbols]);
            coeff_row_idx++;
            coeff_row_idx = coeff_row_idx % 8;
            num_encoded_symbols++;
            continue;
        }

        /* construct repair symbols */
        sum = 0;
        for (j = 0; j < 3; j++) {
            product = g_gf.multiply.w32(&g_gf,
                                        input_symbols[i],
                                        vandermonde_matrix[coeff_row_idx][j]);
            sum = sum ^ product;
        }
        encoded_symbols[num_encoded_symbols] = sum;
        printf("encoding repair symbol_no=%d, src=%d, encoded=%d\n",
               i, input_symbols[i], encoded_symbols[num_encoded_symbols]);
        num_encoded_symbols++;
        coeff_row_idx++;
        coeff_row_idx = coeff_row_idx % 8;
        
        /* reset sum */
        sum = 0;
    }

    *num_output_symbols = num_encoded_symbols;
    return 0;
}

int
main (int argc, char *argv)
{

    uint32_t num_encoded_symbols = 0;
    const uint32_t num_src_symbols = 10;
    
    uint8_t src_symbols[10] = {15,  33,  45,  88, 92,
                               100, 101, 211, 5,  77};
    
    uint8_t encoded_symbols[10] = {0};

    /* initialise a GF(2^8) */
    gf_init_easy(&g_gf, 8);

    encode_symbols(src_symbols, 7, encoded_symbols, &num_encoded_symbols);
    
    
}
//gcc -g main.c -lgf_complete -o fec
