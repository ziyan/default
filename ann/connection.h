#ifndef ANN_CONNECTION_H
#define ANN_CONNECTION_H

struct connection
{
	struct perceptron *from, *to;
	double weight, value;
};

struct connection *connection_create(struct perceptron *from, struct perceptron *to, double weight);
void connection_free(struct connection **connection);
void connection_dump(struct connection *connection);
double connection_get_weighted_value(struct connection *connection);
void connection_set_value(struct connection *connection, double value);
void connection_update(struct connection *connection, double delta, double alpha);
double connection_get_weight(struct connection *connection);

#endif
