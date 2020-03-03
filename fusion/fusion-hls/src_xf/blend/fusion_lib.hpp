#ifndef _XF_FUSION_LIB_
#define _XF_FUSION_LIB_

#include "xf_fusion.h"

namespace fusion {

//template<int ROWS, int COLS>
//void blendLaplacianPyramidsByRE2(xf::Mat<_TYPE, ROWS, COLS, _NPC1>& imageA, xf::Mat<_TYPE, ROWS, COLS, _NPC1>& imageB, xf::Mat<_TYPE, ROWS, COLS, _NPC1>& imageS) {
//	// 均值滤波
//    float G[3][3] = {
//        {0.1111, 0.1111, 0.1111},
//        {0.1111, 0.1111, 0.1111},
//        {0.1111, 0.1111, 0.1111}
//    };
//    float matchDegreeLimit = 0.618;
//
//    int height = imageA.rows;
//    int width = imageB.cols;
//
//    for (int i = 2; i < height - 2; i++) {
//#pragma HLS LOOP_TRIPCOUNT min=1 max=ROWS
//        for (int j = 2; j < width - 2; j++) {
//#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS
//#pragma HLS PIPELINE
//        	// 3*3
//        	float deltaA = 0.0;
//        	float deltaB = 0.0;
//			float matchDegree = 0.0;
//
//			for (int rowOffset = -1; rowOffset <= 1; rowOffset++) {
//#pragma HLS LOOP_TRIPCOUNT min=1 max=3
//#pragma HLS UNROLL
//				for (int colOffset= -1; colOffset <= 1; colOffset++) {
//#pragma HLS LOOP_TRIPCOUNT min=1 max=3
//					// 单通道
//					int x = i + rowOffset;
//					int y = j + colOffset;
//					int index = (int)(x*width+y);
//
//					deltaA += G[1+rowOffset][1+colOffset] * imageA.read(index) * imageA.read(index);
//					deltaB += G[1+rowOffset][1+colOffset] * imageB.read(index) * imageB.read(index);
//					matchDegree += G[1+rowOffset][1+colOffset] * imageA.read(index) * imageB.read(index);
//				}
//			}
//			// 计算匹配度
//			matchDegree = matchDegree * matchDegree / (deltaA * deltaB);
//
//			int pix = (int)(i*width+j);
//			if (hls::isnan(matchDegree) || matchDegree < matchDegreeLimit) {
//				if (deltaA == deltaB) {
//					imageS.write(pix, (int)(0.5 * imageA.read(pix) + 0.5 * imageB.read(pix)));
//				} else if (deltaA > deltaB) {
//					imageS.write(pix, imageA.read(pix));
//				} else {
//					imageS.write(pix, imageB.read(pix));
//				}
//			} else {
//				float wMin = 0.5 * (1 - (1 - matchDegree)/(1 - matchDegreeLimit));
//
//				int min = hls::min<int>(imageA.read(pix), imageB.read(pix));
//				int max = hls::max<int>(imageA.read(pix), imageB.read(pix));
//				float value = min * wMin + max * (1 - wMin);
//				imageS.write(pix, (int)value);
//			}
//        }
//    }
//}

template<int NPC,int DEPTH, int WIN_SZ, int WIN_SZ_SQ,int PLANES>
void xFPyrDownApplykernel(
		XF_PTUNAME(DEPTH) OutputValues[XF_NPIXPERCYCLE(NPC)],
		XF_PTUNAME(DEPTH) src_buf_A[WIN_SZ][XF_NPIXPERCYCLE(NPC)+(WIN_SZ-1)],
		XF_PTUNAME(DEPTH) src_buf_B[WIN_SZ][XF_NPIXPERCYCLE(NPC)+(WIN_SZ-1)],
		ap_uint<8> win_size
		)
{
#pragma HLS INLINE
	ap_uint<32> array_A[WIN_SZ_SQ];
#pragma HLS ARRAY_PARTITION variable=array_A complete dim=1
	ap_uint<32> array_B[WIN_SZ_SQ];
#pragma HLS ARRAY_PARTITION variable=array_B complete dim=1


		int array_ptr=0;

		Compute_Grad_Loop:
		for(int copy_arr=0;copy_arr<WIN_SZ;copy_arr++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=WIN_SZ
#pragma HLS UNROLL
			for (int copy_in=0; copy_in<WIN_SZ; copy_in++)
			{
#pragma HLS LOOP_TRIPCOUNT min=1 max=WIN_SZ
#pragma HLS UNROLL
				array_A[array_ptr] = src_buf_A[copy_arr][copy_in];
				array_B[array_ptr] = src_buf_B[copy_arr][copy_in];
				array_ptr++;
			}
		}
ap_uint<32> out_pixel = 0;
		int k[25]={1,  4,  6,  4, 1,
		           4, 16, 24, 16, 4,
				   6, 24, 36, 24, 6,
				   4, 16, 24, 16, 4,
				   1,  4,  6,  4, 1};
		for(int i=0,k=0; i<PLANES;i++,k+=8)
		{
#pragma HLS PIPELINE II=1
		out_pixel  = array[0*5 + 0].range(k+7,k) + array[0*5 + 4].range(k+7,k) + array[4*5 + 0].range(k+7,k) + array[4*5 + 4].range(k+7,k);
		out_pixel += (array[0*5 + 1].range(k+7,k) + array[0*5 + 3].range(k+7,k) + array[1*5 + 0].range(k+7,k) + array[1*5 + 4].range(k+7,k)) << 2;
		out_pixel += (array[4*5 + 1].range(k+7,k) + array[4*5 + 3].range(k+7,k) + array[3*5 + 0].range(k+7,k) + array[3*5 + 4].range(k+7,k)) << 2;
		out_pixel += (array[0*5 + 2].range(k+7,k) + array[2*5 + 0].range(k+7,k) + array[2*5 + 4].range(k+7,k) + array[4*5 + 2].range(k+7,k)) << 2;
		out_pixel += (array[0*5 + 2].range(k+7,k) + array[2*5 + 0].range(k+7,k) + array[2*5 + 4].range(k+7,k) + array[4*5 + 2].range(k+7,k)) << 1;
		out_pixel += (array[1*5 + 1].range(k+7,k) + array[1*5 + 3].range(k+7,k) + array[3*5 + 1].range(k+7,k) + array[3*5 + 3].range(k+7,k)) << 4;
		out_pixel += (array[1*5 + 2].range(k+7,k) + array[2*5 + 1].range(k+7,k) + array[2*5 + 3].range(k+7,k) + array[3*5 + 2].range(k+7,k)) << 4;
		out_pixel += (array[1*5 + 2].range(k+7,k) + array[2*5 + 1].range(k+7,k) + array[2*5 + 3].range(k+7,k) + array[3*5 + 2].range(k+7,k)) << 3;
		out_pixel += (array[2*5 + 2].range(k+7,k)) << 5;
		out_pixel += (array[2*5 + 2].range(k+7,k)) << 2;

		OutputValues[0].range(k+7,k) = (unsigned char)( (out_pixel + 128) >> 8);
		}
		return;
}

template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH, int TC, int WIN_SZ, int WIN_SZ_SQ,int PLANES>
void xFPyrDownprocessgaussian(hls::stream< XF_TNAME(DEPTH,NPC) > & _src_mat_A,
		hls::stream< XF_TNAME(DEPTH,NPC) > & _src_mat_B,
		hls::stream< XF_TNAME(DEPTH,NPC) > & _out_mat,
		XF_TNAME(DEPTH,NPC) buf_A[WIN_SZ][(COLS >> XF_BITSHIFT(NPC))], XF_PTUNAME(DEPTH) src_buf_A[WIN_SZ][XF_NPIXPERCYCLE(NPC)+(WIN_SZ-1)],
		XF_TNAME(DEPTH,NPC) buf_B[WIN_SZ][(COLS >> XF_BITSHIFT(NPC))], XF_PTUNAME(DEPTH) src_buf_B[WIN_SZ][XF_NPIXPERCYCLE(NPC)+(WIN_SZ-1)],
		XF_PTUNAME(DEPTH) OutputValues[XF_NPIXPERCYCLE(NPC)],
		XF_PTUNAME(DEPTH) &P0, uint16_t img_width,  uint16_t img_height, uint16_t &shift_x,  ap_uint<13> row_ind[WIN_SZ], ap_uint<13> row, ap_uint<8> win_size)
{
#pragma HLS INLINE

	XF_TNAME(DEPTH,NPC) buf_cop_A[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable=buf_cop_A complete dim=1

	XF_TNAME(DEPTH,NPC) buf_cop_B[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable=buf_cop_B complete dim=1

	uint16_t npc = XF_NPIXPERCYCLE(NPC);
	Col_Loop:
	for(ap_uint<13> col = 0; col < img_width+(WIN_SZ>>1); col++)
	{
#pragma HLS LOOP_FLATTEN OFF
#pragma HLS LOOP_TRIPCOUNT min=1 max=TC
#pragma HLS pipeline
		if(row < img_height && col < img_width)
		{
			buf_A[row_ind[win_size-1]][col] = _src_mat_A.read(); // Read data
			buf_B[row_ind[win_size-1]][col] = _src_mat_B.read(); // Read data
		}
		else
		{
			buf_A[row_ind[win_size-1]][col] = 0;
			buf_B[row_ind[win_size-1]][col] = 0;
		}

		for(int copy_buf_var=0;copy_buf_var<WIN_SZ;copy_buf_var++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=WIN_SZ
#pragma HLS UNROLL
			if(	(row >(img_height-1)) && (copy_buf_var>(win_size-1-(row-(img_height-1)))))
			{
				buf_cop_A[copy_buf_var] = buf_A[(row_ind[win_size-1-(row-(img_height-1))])][col];
				buf_cop_B[copy_buf_var] = buf_B[(row_ind[win_size-1-(row-(img_height-1))])][col];
			}
			else
			{
				buf_cop_A[copy_buf_var] = buf_A[(row_ind[copy_buf_var])][col];
				buf_cop_B[copy_buf_var] = buf_B[(row_ind[copy_buf_var])][col];
			}

		}
		for(int extract_px=0;extract_px<WIN_SZ;extract_px++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=WIN_SZ
#pragma HLS UNROLL
			if(col<img_width)
			{
				src_buf_A[extract_px][win_size-1] = buf_cop_A[extract_px];
				src_buf_B[extract_px][win_size-1] = buf_cop_B[extract_px];
			}
			else
			{
				src_buf_A[extract_px][win_size-1] = src_buf_A[extract_px][win_size-2];
				src_buf_B[extract_px][win_size-1] = src_buf_B[extract_px][win_size-2];
			}
		}

		xFPyrDownApplykernel<NPC, DEPTH, WIN_SZ, WIN_SZ_SQ,PLANES>(OutputValues,src_buf_A,src_buf_B, win_size);
		if(col >= (win_size>>1))
		{
			_out_mat.write(OutputValues[0]);
		}

		for(int wrap_buf=0;wrap_buf<WIN_SZ;wrap_buf++)
		{
	#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT min=1 max=WIN_SZ
			for(int col_warp=0; col_warp<WIN_SZ-1;col_warp++)
			{
	#pragma HLS UNROLL
#pragma HLS LOOP_TRIPCOUNT min=1 max=WIN_SZ
				if(col==0)
				{
					src_buf_A[wrap_buf][col_warp] = src_buf_A[wrap_buf][win_size-1];
					src_buf_B[wrap_buf][col_warp] = src_buf_B[wrap_buf][win_size-1];
				}
				else
				{
					src_buf_A[wrap_buf][col_warp] = src_buf_A[wrap_buf][col_warp+1];
					src_buf_B[wrap_buf][col_warp] = src_buf_B[wrap_buf][col_warp+1];
				}
			}
		}
	} // Col_Loop
}

template<int ROWS, int COLS, int DEPTH, int NPC, int WORDWIDTH, int TC,int WIN_SZ, int WIN_SZ_SQ,int PLANES>
void xf_pyrdown_gaussian_nxn(
		hls::stream< XF_TNAME(DEPTH,NPC) > &_src_mat_A,
		hls::stream< XF_TNAME(DEPTH,NPC) > &_src_mat_B,
		hls::stream< XF_TNAME(DEPTH,NPC) > &_out_mat, ap_uint<8> win_size,
		uint16_t img_height, uint16_t img_width)
{
	ap_uint<13> row_ind[WIN_SZ];
#pragma HLS ARRAY_PARTITION variable=row_ind complete dim=1

	ap_uint<8> buf_size = XF_NPIXPERCYCLE(NPC) + (WIN_SZ-1);
	uint16_t shift_x = 0;
	ap_uint<13> row, col;

	XF_TNAME(DEPTH,NPC) OutputValues[XF_NPIXPERCYCLE(NPC)];
#pragma HLS ARRAY_PARTITION variable=OutputValues complete dim=1


	XF_PTUNAME(DEPTH) src_buf_A[WIN_SZ][XF_NPIXPERCYCLE(NPC)+(WIN_SZ-1)];
#pragma HLS ARRAY_PARTITION variable=src_buf_A complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf_A complete dim=2
	XF_PTUNAME(DEPTH) src_buf_B[WIN_SZ][XF_NPIXPERCYCLE(NPC)+(WIN_SZ-1)];
#pragma HLS ARRAY_PARTITION variable=src_buf_B complete dim=1
#pragma HLS ARRAY_PARTITION variable=src_buf_B complete dim=2
// src_buf1 et al merged
	XF_TNAME(DEPTH,NPC) P0;

	XF_TNAME(DEPTH,NPC) buf_A[WIN_SZ][(COLS >> XF_BITSHIFT(NPC))];
#pragma HLS ARRAY_PARTITION variable=buf_A complete dim=1
#pragma HLS RESOURCE variable=buf_A core=RAM_S2P_BRAM

	XF_TNAME(DEPTH,NPC) buf_B[WIN_SZ][(COLS >> XF_BITSHIFT(NPC))];
#pragma HLS ARRAY_PARTITION variable=buf_B complete dim=1
#pragma HLS RESOURCE variable=buf_B core=RAM_S2P_BRAM

//initializing row index

	for(int init_row_ind=0; init_row_ind<win_size; init_row_ind++)
	{
	#pragma HLS LOOP_TRIPCOUNT min=1 max=WIN_SZ
		row_ind[init_row_ind] = init_row_ind;
	}

	read_lines:
	for(int init_buf=row_ind[win_size>>1]; init_buf <row_ind[win_size-1] ;init_buf++)
	{
	#pragma HLS LOOP_TRIPCOUNT min=1 max=WIN_SZ
		for(col = 0; col < img_width; col++)
		{
	#pragma HLS LOOP_TRIPCOUNT min=1 max=TC
	#pragma HLS LOOP_FLATTEN OFF
	#pragma HLS pipeline
			buf_A[init_buf][col] = _src_mat_A.read();
			buf_B[init_buf][col] = _src_mat_B.read();
		}
	}

	//takes care of top borders
		for(col = 0; col < img_width; col++)
		{
	#pragma HLS LOOP_TRIPCOUNT min=1 max=TC
			for(int init_buf=0; init_buf < WIN_SZ>>1;init_buf++)
			{
	#pragma HLS LOOP_TRIPCOUNT min=1 max=WIN_SZ
	#pragma HLS UNROLL
				buf_A[init_buf][col] = buf_A[row_ind[win_size>>1]][col];
				buf_B[init_buf][col] = buf_B[row_ind[win_size>>1]][col];
			}
		}

	Row_Loop:
	for(row = (win_size>>1); row < img_height+(win_size>>1); row++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=ROWS
		P0 = 0;
		xFPyrDownprocessgaussian<ROWS, COLS, DEPTH, NPC, WORDWIDTH, TC, WIN_SZ, WIN_SZ_SQ,PLANES>(_src_mat_A, _src_mat_B, _out_mat, buf_A, src_buf_A, buf_B, src_buf_B, OutputValues, P0, img_width, img_height, shift_x, row_ind, row,win_size);

		//update indices
		ap_uint<13> zero_ind = row_ind[0];
		for(int init_row_ind=0; init_row_ind<WIN_SZ-1; init_row_ind++)
		{
	#pragma HLS LOOP_TRIPCOUNT min=1 max=WIN_SZ
	#pragma HLS UNROLL
			row_ind[init_row_ind] = row_ind[init_row_ind + 1];
		}
		row_ind[win_size-1] = zero_ind;

	} // Row_Loop
}

template<int ROWS,int COLS,int DEPTH,int NPC,int WORDWIDTH,int PIPELINEFLAG, int WIN_SZ, int WIN_SZ_SQ,int PLANES>
void xFPyrDownGaussianBlur(
		hls::stream< XF_TNAME(DEPTH,NPC) > &_src_A,
		hls::stream< XF_TNAME(DEPTH,NPC) > &_src_B,
		hls::stream< XF_TNAME(DEPTH,NPC) > &_dst, ap_uint<8> win_size,
		int _border_type,uint16_t imgheight,uint16_t imgwidth)
{
	assert(((imgheight <= ROWS ) && (imgwidth <= COLS)) && "ROWS and COLS should be greater than input image");

	assert( (win_size <= WIN_SZ) && "win_size must not be greater than WIN_SZ");

	imgwidth = imgwidth >> XF_BITSHIFT(NPC);

	xf_pyrdown_gaussian_nxn<ROWS,COLS,DEPTH,NPC,WORDWIDTH,(COLS>>XF_BITSHIFT(NPC))+(WIN_SZ>>1),WIN_SZ, WIN_SZ_SQ,PLANES>(_src_A,_src_B, _dst,WIN_SZ,imgheight,imgwidth);

}


template<int ROWS, int COLS>
void blendOperator(
		hls::stream< ap_uint<8> > &_srcA,
		hls::stream< ap_uint<8> > &_srcB,
		hls::stream< ap_uint<8> > &_dst,
		uint16_t img_height, uint16_t img_width)
{
#pragma HLS INLINE
	for (int i = 0; i < img_height; i++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=ROWS
		for (int j = 0; j < img_width; j++) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS
#pragma HLS PIPELINE II=1
			ap_uint<8> res = (_srcA.read() + _srcB.read()) >> 1;
			_dst.write(res);
		}
	}
}

// 优化版图像融合算法
template<int ROWS, int COLS>
void blendOpt(xf::Mat<_TYPE, ROWS, COLS, _NPC1>& imageA,
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& imageB,
		xf::Mat<_TYPE, ROWS, COLS, _NPC1>& imageS) {
#pragma HLS DATAFLOW
	hls::stream< ap_uint<8> > _imageA_in;
	hls::stream< ap_uint<8> > _imageB_in;
	hls::stream< ap_uint<8> > _imageS_out;
	unsigned int read_pointer = 0;
	for(int i=0; i < imageA.rows; i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=ROWS
		for(int j=0; j < imageA.cols; j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS
#pragma HLS PIPELINE II=1
			_imageA_in.write(imageA.read(read_pointer));
			_imageB_in.write(imageB.read(read_pointer));
			read_pointer++;
		}
	}
	blendOperator<ROWS, COLS>(_imageA_in, _imageB_in, _imageS_out, imageA.rows, imageA.cols);

	unsigned int write_ptr = 0;
	for(int i=0;i<imageA.rows;i++)
	{
#pragma HLS LOOP_TRIPCOUNT min=1 max=ROWS
		for(int j=0;j<imageA.cols;j++)
		{
#pragma HLS LOOP_TRIPCOUNT min=1 max=COLS
#pragma HLS PIPELINE II=1
			ap_uint<8> read_fil_out = _imageS_out.read();
			imageS.write(write_ptr,read_fil_out);
			write_ptr++;
		}
	}
	return;
}

}
#endif
