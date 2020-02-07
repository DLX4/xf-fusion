#include "hls_stream.h"
using namespace hls;
typedef struct {
	short data;
	bool valid;
	bool invert;
} input_interface;
bool invert(stream<input_interface>& in_data_1,
		stream<input_interface>& in_data_2, stream<short>& output) {
	input_interface in;
	bool full_n;
// Read an input value or return
	if (!in_data_1.read_nb(in))
		if (!in_data_2.read_nb(in))
			return false;
// If the valid data is written, return not-full (full_n) as true
	if (in.valid) {
		if (in.invert)
			full_n = output.write_nb(~in.data);
		else
			full_n = output.write_nb(in.data);
	}
	return full_n;
}
