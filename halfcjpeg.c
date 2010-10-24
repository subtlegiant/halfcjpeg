#include <stdio.h>

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
    
int main(argc, argv)
{

	

    return 1;
}
