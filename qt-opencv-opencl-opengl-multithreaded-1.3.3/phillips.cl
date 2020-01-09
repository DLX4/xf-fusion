/**
 *   @file  phillips.cl
 *
 *   @brief
 *      This file includes OpenCL wave surface simulation 
 *      kernels that are dispatched to C66x cores
 *
 *  \par
 *  ============================================================================
 *  @n Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/ 
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  \par
*/

void DSPF_sp_fftSPxSP(int N, local float *x, local float *w, local float *y, unsigned char *brev, int n_min, int offset, int n_max);

#define NXY 256
kernel void ocl_DSPF_sp_fftSPxSP(int N, global float *x, global float *w, global float *y, int n_min, int offset, int n_max)
{
  local float xloc[2*NXY];
  local float yloc[2*NXY];
  local float wloc[2*NXY];
  int i;
  for(i = 0; i < 2*N; i ++)
  {
     xloc[i] = x[i];
     yloc[i] = y[i];
     wloc[i] = w[i];
  }
  DSPF_sp_fftSPxSP (N, xloc, wloc, yloc, 0, n_min, offset, n_max);
  for(i = 0; i < 2*N; i ++)
  {
      x[i] = xloc[i];
      y[i] = yloc[i];
  }
}

void DSPF_sp_ifft2D_FIXEDSIZE_SPxSP(global float2 *c, global float *twiddles)
{
   local float input[2*NXY];
   local float output[2*NXY];
   local float twloc[2*NXY];
   int i,j;
   int nx, ny, idx;

   for(int i = 0; i < 2*NXY; i ++) twloc[i] = twiddles[i];
   /* Transform the rows */
   for (j=0;j<NXY;j++)
   {
      for (i=0;i<NXY;i++)
      {
         idx = j * NXY + i;
         input[2 * i + 0] = c[idx].x;
         input[2 * i + 1] = -c[idx].y;
      }
      DSPF_sp_fftSPxSP (NXY, input, twloc, output, 0, 4, 0, NXY);
      for (i=0;i<NXY;i++)
      {
         idx = j * NXY + i;
         c[idx].x =  output[2*i+0] / (float)(NXY/NXY);
         c[idx].y = -output[2*i+1] / (float)(NXY/NXY);
      }
   }

   /* Transform the columns (transpose included) */
   for (i=0;i<NXY;i++)
   {
      for (j=0;j<NXY;j++)
      {
         idx = j * NXY + i;
         input[2*j+0] =  c[idx].x;
         input[2*j+1] = -c[idx].y;
      }
      DSPF_sp_fftSPxSP (NXY, input, twloc, output, 0, 4, 0, NXY);
      for (j=0;j<NXY;j++)
      {
         idx = j * NXY + i;
         c[idx].x =  output[2*j+0] / (float)(NXY);
         c[idx].y = -output[2*j+1] / (float)(NXY);
      }
   }
}

kernel void ocl_DSPF_sp_ifft2D_FIXEDSIZE_SPxSP(global float2 *spect, global float *tw)
{
   DSPF_sp_ifft2D_FIXEDSIZE_SPxSP(spect, tw);
}

/* Update spectrum in time, to simulate movement */
// complex math functions
float2 __attribute__((always_inline)) conjugate(float2 arg)
{
  return (float2)(arg.x, -arg.y);
}

float2 __attribute__((always_inline)) complex_mult(float2 arg1, float2 arg2)
{
  return (float2)(arg1.x * arg2.x - arg1.y * arg2.y, arg1.x * arg2.y + arg1.y * arg2.x);
}

float2 __attribute__((always_inline)) complex_add(float2 arg1, float2 arg2)
{
  return (float2)(arg1.x + arg2.x, arg1.y + arg2.y);
}

float2 __attribute__((always_inline)) complex_exp(float arg)
{
float s;
float c;
  s = sincos(arg, &c);
  return (float2)(c,s);
}


#define OPENCL_PI_F 3.14159f
#define RAND_MAX 32768

float2 gausRnd(void)
{
float x1, x2, w, y1, y2;
float2 gausret;

    do {
      x1 = 2.0f * rand() / (float)RAND_MAX - 1.0f;
      x2 = 2.0f * rand() / (float)RAND_MAX - 1.0f;
      w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0f );

    w = sqrt( (-2.0f * log( w ) ) / w );
    gausret.x = x1 * w;
    gausret.y = x2 * w;
    return gausret;
}

float phillips (float kx, float ky, float windSpeed, float windDir, float A)
{
  float fWindDir = windDir * OPENCL_PI_F / 180.0f;
  float k_length = sqrt (kx * kx + ky * ky);
  if(k_length < 0.000001f) return 0.0f;
  float k_length2 = k_length * k_length;
  float wind_x = cosf(fWindDir);
  float wind_y = sinf(fWindDir);
  kx /= k_length; // normalize
  ky /= k_length; // normalize
  float wind_lenth = sqrt(wind_x * wind_x + wind_y * wind_y);
  float k_length4 = k_length2 * k_length2;
  float k_dot_w = kx * wind_x + ky * wind_y;
  float k_dot_w2 = k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w;
  float L = windSpeed * windSpeed / 9.81f;
  float L2 = L * L;
  return A * pow(2.7183f, -1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * pow(2.7183f, -k_length2 * 0.000001f * L2);
}

kernel void ocl_genHMap(global float2 * h0, global float2 *h0_spectrum, global float *twiddles, unsigned int patchSize, float windSpeed, float windDir)
{
float2 fMultiplier;

  for (unsigned int y = 0; y<NXY; y++)
  {
    for (unsigned int x = 0; x<NXY; x++)
    {
      float kx = OPENCL_PI_F * x / (float) patchSize;
      float ky = 2.0f * OPENCL_PI_F * y / (float) patchSize;
      int i = y*NXY+x;

      if (!((kx == 0.f) && (ky == 0.f))) {
        fMultiplier = sqrt(phillips(kx,ky, windSpeed, windDir, 0.5f));
      } else {
        fMultiplier = 0.f;
      }
      h0_spectrum[i] = h0[i] = fMultiplier * gausRnd();
    }
  }
  DSPF_sp_ifft2D_FIXEDSIZE_SPxSP(h0, twiddles);

}

kernel void ocl_updateHMap( global float *twiddles, global float2* h0, global float2* ht, float t, unsigned int patchSize)
{
  for(unsigned int y = 0; y < NXY; y ++)
  {
    for(unsigned int x = 0; x < NXY; x ++)
    {
      int i = NXY*y+x;
      float2 k;
      k.x = OPENCL_PI_F * x / (float) patchSize;
      k.y = 2.0f * OPENCL_PI_F * y / (float) patchSize;
      // calculate dispersion w(k)
      float k_len = sqrtf(k.x*k.x + k.y*k.y);
      float w = sqrtf(9.81f * k_len);
      float omegat = w * t;
      float2 h0_k  = h0[i];
      float2 h0_mk = h0[(((NXY-1)-y)*NXY) + (NXY-1-x)];
      float2 h_tilda = complex_mult( h0_k, complex_exp(omegat)) +  complex_mult(conjugate(h0_mk), complex_exp(-omegat));
      // output frequency-space complex values
      ht[i] = h_tilda;
    }
  }
  DSPF_sp_ifft2D_FIXEDSIZE_SPxSP(ht, twiddles);
}
/* nothing past this point */
