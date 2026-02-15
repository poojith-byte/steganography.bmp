#ifndef DECODE_H
#define DECODE_H
#include<stdio.h>

#include "types.h"

typedef struct _DecodeInfo
{   char *src_image_name;
    FILE *stego_image_pointer;
    int stego_extenshion_size;
    int stego_data_size;
    char stego_extenshion[20];
    char stego_secret_data[100];
    char output[20];
    FILE *file_pointer_out;
}DecodeInfo;

/*Decoding function prototype*/

Dstatus do_Decoding(DecodeInfo *decodeInfo);
Dstatus read_and_validate_decode_args(int argc,char *argv[],DecodeInfo *decodeInfo);
#endif