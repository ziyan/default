/**
 * Global Define for ERC
 */

#ifndef ERC_DEFINE_H
#define ERC_DEFINE_H

#define VERSION 0x12 //version 1.2

//some useful functions
#define GRID_TO_INDEX(r,c) (r*GRID_WIDTH+c)
#define INDEX_TO_ROW(i) (i/GRID_WIDTH)
#define INDEX_TO_COL(i) (i%GRID_WIDTH)

#define SHORT_TO_WORD_LOW(s) (unsigned char)(s & 0xff)
#define SHORT_TO_WORD_HIGH(s) (unsigned char)((s>>8) & 0xff)
#define WORD_TO_SHORT(l,h) ((unsigned short int)(l & 0xff) + (unsigned short int)((h & 0xff)<<8))

#define ERC_PI 3.141592653589793238462643383279502884197169399375105820974944593207816406286
#define DEG_TO_RAD(x)(((0.0+x)/180)*ERC_PI)
#define RAD_TO_DEG(x)(((0.0+x)/ERC_PI)*180)

#endif

