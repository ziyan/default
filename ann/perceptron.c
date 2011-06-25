#include "perceptron.h"
#include "connection.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

struct perceptron *perceptron_create(int inputs_count)
{
	struct perceptron* p = 0;
	
	assert(inputs_count >= 0);
	p = (struct perceptron*)malloc(sizeof(struct perceptron));
	assert(p);
	
	p->inputs_count = inputs_count;
	if (p->inputs_count > 0)
	{
		p->inputs = (struct connection**)malloc(sizeof(struct connection*) * p->inputs_count);
		assert(p->inputs);
	}
	else
	{
		p->inputs = 0;
	}

	p->outputs_count = 0;
	p->outputs = 0;
	
	p->sum = 0.0;
	p->output = 0.0;

	return p;
}

void perceptron_free(struct perceptron **perceptron)
{
	assert(perceptron);
	if (*perceptron)
	{
		if ((*perceptron)->inputs)
		{
			free((*perceptron)->inputs);
			(*perceptron)->inputs = 0;
		}
		(*perceptron)->inputs_count = 0;
		
		if ((*perceptron)->outputs)
		{
			free((*perceptron)->outputs);
			(*perceptron)->outputs = 0;
		}
		(*perceptron)->outputs_count = 0;
		
		free(*perceptron);
		*perceptron = 0;
	}
}

void perceptron_init(struct perceptron *perceptron, int outputs_count)
{
	assert(perceptron);
	assert(outputs_count >= 0);
	perceptron->outputs_count = outputs_count;
	if (perceptron->outputs_count > 0)
	{
		perceptron->outputs = (struct connection**)malloc(sizeof(struct connection*) * perceptron->outputs_count);
		assert(perceptron->outputs);
	}
}

void perceptron_dump(struct perceptron *perceptron)
{
	int i = 0;
	assert(perceptron);
	printf("\tPerceptron: [%i, %i], s = %f, o = %f\n", perceptron->inputs_count, perceptron->outputs_count, perceptron->sum, perceptron->output);
	if (perceptron->inputs)
	{
		for (i = 0; i < perceptron->inputs_count; i++)
		{
			printf("\t\t[I] ");
			connection_dump(perceptron->inputs[i]);
		}
	}
	
	if (perceptron->outputs)
	{
		for (i = 0; i < perceptron->outputs_count; i++)
		{
			printf("\t\t[O] ");
			connection_dump(perceptron->outputs[i]);
		}
	}
}

void perceptron_connect_input(struct perceptron *perceptron, struct connection *connection, int i)
{
	assert(perceptron);
	assert(connection);
	assert(i >= 0 && i < perceptron->inputs_count);
	perceptron->inputs[i] = connection;
}

void perceptron_connect_output(struct perceptron *perceptron, struct connection *connection, int i)
{
	assert(perceptron);
	assert(connection);
	assert(i >= 0 && i < perceptron->outputs_count);
	perceptron->outputs[i] = connection;
}

double perceptron_g(double sum)
{
	return 1.0 / (1.0 + exp(-sum));
}

void perceptron_run(struct perceptron *perceptron)
{
	int i = 0;

	assert(perceptron);
	perceptron->sum = 0.0;
	for (i = 0; i < perceptron->inputs_count; i++)
	{
		perceptron->sum += connection_get_weighted_value(perceptron->inputs[i]); 
	}
	perceptron_set_output(perceptron, perceptron_g(perceptron->sum));
}

double perceptron_get_output(struct perceptron *perceptron)
{
	assert(perceptron);
	return perceptron->output;
}

void perceptron_set_output(struct perceptron *perceptron, double output)
{
	int i = 0;

	assert(perceptron);
	perceptron->output = output;
	if (perceptron->outputs)
	{
		for (i = 0; i < perceptron->outputs_count; i++)
		{
			connection_set_value(perceptron->outputs[i], perceptron->output); 
		}
	}
}

double perceptron_error(struct perceptron *perceptron, double *deltas)
{
	int i = 0;
	double error = 0.0;
	assert(perceptron);
	assert(deltas);

	if (perceptron->outputs)
	{
		for (i = 0; i < perceptron->outputs_count; i++)
		{
			error += connection_get_weight(perceptron->outputs[i]) * deltas[i];
		}
	}
	return error;
}

void perceptron_update(struct perceptron *perceptron, double delta, double alpha)
{
	int i = 0;
	assert(perceptron);
	if (perceptron->inputs)
	{
		for (i = 0; i < perceptron->inputs_count; i++)
		{
			connection_update(perceptron->inputs[i], delta, alpha);
		}
	}
}



