#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILE_NAME 30
#define BINARYLENGTH 9
#define MAX_CODEWORD_SIZE 16
#define MAX_DATAWORD_SIZE 8

void asciichange(char,char*);
int divide(char *,char*,char*);
void binaryToByte(char*,int,char*,int);

void binaryToByte(char* binary, int binaryLength, char* byteBuffer,int byteBufferLength) {
	size_t i;
        for (i = 0; i < byteBufferLength; ++i) {
        	unsigned char byte = 0;
          	for (size_t j = 0; j < 8; ++j) {
             		if (binary[i * 8 + j] == '1') {
                  		byte |= (1 << (8 - 1 - j));
              		}
		}
		byteBuffer[i]=byte;
	}
}

void asciichange(char c,char *binary){

	for(int i=7;i>=0;i--) binary[7-i]=(c&(1<<i))?'1':'0';
	binary[8]='\0';
}

int divide(char *codeword,char *generator,char *print_data){

    int i,j;
	int codeword_size=strlen(codeword);
	int genertator_size=strlen(generator);
	int print_data_size=strlen(print_data);
	char temp[codeword_size];
	int temp_size=codeword_size-genertator_size+1;
	
	strcpy(temp,codeword);
	print_data=(char*)realloc(print_data,print_data_size+codeword_size-genertator_size+2);
	strncat(print_data,temp,temp_size);	
	while(codeword_size!=genertator_size-1){
		if(codeword[0]=='1'){
			for (j = 0; j < genertator_size; j++) {
        			codeword[j] = ((codeword[j] - '0') ^ (generator[j] - '0')) + '0';
        		}
		}
        	memmove(codeword,codeword+1,codeword_size);
		codeword_size=strlen(codeword);
	}
	
	for (i = 0; i < genertator_size-1; i++) {
        	if (codeword[i] != '0'){
			return 0;
		}
    	}	
	
    	return 1;
}

int main(int argc,char *argv[]){
	
	FILE *in;
	FILE *out;
    FILE *res;
	int total=0,error=0;
    long i,j,input_size,ret;
	char paddingByte;
    char *input_data;
    char *codeword;
    int padding_size;
	int size=0;
    char binary[BINARYLENGTH];
    int codeword_size;
	char *print_data;
	if(argc!=6){
		
		printf("usage: ./crc_encoder input_file output_file generator dataword_size\n");
		return 0;
	}
	
	if((in=fopen(argv[1],"rb"))==NULL){

		printf("input file open error.\n");
		return 0;
	}	
	
	if((out=fopen(argv[2],"w"))==NULL){

		printf("output file open error.\n");
		return 0;
	}
	
    if((res=fopen(argv[3],"w"))==NULL){

		printf("output file open error.\n");
		return 0;
	}

	int generator_len=strlen(argv[4]);
    int dataword_size=atoi(argv[5]);
	if(strcmp(argv[5],"4")!=0&& strcmp(argv[5],"8")!=0){

		printf("dataword size must be 4 or 8\n");
		return 0;
	}	
	
	fread(&paddingByte,1,1,in);

	padding_size=(int)paddingByte;	

    fseek(in,0,SEEK_END);
    input_size=ftell(in);
    fseek(in,1,SEEK_SET);

	input_data=(char*)malloc(input_size);    
	fread(input_data,1,input_size,in);

	char* bit_data=(char*)malloc(input_size*8+1);
	
    for(i=0;i<input_size;i++){

        asciichange(input_data[i],binary);
        strncat(bit_data,binary,8);
    }

    bit_data[input_size*8]='\0';
    codeword_size=generator_len+dataword_size-1;
    i=padding_size;
    j=0;
    codeword=(char*)malloc(codeword_size+1);
	print_data=(char*)malloc(1);
    while(bit_data[i]!='\0'){

        if(j==codeword_size){
            codeword[codeword_size]='\0';
            ret=divide(codeword,argv[4],print_data);
            total++;
            if(!ret) error++;
            codeword[0]='\0';
            j=0;
        }

        strncat(codeword,bit_data+i,1);

        i++;
        j++;
    }

    fprintf(res,"%d %d\n",total-1,error);
	char *byte_data;
	int print_data_size=strlen(print_data);
	byte_data=(char*)malloc(sizeof(char)*(print_data_size/8+1));
	binaryToByte(print_data,print_data_size,byte_data,print_data_size/8+1);
	byte_data[strlen(print_data)/8]='\0';
	fprintf(out,"%s",byte_data);

    fclose(in);
    fclose(out);
	fclose(res);
	free(print_data);
	free(bit_data);
	free(input_data);
    free(codeword);
    return 0;
}
