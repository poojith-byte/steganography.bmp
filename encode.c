#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions 
* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data    
    int i=0;
    char ch;
    while((ch=fgetc(fptr))!=EOF){
        i++;
    }
    printf("size of sec file: %d\n",i);
    return i;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
int check_extenshion(char name[],char str[]){
    // int bmp,i=0,j=0,k=0;
    // char dot[20];
    // while(name[i]!='\0'){
    //     if(name[i]=='.'){
    //         j=i;
    //         while(name[j]!='\0'){
    //             dot[k]=name[j];
    //             k++;j++;
    //         }
    //         dot[k]='\0';
    //         break;
    //     }
    //     i++;
    // }
    // if(!(strcmp(dot,str))){
    //     return 1;
    // }
    // else {return 0;}
    char *dot =strrchr(name,'.');
    if(dot && strcmp(dot,str)==0){
        return 1;
    }
    return 0;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{   

    //step1 -> check source file name having .bmp present or not
    // no -> return e_failure
    // yes -> store source file name into encInfo->src_image_fname
    if(check_extenshion(argv[2],".bmp")){
        encInfo->src_image_fname = argv[2];
    }
    else{
        return e_failure;
    }
    
    //step2 -> check secret file having extn or not
    // no -> return e_failure
    // yes -> store secret file name into encInfo->src_image_fname
    
    if(check_extenshion(argv[3],".txt")){
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file,".txt");
    }
    else{
    return e_failure;
    }
    
    // step3 -> check optional file is passed or not
    // yes -> check the file having .bmp or not
            // no -> return e_failure
            //yes -> store the file name into encInfo->stego_image_fname
    // no -> store default name to encInfo->stego_image_fname = "stego.bmp";        
    
    if(argv[4]!=NULL){
        if(!check_extenshion(argv[4],".bmp")){
        return e_failure;
    }
    encInfo->stego_image_fname=argv[4];
    }
    else{
        encInfo->stego_image_fname="stego.bmp";
    }
    //step4 -> return e_success
    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    // step1 -> encInfo->image_capacity =get_image_size_for_bmp(source_file_pointer)
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    // step2 -> find secret file size encInfo -> size_secret_file = get_file_size(secret file pointer)
    encInfo->size_secret_file= get_file_size(encInfo->fptr_secret);
    // step3 -> compare encInfo->image_capacity > 16 + 32 + 32 + 32 + 54 + (encInfo -> size_secret_file * 8)
            //yes -> return e_success
            // no -> return e_failure
    if((encInfo->image_capacity)>16+32+32+54+(encInfo->size_secret_file*8)){
        return e_success;
    }
    else{
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{   
    //step 1 : rewind file pointer to oth position, and create a buffer to store the 54 bytes of data
    rewind(fptr_src_image);
    rewind(fptr_dest_image);
    //step 2 : read 54 bytes from sourse file(use fread) 
    char head[54];
    if(fread(head,1,54,fptr_src_image)!=54){
        printf("couldnot able to read from sourse image , or copy to char head[54]");
        return e_failure;
    }
    //step 3 : write the 54 bytes to stego image(from buffer);
    if(fwrite(head,1,54,fptr_dest_image)!=54){
        printf("couldnot able to write to destination image");
        return e_failure;
    }
    //if ok return success else no
    return e_success;
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    //step1: read buffer 8 bytes of buffer from sourse image
    int len = strlen(MAGIC_STRING);
    char buffer[8];
    
    for(int i=0;i<len;i++)
    {   //read 8bytes from sourse image
        if((fread(buffer,1,8,encInfo->fptr_src_image))!=8){
            printf("Error:could not read magic string 8byte buffer");
            return e_failure;
        }
        //step2: call encode_byte_to_lsb(magic_string[0],buffer);
        if((encode_byte_to_lsb(MAGIC_STRING[i],buffer))==e_failure){
            printf("Error:Magic string encode byte to lsb failed at %c",MAGIC_STRING[i]);
            return e_failure;
        }
        //step3: store the buffer to stugo image file;
        if(fwrite(buffer,1,8,encInfo->fptr_stego_image)!=8){
            printf("Error:couldnot load the data to fptr stego file in encoading magic string function");
            return e_failure;
        }
        
    }
    // do this up to magic string size type(step 1)
    //return e_success
    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{   
    char buffer[32];
    int i=0;
    //step 1: read buffer 32 bytes of buffer from sourse image
    fread(buffer,1,32,encInfo->fptr_src_image);
    //step 2: call encode_size_to_lsb (size,image buzzer)
    if((encode_size_to_lsb(size,buffer))==e_failure){
        printf("couldnnot load the file size to buffer\n");
    }
    //step 3: store the buffer to stugo imagt file
    if (fwrite(buffer,1,32,encInfo->fptr_stego_image)!=32){
        printf("couldnot able to load the data to stego image\n");
        return e_failure;
    }
    return e_success;
    // return e_success
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    int len = strlen(file_extn),i=0;
    char buffer[8];
    for(i=0;i<len;i++){
        //step 1: read 8 bytes of buffer from sourse image file
        if((fread(buffer,1,8,encInfo->fptr_src_image))!=8){
            printf("error in encoading secret file extenshion\n");
            return e_failure;
        }
         //step 2: call encode_byte_to_lsb(file_exten[0],buffer);
        if((encode_byte_to_lsb(file_extn[i],buffer))==e_failure){
            printf("Error in encoding secret file extenshion to buffer\n");
            return e_failure;
        }
        //store the fuffer into stugo.image file
        if(fwrite(buffer,1,8,encInfo->fptr_stego_image)!=8){
            printf("Error couldnot wble to load the data to stego in encode_secret_file_extn");
        }

    }
        //repeat this upto size of exten size
        //return e_success;
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{   char buffer[32];
    //step 1: read buffer 32 bytes of buffer from sourse image
    fread(buffer,1,32,encInfo->fptr_src_image);
    //step 2: call encode_size_to_lsb (file_size,image buzzer)
    if((encode_size_to_lsb(file_size,buffer))==e_failure){
        printf("Error: in encode secret file size could not able to load size\n");
        return e_failure;
    }
    //step 3: store the buffer to stego image file
    if((fwrite(buffer,1,32,encInfo->fptr_stego_image))!=32){
        printf("Error: in encode secret file size couldnot able to load the data to stego image \n");
        return e_failure;
    }
    // return e_success
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{   
    rewind(encInfo->fptr_secret);
    for(int i=0;i<encInfo->size_secret_file;i++){
        char data_buffer[1];
        char image_buffer[8];
        //step1: read the secret data in to one data buffer;
         if(fread(data_buffer,1,1,encInfo->fptr_secret)!=1){
            printf("Error: in encode_secret_file_data could not able to load to buffer\n");
        }
        //step2: read buffer 8 bytes of buffer from sourse image
        if(fread(image_buffer,1,8,encInfo->fptr_src_image)!=8){
            printf("Error: in encode_secret_file_data could not able to load to buffer\n");
        }
        //step 2: call encode_byte to lsb (data buffer[0],image buzzer)
        if((encode_byte_to_lsb(data_buffer[0],image_buffer))==e_failure){
            printf("Error encode_secret_file_data could not able to process the data\n");
            return e_failure;
        }
        //step 3: store the buffer to stego image file
        if(fwrite(image_buffer,1,8,encInfo->fptr_stego_image)!=8){
            printf("Error: in encode_secret_file_data could not able to load to stego\n");
            return e_failure;
        }

    }
        //repeat the same step till secreat file size;
    // return e_success
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    //run a loop up to reaching EOF 
    //read buffer from sourse image file
    //store into stego image file;
    //return e_success
    int ch;
    while((ch=fgetc(fptr_src))!=EOF){
        fputc(ch,fptr_dest);
    }
    return e_success;
}


Status encode_byte_to_lsb(char data, char *image_buffer)//for character
{
   //logic to encode data.8times 
   int i=0;
   for(i=0;i<8;i++){
    image_buffer[i]=(image_buffer[i]&0xfe)|((data>>(7-i))&1);
   }
   if(i==8){
   return e_success;}
   else{
    return e_failure;
   }
}

Status encode_size_to_lsb(int size, char *imageBuffer)//for intiger
{
    //logic to encode data.32times 
    int i=0;
    for(i=0;i<32;i++){
        imageBuffer[i]=((imageBuffer[i]&0xfe)|((size)>>(31-i))&1);
    }
        return e_success;}

Status do_encoding(EncodeInfo *encInfo)
{
    //step1 -> call open file(encInfo)
            // e_failure -> print error msg and return e_failure
            // e_success -> print success msg goto next step
    if(open_files(encInfo)==e_failure){
        printf("Error: unable to open file\n");
        return e_failure;
    }
    else{
        printf("File open success\n");
    }
    // step2 -> call check_capacity(encInfo)
            // e_failure -> print error msg and return e_failure
            // e_success -> print success msg goto next step
    if(check_capacity(encInfo)==e_failure){
        printf("Error: insufficient capacity in image\n");
        return e_failure;
    }
    else{
        printf("Efficient capacity in imaage\n");
  
    }
    //step 3 call copy_bmp_header(encInfo->fpter_src_image,encInfo->fptr_stego_image)
        //print success message and go to next step
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
        printf("header data transfer complete\n");
    }
    //step 4 : call encode magic string(MAGIC_STRING(which is in common .h file and it is macro),encInfo)
        //print success message and go to next step
    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success){
        printf("magic string copyed successfully\n");
    }
    //step 5: encode_secret_file_extn_size(strlen(encinfo->extn_secret_file,)
        //print success message and go to next step
    if((encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo))==e_success){
        printf("secret file extension size has transfered completly\n");
    }
    //step 6: call encode_secrot_file_extenshiom(encInfo->extn_secret_file,enxInfo)
        //check it is retuening e sucess or not , print success message and go to next step
     if((encode_secret_file_extn(encInfo->extn_secret_file,encInfo))==e_success){
        printf("file extenshion is transfer completly\n");
    }
    //step 7:encode_secrot_file_size(encInfo->size_secret_file,encfo);
        //check it is retuening e sucess or not , print success message and go to next step
    if((encode_secret_file_size(encInfo->size_secret_file,encInfo))==e_success){
        printf("size of secreat file successfully transfered\n");
    }
    //step 8: encode_sec_file_data(encinfo); 
        ////check it is retuening e sucess or not , print success message and go to next step
    if((encode_secret_file_data(encInfo))==e_success){
        printf("secreat data has sucess fully transfered\n");
    }
    //step 9: call remaining imgdata(pass two file pointer)
        //check it is retuening e sucess or not , print success message and go to next step
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
        printf("Remaining data transfered completly\n");
    }
    //step 10: retuen e sucess

    return e_success;
}
