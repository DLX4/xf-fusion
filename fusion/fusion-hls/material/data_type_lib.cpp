// C++ 任意精度类型 整型
#include "ap_int.h"
typedef ap_int<6> dinA_t;
typedef ap_int<12> dinB_t;
typedef ap_int<22> dinC_t;

// C++ 任意精度类型 小数
#include "ap_fixed.h"
typedef ap_ufixed<10,8, AP_RND, AP_SAT> din1_t;
typedef ap_fixed<6,3, AP_RND, AP_WRAP> din2_t;
typedef ap_fixed<22,17, AP_TRN, AP_SAT> dint_t;
typedef ap_fixed<36,30> dout_t;
dout_t cpp_ap_fixed(din1_t d_in1, din2_t d_in2) {
	static dint_t sum;
	sum += d_in1;
	return sum * d_in2;
}

