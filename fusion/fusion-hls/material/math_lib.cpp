#include <cmath>
#include "hls_math.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;
typedef float data_t;
data_t cpp_math(data_t angle) {
	data_t s = sinf(angle);
	data_t c = cosf(angle);
	return sqrtf(s * s + c * c);
}
data_t cpp_math_sw(data_t angle) {
	data_t s = sinf(angle);
	data_t c = cosf(angle);
	return sqrtf(s * s + c * c);
}

data_t cpp_math2(data_t angle) {
	data_t s = hls::sinf(angle);
	data_t c = hls::cosf(angle);
	return hls::sqrtf(s * s + c * c);
}

int main() {
	data_t angle = 0.01;
	data_t output, exp_output, diff;
	int retval = 0;
	for (data_t i = 0; i <= 250; i++) {
		output = cpp_math(angle);
		exp_output = cpp_math_sw(angle);
// Check for differences
		diff = ((exp_output > output) ?
				exp_output - output : output - exp_output);
		if (diff > 0.0000005) {
			printf("Difference %.10f exceeds tolerance at angle %.10f \n", diff,
					angle);
			retval = 1;
		}
		angle = angle + .1;
	}
	if (retval != 0) {
		printf("Test failed !!!\n");
		retval = 1;
	} else {
		printf("Test passed !\n");
	}
// Return 0 if the test passes
	return retval;
}
