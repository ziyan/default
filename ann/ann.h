#ifndef ANN_H
#define ANN_H

struct ann
{
	struct layer **layers;
	int layers_count;
};

struct ann *ann_create(int inputs_count, int outputs_count, int hidden_layers_count, int *hidden_layers_perceptrons_count);
void ann_free(struct ann** ann);
void ann_dump(struct ann* ann);
void ann_run(struct ann* ann, double *inputs, double *outputs);
void ann_train(struct ann* ann, double *errors, double alpha);

#endif
