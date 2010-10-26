#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define u8 unsigned char

int get_macro_block(u8 * data_buf, u8 ** mblock, int offset)
{
    int i,j;

    data_buf = data_buf + offset;
    
    // pull a 16bit x 16bit macro block of data 
    // from data_buf, starting at offset
    for (i=0; i<16; ++i) {
        for (j=0; j<2; ++j) {
            mblock[i][j] = *(data_buf);
            ++data_buf;
        }
     }

     return 1;
}

int get_block(u8 ** mblock, u8 * block, int index)
{
    int i;
    int offset = 0;
    
    mblock = mblock + index;

    // pull an 8bit x 8bit block of data
    // from mblock, starting at the given
    // index
    for (i=0; i<8; ++i) {
        if(offset > 1) {
            offset = 0;
            ++index;
        }
       
        block[i] = mblock[index][offset];
        ++offset;
    }
    
    return 1;
}        




    
int main(int argc, char** argv)
{
    // check input
    if (argc != 5) {
	perror("Wrong number of arguments. Exiting.");
	exit(EXIT_FAILURE);
    }
    
    // user arguments
    char* image = argv[1];
    char* quantFileName = argv[2];
    int qScale = atoi(argv[3]);
    char* outputFileName = argv[4];
    printf("%s\n", image);
    printf("%s\n", quantFileName);    
    printf("%d\n", qScale);
    printf("%s\n", outputFileName);


    // PGM header file information
    char* head = "";
    char* xValTemp = "";
    int xVal = 0;
    char* yValTemp = "";
    int yVal = 0;
    char* colorVal;


    // read input from PGM file image
    FILE* dataFile = fopen(image,"r");
    FILE* outputFileHandle = fopen(outputFileName,"w");
    // read the first line of the PGM file, should be P5
    char tempBuff[256];
    fgets(tempBuff, 10, dataFile);
    printf("tempBuff: %s\n", tempBuff);

    // read the second line of input, x and y pixel sizes
    fgets(tempBuff,256,dataFile);
    printf("tempBuff line 2: %s\n", tempBuff);
   

    // get each pixel number, delimited by a space and
    // copy x value from buffer 
    xValTemp = strtok(tempBuff, " ");
    xVal = atoi(xValTemp);
    printf("xVal: %d\n", xVal);

    // get and copy y value from buffer
    yValTemp = strtok(NULL," ");
    yVal = atoi(yValTemp);
    printf("yVal: %d\n", yVal);

    // get and copy color value from PGM file
    fgets(tempBuff, 256, dataFile);
    colorVal = (char *) malloc(sizeof(tempBuff)/sizeof(tempBuff[0]));
    strcpy(colorVal, tempBuff);
    printf("colorval: %s\n", colorVal);

    // create buffer and store grayscale data
    u8 * dataBuffer = (u8 *) malloc(xVal*yVal);  
    fread(dataBuffer, sizeof(u8), (xVal*yVal), dataFile);
    fclose(dataFile);
    printf("%x\n",  dataBuffer);


    return 1;
}
