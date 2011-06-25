#ifndef ANN_PERCEPTRON_H
#define ANN_PERCEPTRON_H

struct perceptron
{
	struct connection **inputs, **outputs;
	int inputs_count, outputs_count;
	double sum, output;
};

struct perceptron *perceptron_create(int inputs_count);
void perceptron_free(struct perceptron **perceptron);
void perceptron_dump(struct perceptron *perceptron);
void perceptron_init(struct perceptron *perceptron, int outputs_count);
void perceptron_connect_input(struct perceptron *perceptron, struct connection *connection, int i);
void perceptron_connect_output(struct perceptron *perceptron, struct connection *connection, int i);
double perceptron_g(double sum);
void perceptron_run(struct perceptron *perceptron);
double perceptron_get_output(struct perceptron *perceptron);
void perceptron_set_output(struct perceptron *perceptron, double output);

double perceptron_error(struct perceptron *perceptron, double *deltas);
void perceptron_update(struct perceptron *perceptron, double delta, double alpha);

#endif
