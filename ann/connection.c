#include "connection.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct connection *connection_create(struct perceptron *from, struct perceptron *to, double weight)
{
	struct connection *c = 0;
	
	assert(to);
	c = (struct connection*)malloc(sizeof(struct connection));
	assert(c);

	c->from = from;
	c->to = to;
	c->weight = weight;
	c->value = 1.0;
	
	return c;
}

void connection_free(struct connection **connection)
{
	assert(connection);
	if (*connection)
	{
		free(*connection);
		*connection = 0;
	}
}

void connection_dump(struct connection *connection)
{
	assert(connection);
	printf("Connection: w = %f, v = %f\n", connection->weight, connection->value);
}

double connection_get_weighted_value(struct connection *connection)
{
	assert(connection);
	return connection->weight * connection->value;
}

void connection_set_value(struct connection *connection, double value)
{
	assert(connection);
	connection->value = value;
}

void connection_update(struct connection *connection, double delta, double alpha)
{
	assert(connection);
	connection->weight += alpha * connection->value * delta;
}

double connection_get_weight(struct connection *connection)
{
	assert(connection);
	return connection->weight;
}

