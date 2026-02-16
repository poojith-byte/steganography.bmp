#include<stdio.h>
#include<string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/*
1:Read and validate .bmp
*/
//
char* Decode_character(char buffer[],char *magic){//for character
    //logic to encode data.8times 
    int i=0;
    for(i=0;i<8;i++){
        *magic = *magic|((buffer[i]&1)<<(7-i));
    }
    if(i==8){
        //printf("this is %c\n",*magic);
        return magic;//return the character
        
    }
    else{
        return NULL;
    }
    
    
}
int *decode_integer(char buffer[],int *num)//for intiger
{   //logic to encode data.32times
    int i=0;
    for(i=0;i<32;i++)
    {
        *num = *num|((buffer[i]&1)<<(31-i));
    }
    return num;
}
Dstatus read_and_validate_decode_args(int argc,char *argv[],DecodeInfo *decodeInfo)
{   
    //step1 -> check source file name having .bmp present or not
    // no -> return d_failure
    // yes -> store source file name into decodeInfo->src_image_name
    printf("count - %d\n",argc);
    char *ret = strrchr(argv[2],'.');
    if(ret ==NULL || strcmp(ret,".bmp")!=0) 
    {
        printf(".bmp not present");
        return d_failure;
    }
    decodeInfo->src_image_name = argv[2];
    int i=0;
    //check for another argument if present , store the string before "." and save it to decodeInfo->output 
    // if present save it to decodeInfo->output
    // else save stego to decodeInfo->output
    if(argc==4)
    {
        while(argv[3][i]!='\0'){
            if(argv[3][i]=='.'){
                break;
            }
            decodeInfo->output[i]=argv[3][i];
            i++;
        }
        decodeInfo->output[i]='\0';  
    }
    else{
        strcpy(decodeInfo->output,"stego");
    }

    return d_success;
}

Dstatus Dopen_files(DecodeInfo *decodeInfo )
{   // Stego Image file
    decodeInfo->stego_image_pointer = fopen(decodeInfo->src_image_name,"r");
    // Do Error handling
    if(decodeInfo->stego_image_pointer==NULL){
        printf("Error opning the stego file\n");
        return d_failure;
    }
    return d_success;
};
Dstatus open_outfile(DecodeInfo *decodeInfo)
{   // open output file
    decodeInfo->file_pointer_out =fopen(decodeInfo->output,"w");
    // Do Error handling
    if(decodeInfo->file_pointer_out==NULL){
        printf("Error opning the output file\n");
        return d_failure;
    }
    return d_success;
}
Dstatus Decode_magic_string(FILE *stego_pointer)    
{
    // set the curser to 54 byte
    if(fseek(stego_pointer,54,SEEK_SET)!=0)return d_failure;
     if(fseek(stego_pointer,54,SEEK_SET)!=0)return d_failure;
    char buffer[8];
    unsigned char magic=0;
    int len = strlen(MAGIC_STRING);
    
    for(int i=0;i<len;i++)
    {   
        //read 8bytes from sourse image
        magic=0;
        if(fread(buffer,1,8,stego_pointer)!=8)
        {
            printf("Error: in decode_magic_string unable to read from stego\n");
            return d_failure;
        }
        // call the decode character to decode the character
        if(Decode_character(buffer,&magic)!=NULL)
        {
            if(magic != MAGIC_STRING[i])
            {
                printf("Error:Mgic string did not match\n");
                return d_failure;
            }
        }
    }
    
    return d_success;


}
Dstatus Decode_extenshion_size(FILE *stego_pointer,DecodeInfo *decodeInfo)
{   
    int num=0;
    char buffer[32];
    //step 1: read buffer 32 bytes of buffer from sourse image
    fread(buffer,1,32,stego_pointer);
    //step 2: call decode_integer 
    if((decodeInfo->stego_extenshion_size = *decode_integer(buffer,&num))==d_failure){
        return d_failure;
    }
    printf("size of extenshion is : %d\n",num);
    return d_success;
}

Dstatus decode_sec_ext(DecodeInfo *decodeInfo)
{

    int size = decodeInfo->stego_extenshion_size,i;
    char buffer[8],ch=0;
    char str[size];
    for(i=0;i<size;i++)
    {
    ch = 0;
    //step 1: read 8 bytes of buffer from sourse image file
    fread(buffer,1,8,decodeInfo->stego_image_pointer);
    //call cecode character
    if(Decode_character(buffer,&ch)==NULL){
        return d_failure;
    }
    //copy each character to str
    str[i]=ch;
    }
    // make last bit of str to \0
    str[i]='\0';
    //copy str to decodeInfo->stego_extenshion
    strcpy(decodeInfo->stego_extenshion,str);
    printf("the extenshion of the file is : %s\n",decodeInfo->stego_extenshion);
    printf("the extenshion of the file name is : %s\n",decodeInfo->output);
    // combine two string
    strcat(decodeInfo->output,decodeInfo->stego_extenshion);
    printf("the output file name is  : %s\n",decodeInfo->output);
    return d_success;
}
Dstatus Decode_data_size(DecodeInfo *decodeInfo){
    int num=0;

    char buffer[32];
    //step 1: read 32 bytes of buffer from sourse image file
    fread(buffer,1,32,decodeInfo->stego_image_pointer);
    //call decode integer 
    if((decodeInfo->stego_data_size = *decode_integer(buffer,&num))==d_failure){
        return d_failure;
    }
    printf("size of secreat data is : %d\n",num);
    return d_success;
}
Dstatus decode_sec_data(DecodeInfo *decodeInfo){
    char buffer[8];
    unsigned char ch=0;
    int num = decodeInfo->stego_data_size;
    for(int i=0;i<num;i++)
    {
    //step 1: read 8 bytes of buffer from sourse image file
    fread(buffer,1,8,decodeInfo->stego_image_pointer);
    //call decode character
    Decode_character(buffer,&ch);
    // copy ch to decodeInfo->file_pointer_out
    fputc(ch,decodeInfo->file_pointer_out);
    ch =0;
    }
    return d_success;
}

Dstatus do_Decoding(DecodeInfo *decodeInfo)
{
    // Step 1 -> Call Dopen_files(decodeInfo) to open required files
//           d_success -> Print success message and proceed to next step
//           Otherwise -> (Implicitly) handle failure or proceed based on logic
if(Dopen_files(decodeInfo) == d_success) 
    printf("File successfully opened\n");

// Step 2 -> Call Decode_magic_string to verify the stego-image signature
//           d_success -> Print success message (magic string matched) and proceed
if(Decode_magic_string(decodeInfo->stego_image_pointer) == d_success) 
    printf("magic string matched\n");

// Step 3 -> Call Decode_extenshion_size to get the secret file's extension length
//           d_success -> Print success message and proceed
if(Decode_extenshion_size(decodeInfo->stego_image_pointer, decodeInfo) == d_success) 
    printf("extenshion size has been decoded\n");

// Step 4 -> Call decode_sec_ext to extract the actual extension (e.g., .txt)
//           d_success -> Print success message and proceed
if((decode_sec_ext(decodeInfo)) == d_success) 
    printf("extenshion has been extracted\n");

// Step 5 -> Call open_outfile to create/open the destination file for decoded data
//           d_success -> Print success message and proceed
if((open_outfile(decodeInfo)) == d_success) 
    printf("out put file is open \n");

// Step 6 -> Call Decode_data_size to determine the size of the hidden secret message
//           d_success -> Print success message and proceed
if((Decode_data_size(decodeInfo)) == d_success) 
    printf("data size has been successfully decoded\n");

// Step 7 -> Call decode_sec_data to extract and write the secret data to the output file
//           d_success -> Print success message and complete process
if((decode_sec_data(decodeInfo)) == d_success) 
    printf("successfully data has been decoded\n");

// Final Step -> Return d_success indicating the full decoding process is finished
return d_success;

    // if(Dopen_files(decodeInfo)==d_success) printf("File sucessfully opned\n");
    // if(Decode_magic_string(decodeInfo->stego_image_pointer)==d_success) printf("magic string matched\n");
    // if(Decode_extenshion_size(decodeInfo->stego_image_pointer,decodeInfo)==d_success) printf("extenshion size has been decoded\n");
    // if((decode_sec_ext(decodeInfo))==d_success) printf("extenshion has been extracted\n");
    // if((open_outfile(decodeInfo))==d_success) printf("out put file is open \n");
    // if((Decode_data_size(decodeInfo))==d_success) printf("data size has been sucessfully decoded\n");
    // if((decode_sec_data(decodeInfo))==d_success) printf("sucessfully data has been decoded\n");
    // return d_success;

}