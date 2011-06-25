#ifndef ANN_LAYER_H
#define ANN_LAYER_H

struct layer
{
	struct connection **connections;
	struct perceptron **perceptrons;
	int connections_count, perceptrons_count;
	double *errors, *deltas;
};

struct layer *layer_create(struct layer *previous, int n);
void layer_free(struct layer **layer);
void layer_dump(struct layer *layer);
void layer_run(struct layer *layer);
void layer_set_outputs(struct layer *layer, double *outputs);
void layer_get_outputs(struct layer *layer, double *outputs);
double *layer_errors(struct layer *layer, double *deltas);
double *layer_backprop(struct layer *layer, double *errors, double alpha);

#endif


