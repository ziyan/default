#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		cerr << "Usage: " << argv[0] << " <number-of-points>" << endl;
		return -1;
	}
	int n = atoi(argv[1]);
	for (int i = 0; i < n; i++)
	{
		cout << (20.0f * ((float)rand() / (float)RAND_MAX ) - 10.0f)
			<< " " << (20.0f * ((float)rand() / (float)RAND_MAX) - 10.0f) << endl;
	}
	return 0;
}
