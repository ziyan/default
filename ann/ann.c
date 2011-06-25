#include "ann.h"
#include "layer.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct ann *ann_create(int inputs_count, int outputs_count, int hidden_layers_count, int *hidden_layers_perceptrons_count)
{
	int i = 0;
	struct ann *ann = 0;

	assert(inputs_count > 0);
	assert(outputs_count > 0);
	assert(hidden_layers_count > 0);
	assert(hidden_layers_perceptrons_count);

	ann = (struct ann*)malloc(sizeof(struct ann));
	assert(ann);
	
	ann->layers_count = 2 + hidden_layers_count;
	ann->layers = (struct layer**)malloc(sizeof(struct layer*) * ann->layers_count);
	assert(ann->layers);
	
	// create input layer
	ann->layers[0] = layer_create(0, inputs_count);
	
	// create hidden layers
	for (i = 0; i < hidden_layers_count; i++)
	{
		assert(hidden_layers_perceptrons_count[i] > 0);
		ann->layers[i + 1] = layer_create(ann->layers[i], hidden_layers_perceptrons_count[i]);
	}
	
	// create output layer
	ann->layers[ann->layers_count - 1] = layer_create(ann->layers[ann->layers_count - 2], outputs_count);

	return ann;
}

void ann_free(struct ann** ann)
{
	int i = 0;

	assert(ann);
	if (*ann)
	{
		if ((*ann)->layers)
		{
			for (i = 0; i < (*ann)->layers_count; i++)
			{
				layer_free(&(*ann)->layers[i]);
			}
			free((*ann)->layers);
			(*ann)->layers = 0;
		}
		(*ann)->layers_count = 0;
		
		free(*ann);
		*ann = 0;
	}
}

void ann_dump(struct ann* ann)
{
	int i = 0;

	assert(ann);

	printf("=== ANN ===\n");
	for (i = 0; i < ann->layers_count; i++)
	{
		printf("=== Layer %i ===\n", i);
		layer_dump(ann->layers[i]);
	}
}

void ann_run(struct ann* ann, double *inputs, double *outputs)
{
	int i = 0;
	assert(ann);
	assert(inputs);
	assert(outputs);
	
	layer_set_outputs(ann->layers[0], inputs);
	for (i = 1; i < ann->layers_count; i++)
	{
		layer_run(ann->layers[i]);
	}
	layer_get_outputs(ann->layers[ann->layers_count - 1], outputs);
}

void ann_train(struct ann* ann, double *errors, double alpha)
{
	int i = 0;
	double *deltas = 0;
	assert(ann);
	assert(errors);
	deltas = layer_backprop(ann->layers[ann->layers_count - 1], errors, alpha);

	for (i = ann->layers_count - 2; i > 0; i--)
	{
		deltas = layer_backprop(ann->layers[i], layer_errors(ann->layers[i], deltas), alpha);
	}
}

