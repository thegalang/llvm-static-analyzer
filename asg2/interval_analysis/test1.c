#include <assert.h>

int main() {
	int x, y, z=10;
	if (y == 10)
		x = z + 0;
	else
		x = z - 10;
	assert (x < 11);
	return x;
}