#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
EncodeInfo encInfo;
DecodeInfo dncInfo;
OperationType check_operation_type(char *);


int main(int argc, char *argv[])
{   
    // int type = check_operation_type(argv);
    // if(type == e_encode){
    //     if(check_extenshion(argv[2],".bmp")){
    //         printf(".bmp file detected\n");
    //     }
    //     else{
    //         printf("please enter a correct .bmp image file\n");
    //         return 0;
    //     }
    //     if(check_extenshion(argv[3],".txt")){
    //         printf(".txt file detected\n");
    //     }
    //     else{
    //         printf("please enter a correct .txt secret file\n");
    //         return 0;
    //     }
    //     if(argv==4){
    //     }
    //     else if(argv==5){
    //     }
    //     else{
    //     }
    // }
    // else if(type == e_decode){
    // }
    // else{
    //     return 0;
    // }
    
    //step1 -> check_operation_type(argv[1])
    //step2 -> check the return value == e_encode
            //declare structure variable EncodeInfo encInfo
            //--> read_and_validate_encode_args(pass command line arg, &encInfo) == e_success or e_failure
            // e_failure -> print error msg and stop the program
            // e_success -> next step.
        // call do_encoding(&encInfo);
            //e_success: print sucesss message and stop program
            //e_failure -> print error msg and stop the program
    if(check_operation_type(argv[1])==e_encode){
        EncodeInfo encInfo;
        if(read_and_validate_encode_args(argv,&encInfo)==e_failure){
            printf("Error: invalid CLA\n");
            return 0;
        }
        if(do_encoding(&encInfo)==e_failure){
            printf("Error:encoding failed\n");
            return 0;
        }
        
    }
    //step3 -> return value == e_decode
            // --
    else if(check_operation_type(argv[1])==e_decode)
    {
        if(read_and_validate_decode_args(argc,argv,&dncInfo)==d_failure){
            printf("Error : invalid CLA\n");
            return 0;
        }
        if(do_Decoding(&dncInfo)==d_failure){
            printf("Error:decoding failed\n");
            return 0;
        }

    }
    //step3 -> return value == e_unsupported
            // --> print invalid arg
            // -e -> encode
            // -d  -> decode
    else if(check_operation_type(argv[1])==e_unsupported){
        printf("Enter a valid symbol\n1: -e - encoding\n2: -d - decoading\n");
    }
}

OperationType check_operation_type(char *symbol)
{
    //step1 -> check it is -e or -d 
    // if it is -e return e_encode
    // else if it is -d rteun e_decode
    // else return e_unsuported
    if(!strcmp(symbol,"-e")){
        return e_encode;
    }
    else if(!strcmp(symbol,"-d")){
        return e_decode;
    }
    else{
        return e_unsupported;
    }
}
