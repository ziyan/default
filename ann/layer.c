#include "layer.h"
#include "perceptron.h"
#include "connection.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct layer *layer_create(struct layer *previous, int n)
{
	struct layer* layer = 0;
	int i = 0, j = 0, k = 0;
	
	assert(n > 0);
	layer = (struct layer*)malloc(sizeof(struct layer));
	assert(layer);
	
	// create perceptrons
	layer->perceptrons_count = n;
	layer->perceptrons = (struct perceptron**)malloc(sizeof(struct perceptron*) * layer->perceptrons_count);
	assert(layer->perceptrons);
	for (i = 0; i < layer->perceptrons_count; i++)
	{
		layer->perceptrons[i] = perceptron_create(previous ? previous->perceptrons_count + 1 : 0);
	}
	
	layer->errors = (double*)malloc(sizeof(double) * layer->perceptrons_count);
	assert(layer->errors);
	
	layer->deltas = (double*)malloc(sizeof(double) * layer->perceptrons_count);
	assert(layer->deltas);

	// if there is a layer before us?
	if (previous)
	{

		// create a list to hold all connections between this layer and previous layer
		layer->connections_count = previous->perceptrons_count * layer->perceptrons_count + layer->perceptrons_count;
		layer->connections = (struct connection**)malloc(sizeof(struct connection*) * layer->connections_count);

		for (i = 0; i < previous->perceptrons_count; i++)
		{
			// initialize previous layer's perceptron so that it knows number of outputs it is connected to
			perceptron_init(previous->perceptrons[i], layer->perceptrons_count);

			for (j = 0; j < layer->perceptrons_count; j++)
			{
				// create the connection and connect the perceptrons
				layer->connections[k] = connection_create(previous->perceptrons[i], layer->perceptrons[j], -1.0 / (double)((i + 1) * (j + 1)));
				perceptron_connect_output(previous->perceptrons[i], layer->connections[k], j);
				perceptron_connect_input(layer->perceptrons[j], layer->connections[k], i + 1);
				k++;
			}
		}
		
		// create bias connections
		for (j = 0; j < layer->perceptrons_count; j++)
		{
			layer->connections[k] = connection_create(0, layer->perceptrons[j], -1.0 / (double)(j + 1));
			perceptron_connect_input(layer->perceptrons[j], layer->connections[k], 0);
			k++;
		}
	}
	else
	{
		layer->connections_count = 0;
		layer->connections = 0;
	}
	return layer;
}


void layer_free(struct layer **layer)
{
	int i = 0;

	assert(layer);
	if (*layer)
	{
		if ((*layer)->perceptrons)
		{
			for (i = 0; i < (*layer)->perceptrons_count; i++)
			{
				perceptron_free(&(*layer)->perceptrons[i]);
			}
			free((*layer)->perceptrons);
			(*layer)->perceptrons = 0;
		}
		(*layer)->perceptrons_count = 0;

		if ((*layer)->connections)
		{
			for (i = 0; i < (*layer)->connections_count; i++)
				connection_free(&(*layer)->connections[i]);
			free((*layer)->connections);
			(*layer)->connections = 0;
		}
		(*layer)->connections_count = 0;
		
		if ((*layer)->errors)
		{
			free((*layer)->errors);
			(*layer)->errors = 0;
		}
		
		if ((*layer)->deltas)
		{
			free((*layer)->deltas);
			(*layer)->deltas = 0;
		}

		free(*layer);
		*layer = 0;
	}
}

void layer_dump(struct layer *layer)
{
	int i = 0;

	assert(layer);

	// run each perceptron
	for (i = 0; i < layer->perceptrons_count; i++)
	{
		perceptron_dump(layer->perceptrons[i]);
	}
}

void layer_run(struct layer *layer)
{
	int i = 0;

	assert(layer);

	// run each perceptron
	for (i = 0; i < layer->perceptrons_count; i++)
	{
		perceptron_run(layer->perceptrons[i]);
	}
}

void layer_set_outputs(struct layer *layer, double *outputs)
{
	int i = 0;

	assert(layer);

	// run each perceptron
	for (i = 0; i < layer->perceptrons_count; i++)
	{
		perceptron_set_output(layer->perceptrons[i], outputs[i]);
	}
}

void layer_get_outputs(struct layer *layer, double *outputs)
{
	int i = 0;

	assert(layer);

	// run each perceptron
	for (i = 0; i < layer->perceptrons_count; i++)
	{
		outputs[i] = perceptron_get_output(layer->perceptrons[i]);
	}
}

double *layer_errors(struct layer *layer, double *deltas)
{
	int i = 0;

	assert(layer);
	assert(deltas);

	for (i = 0; i < layer->perceptrons_count; i++)
	{
		layer->errors[i] = perceptron_error(layer->perceptrons[i], deltas);
	}
	return layer->errors;
}

double *layer_backprop(struct layer *layer, double *errors, double alpha)
{
	int i = 0;

	assert(layer);
	assert(errors);

	for (i = 0; i < layer->perceptrons_count; i++)
	{
		layer->deltas[i] = errors[i] * perceptron_get_output(layer->perceptrons[i]) * (1.0 - perceptron_get_output(layer->perceptrons[i]));
		perceptron_update(layer->perceptrons[i], layer->deltas[i], alpha);
	}
	return layer->deltas;
}


