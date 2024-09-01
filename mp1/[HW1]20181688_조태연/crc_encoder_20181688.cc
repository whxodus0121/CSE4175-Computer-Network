#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_FILE_NAME 30
#define BINARYLENGTH 9
#define MAX_CODEWORD_SIZE 16
#define MAX_DATAWORD_SIZE 8

void asciichange(char,char*);
void divide(char*,char*);
void binaryTobyte(char*,int,char*,int);

void binaryToByte(char* binary, int binaryLength, char* byteBuffer,int byteBufferLength) {
    size_t i;
    for (i = 0; i < byteBufferLength; ++i) {
        unsigned char byte = 0;
        for (size_t j = 0; j < 8; ++j) {
            if (binary[i * 8 + j] == '1') {
                byte |= (1 << (8 - 1 - j));
            }
        }
        byteBuffer[i] = byte;
    }
}

void asciichange(char c,char *binary){

	for(int i=7;i>=0;i--) binary[7-i]=(c&(1<<i))?'1':'0';
	binary[8]='\0';
}

void divide(char *codeword,char *generator){

	//modulo-2로 나누기 한 후에 완성된 codeword완성시키기
	int i,j;
	int codeword_size=strlen(codeword);
	int genertator_size=strlen(generator);
	
	char *temp=(char*)malloc(genertator_size);
	strncpy(temp,codeword,genertator_size);
	/*
	temp[genertator_size] = '\0';

	for (int i = 0; i < codeword_size - genertator_size + 1; i++) {
        	if (temp[0] == '1') {
            		for (int j = 1; j < genertator_size; j++) {
                		temp[j] = ((temp[j] - '0') ^ (generator[j] - '0')) + '0';
            		}
        	}
        	temp[genertator_size-1] = codeword[i + genertator_size];
        	temp[genertator_size] = '\0';
        	if (i + genertator_size < codeword_size) {
            		memmove(temp, temp+1, codeword_size);
        	}
	}
	*/
    	for(i=0;i<codeword_size-genertator_size+1;i++){

		if (temp[0] == '1') {
			for (j = 0; j < genertator_size; j++) {
				temp[j] = ((temp[j] - '0') ^ (generator[j] - '0')) + '0';
			}
			for(j=1;j<genertator_size;j++){
				temp[j-1]=temp[j];
			}
			temp[j-1]='0';
       		}
		else{
			for(j=1;j<genertator_size;j++){
	
				temp[j-1]=temp[j]; 
			}
			temp[j-1]='0';
		}
	}	
	temp[genertator_size]='\0';
	for(j=codeword_size-genertator_size+1;j<codeword_size;j++){

		codeword[j]=temp[j-genertator_size];
	}
	//strncat(codeword+codeword_size-genertator_size,temp,genertator_size-1);
	free(temp);
}

int main(int argc,char *argv[]){
	
	FILE *in;
	FILE *out;
	int i,j;
	char c;
	char *temp_data;
	int size=0;
	if(argc!=5){
		
		printf("usage: ./crc_encoder input_file output_file generator dataword_size\n");
		return 0;
	}
	
	if((in=fopen(argv[1],"rb"))==NULL){

		printf("input file open error.\n");
		return 0;
	}	
	
	if((out=fopen(argv[2],"wb"))==NULL){

		printf("output file open error.\n");
		return 0;
	}
	
	int generator_len=strlen(argv[3]);

	if(strcmp(argv[4],"4")!=0&& strcmp(argv[4],"8")!=0){

		printf("dataword size must be 4 or 8\n");
		return 0;
	}	

	c=fgetc(in);
	int dataword_size=atoi(argv[4]);

	char binary[BINARYLENGTH];
	
	char *codeword;
	codeword=(char*)malloc((dataword_size+generator_len)*sizeof(char));
	int codeword_size;
	while(c!=EOF){		
		
		asciichange(c,binary); // input파일에서 입력받아온 한글자를 이진수로 바꿔서 binary에 넣음

		for(i=0;i<BINARYLENGTH;i+=dataword_size){

			
			*codeword='\0';
			strncpy(codeword,binary+i,dataword_size);
			codeword[dataword_size]='\0';
			char digit='0';
			for(j=0;j<generator_len-1;j++){

				strncat(codeword,&digit,1);
			}
			divide(codeword,argv[3]);
			codeword_size=strlen(codeword);
			//fwrite(codeword,1,1,out);
			size+=strlen(codeword);
			temp_data=(char*)realloc(temp_data,size+1);
			strcat(temp_data,codeword);			
		}

		c=fgetc(in);
	}
//	fclose(out);

	long file_size;
	//char *temp_data;
	/*
	if((out=fopen(argv[2],"rb"))==NULL){

		printf("output file open error.\n");
		return 0;
	}*/

	char padding_size[9];
	int padding=8-size%8;
	/*
	fseek(out, 0, SEEK_END);
    	file_size = ftell(out);
    	rewind(out);

	temp_data=(char*)malloc(file_size);
	fread(temp_data,1,file_size,out);
	fclose(out);
        
	if((out=fopen(argv[2],"wb"))==NULL){

		printf("output file open error.\n");
		return 0;
	}
	*/
	char totalpadding[16];
	
	if(padding>0&&padding<8){
		
		for (i = 7; i >= 0; i--) {
        	padding_size[7 - i] = (padding & (1 << i)) ? '1' : '0';
    		}

    		padding_size[8] = '\0';

		//fwrite(padding_size,strlen(padding_size),1,out);
		char padding_bit[9]="00000000";
		padding_bit[padding]='\0';
		//fwrite(padding_bit,strlen(padding_bit),1,out);
		sprintf(totalpadding,"%s%s",padding_size,padding_bit);
	}
	else{
		for(i=0;i<8;i++) padding_size[i]='0';
		padding_size[8]='\0';
		//fwrite(padding_size,strlen(padding_size),1,out);
	}
	char *data;
	char *byte_data;
	data=(char*)malloc((size+17)*sizeof(char));
	byte_data=(char*)malloc((size+17)/8*sizeof(char));

	sprintf(data,"%s%s",totalpadding,temp_data);

	int data_size=strlen(data);
	int data_byte_size=strlen(data)/8;
	
	memset(byte_data, 0, (size + 17) / 8);
	binaryToByte(data,data_size,byte_data,data_byte_size);

	fwrite(byte_data,strlen(byte_data),1,out);
	
	free(codeword);
	fclose(in);
	fclose(out);

	return 0;
}
