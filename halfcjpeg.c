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
        for (j=0; j<16; ++j) {
            mblock[i][j] = *(data_buf);
            ++data_buf;
        }
     }

     return 1;
}

int get_block(u8 ** mblock, u8 ** block, int offsetx, int offsety)
{
    int i, j;

    // pull an 8bit x 8bit block of data
    // from mblock, starting at the given
    // index
    for (i=0; i<8; ++i) {
        ++offsetx;
        for (j=0; j<8; ++j) {
            block[i][j] = mblock[offsetx][offsety];
            ++offsety;   
        }
     }
    
    return 1;
}        

int transform_block(u8 ** block, double ** dct_matrix)
{
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
    int i;
 
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
    u8 * dataBuffer = (u8 *) malloc(xVal*yVal + 1);  
    fread(dataBuffer, sizeof(u8), (xVal*yVal), dataFile);
    fclose(dataFile);
    printf("%x\n",  dataBuffer);

    // read in quantfile matrix
    int ** q_matrix;
    int temp;
    int x = 0;
    int y = 0;
    FILE * q_file = fopen(quantFileName, "r");
    
    // allocate quantization matrix
    q_matrix = (int **) malloc(8 * sizeof(int *));
    for (i=0; i<8; ++i) {
        q_matrix[i] = (int *) malloc(8 * sizeof(int));
    }   
 
    while (fscanf(q_file, "%d", &temp) != EOF) {
        q_matrix[x][y] = temp;
        ++x;
        if (x > 7) {
           x = 0;
           ++y;
        }
    }
  
    printf("q_matrix[0][0] %d\n", q_matrix[0][0]);

    int offsetx = 0;
    int offsety = 0;
    int offset = 0;
    u8 ** mb;
    u8 ** block;
    int **  dct_matrix;
    int j, k;
    
    // allocate memory for the dct matrix
    dct_matrix = (int **) malloc(xVal * sizeof(int *));
    for (i=0; i<xVal; ++i) {
        dct_matrix[i] = (int *) malloc(yVal * sizeof(int));
    }

    // allocate memory for the next macro block
    mb = (u8 **) malloc(16 * sizeof(u8*));
    for (i=0; i<16; ++i) {
        mb[i] = (u8 *) malloc(16 * sizeof(u8));
    }

    // allocate memory for block 
    block = (u8 **) malloc(8 * sizeof(u8*));
    for (i=0; i<8; ++i) {
        block[i] = (u8 *) malloc(8 * sizeof(u8));
    }

    while (*(dataBuffer) != '\0') {
       
      get_macro_block(dataBuffer, mb, offsetx);
      for (j=0; j<2; j++) {
          for (k=0; k<2; k++) {
            get_block(mb, block, offsetx, offsety);
            offsetx += 8;
          }
          offsetx = 0;
          offsety += 8; 
      }
    }        
   
    return 1;
}
