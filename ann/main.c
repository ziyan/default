#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "ann.h"

int main(int argc, char **argv)
{
	int hidden_layers_perceptrons_counts[1] = {32};
	struct ann* ann = ann_create(16, 16, 1, hidden_layers_perceptrons_counts);
	assert(ann);
	
	char letters[16] = {'A', 'C', 'D', 'F', 'H', 'I', 'J', 'L', 'N', 'O', 'P', 'T', 'U', 'X', 'Y', 'Z'};
	double letters_bitmap[16][16] = {
		{0,1,1,0,
		 1,0,0,1,
		 1,1,1,1,
		 1,0,0,1}, // A
		{1,1,1,1,
		 1,0,0,0,
		 1,0,0,0,
		 1,1,1,1}, // C
		{1,1,1,0,
		 1,0,0,1,
		 1,0,0,1,
		 1,1,1,0}, // D
		{1,1,1,1,
		 1,0,0,0,
		 1,1,1,1,
		 1,0,0,0}, // F
		{1,0,0,1,
		 1,1,1,1,
		 1,1,1,1,
		 1,0,0,1}, // H
		{1,1,1,1,
		 0,1,1,0,
		 0,1,1,0,
		 1,1,1,1}, // I
		{1,1,1,1,
		 0,0,1,0,
		 1,0,1,0,
		 0,1,1,0}, // J
		{1,0,0,0,
		 1,0,0,0,
		 1,0,0,0,
		 1,1,1,1}, // L
		{1,0,0,1,
		 1,1,0,1,
		 1,0,1,1,
		 1,0,0,1}, // N
		{1,1,1,1,
		 1,0,0,1,
		 1,0,0,1,
		 1,1,1,1}, // O
		{1,1,1,1,
		 1,0,0,1,
		 1,1,1,1,
		 1,0,0,0}, // P
		{1,1,1,1,
		 0,1,1,0,
		 0,1,1,0,
		 0,1,1,0}, // T
		{1,0,0,1,
		 1,0,0,1,
		 1,0,0,1,
		 1,1,1,1}, // U
		{1,0,0,1,
		 0,1,1,0,
		 0,1,1,0,
		 1,0,0,1}, // X
		{1,0,0,1,
		 1,0,0,1,
		 0,1,1,0,
		 0,1,1,0}, // Y
		{1,1,1,1,
		 0,0,1,0,
		 0,1,0,0,
		 1,1,1,1}  // Z
	};
	
	double test_bitmap[16][16] = {
		{0,1,1,0,
		 1,0,0,1,
		 1,1,1,1,
		 1,0,0,1}, // A
		{1,1,1,1,
		 1,0,0,0,
		 1,0,0,1,
		 1,1,1,1}, // C
		{1,1,1,0,
		 1,0,0,1,
		 1,0,0,1,
		 1,1,1,0}, // D
		{1,1,1,1,
		 1,0,0,0,
		 1,1,1,1,
		 1,0,0,0}, // F
		{1,0,0,1,
		 1,1,1,1,
		 1,1,1,1,
		 1,0,0,1}, // H
		{1,1,1,1,
		 0,1,1,0,
		 0,1,1,0,
		 1,1,1,1}, // I
		{1,1,1,1,
		 0,0,1,0,
		 1,0,1,0,
		 0,1,1,0}, // J
		{1,0,0,0,
		 1,0,0,0,
		 1,0,0,0,
		 1,1,1,1}, // L
		{1,0,0,1,
		 1,1,0,1,
		 1,0,1,1,
		 1,0,0,1}, // N
		{1,1,1,1,
		 1,0,0,1,
		 1,0,0,1,
		 1,1,1,1}, // O
		{1,1,1,1,
		 1,0,0,1,
		 1,1,1,1,
		 1,0,0,0}, // P
		{1,1,1,1,
		 0,1,1,0,
		 0,1,1,0,
		 0,1,1,0}, // T
		{1,0,0,1,
		 1,0,0,1,
		 1,0,0,1,
		 1,1,1,1}, // U
		{1,0,0,1,
		 0,1,1,0,
		 0,1,1,0,
		 1,0,0,1}, // X
		{1,0,0,1,
		 1,0,0,1,
		 0,1,1,0,
		 0,1,1,0}, // Y
		{1,1,1,1,
		 0,0,1,0,
		 0,1,0,0,
		 1,1,1,1}  // Z
	};
	
	double outputs[16] = {0};
	double errors[16] = {0};
	int i, j, k = 0;
	double mse = 0.0;
	double alpha = 0.5;

	do 
	{
		k = (int)((double)rand() / RAND_MAX * 16);
		ann_run(ann, letters_bitmap[k], outputs);
		for (j = 0; j < 16; j++)
			errors[j] = (j == k ? 1.0 : 0.0) - outputs[j];
		ann_train(ann, errors, alpha);
		mse = 0.0;
		for (j = 0; j < 16; j++)
			mse += errors[j] * errors[j];
		mse *= 0.5;
		if (!(i++ % 1000))
		{
			printf("mse = %f, alpha = %f\n", mse, alpha);
			alpha *= 0.999999999;
		}
	}
	while (mse > 0.001);
	printf("mse = %f, alpha = %f\n", mse, alpha);

	/*
	k = -1;
	for (j = 0; j < 16; j++)
	{
		if (letters[j] == argv[1][0])
		{
			k = j;
			break;
		}
	}
	if (k == -1) return 0;
	*/

	for (i = 0; i < 16; i++)
	{
		ann_run(ann, letters_bitmap[i], outputs);
		//ann_dump(ann);
	
		// find most likely
		double max = 0.0;
		k = 0;
		for (j = 0; j < 16; j++)
		{
			if (outputs[j] > max)
			{
				max = outputs[j];
				k = j;
			}
		}
		printf("%c", letters[k]);
	}
	printf("\n");
	
	for (i = 0; i < 16; i++)
	{
		ann_run(ann, test_bitmap[i], outputs);
		//ann_dump(ann);
	
		// find most likely
		double max = 0.0;
		k = 0;
		for (j = 0; j < 16; j++)
		{
			if (outputs[j] > max)
			{
				max = outputs[j];
				k = j;
			}
		}
		printf("%c", letters[k]);
	}
	printf("\n");
	
	
	ann_free(&ann);
	return 0;
}

