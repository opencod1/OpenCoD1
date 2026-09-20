/*
 * @fidelity: likely
 * @fidelity-default: unreviewed
 */

#include "../qcommon/qcommon.h"
#include "../qcommon/hexrays_shim.h"
#include "../qcommon/cod1_globals.h"
#include "tr_gl_types.h"
#include "tr_tess.h"

extern int GL_Bind();
extern int R_AllocImage();
extern int R_CreateImageInternal();
extern int R_FreeImage();


typedef struct { unsigned char b[52]; } waterMap_t;
static waterMap_t s_waterMaps[16];

/* 0x011EE0B0.  256 { cos, sin } pairs, step 2*pi/256 == 0.024543693. */
static float s_fftTwiddle[512];

#define dword_11EE0B0  ((int *)s_fftTwiddle)
#define dword_11EE0B4  (s_fftTwiddle + 1)
#define unk_11EDD70    (*(unsigned char *)s_waterMaps)
#define dword_11EDD78  ((int *)((char *)s_waterMaps + 8))
#define dword_11EDD7C  ((int *)((char *)s_waterMaps + 12))

/* ---- FFT_Init  0x00506EE0 ----  VERIFIED */
float *FFT_Init()
{
  int v0;
  float *v1;
  float *result;
  int v3;
  float v4;

  v0 = 0;
  v3 = 0;
  v1 = dword_11EE0B4;
  do
  {
    s_fftBitReverse[v0] = 0;
    if ( (v0 & 1) != 0 )
      s_fftBitReverse[v0] = 128;
    if ( (v0 & 2) != 0 )
      s_fftBitReverse[v0] |= 0x40u;
    if ( (v0 & 4) != 0 )
      s_fftBitReverse[v0] |= 0x20u;
    if ( (v0 & 8) != 0 )
      s_fftBitReverse[v0] |= 0x10u;
    if ( (v0 & 0x10) != 0 )
      s_fftBitReverse[v0] |= 8u;
    if ( (v0 & 0x20) != 0 )
      s_fftBitReverse[v0] |= 4u;
    if ( (v0 & 0x40) != 0 )
      s_fftBitReverse[v0] |= 2u;
    if ( (char)v0 < 0 )
      s_fftBitReverse[v0] |= 1u;
    v4 = (double)v3 * 0.024543693289160728;
    *(v1 - 1) = cos(v4);
    *v1 = sin(v4);
    result = v1 - 1;
    ++v0;
    v1 += 2;
    v3 = v0;
  }
  while ( v0 < 256 );
  return result;
}

/* ---- FFT  0x00506FB0 ----  [HIGH] */
void __cdecl FFT(int bits, int *a1, int a2)
{
  char v2;
  int v3;
  int v4;
  int *v5;
  int v6;
  int v7;
  int *v8;
  float *v9;
  unsigned int v10;
  float *v11;
  double v12;
  double v13;
  double v14;
  double v15;
  double v16;
  double v17;
  double v18;
  double v19;
  double v20;
  double v21;
  double v22; // rt0
  double v23;
  signed int v24;
  int result;
  int v26;
  signed int v27;
  unsigned int v28;
  int v29;
  int v30;
  float *v31;
  double v32;
  double v33;
  bool v34; // cc
  float v35;
  int v36;
  float v37;
  unsigned int v38;
  unsigned int v39;
  int v40;
  int v41;
  int v42;
  int v43;
  int v44;
  char v45;
  char v46;

  v3 = 0;
  v2 = (char)bits;
  v45 = 8 - v2;
  v4 = a2;
  v44 = 1 << v2;
  v41 = 0;
  if ( 1 << v2 > 0 )
  {
    v5 = a1;
    do
    {
      v6 = s_fftBitReverse[v3] >> v45;
      if ( v6 < v3 )
      {
        v7 = *v5;
        v8 = &a1[2 * a2 * v6];
        v36 = v5[1];
        *v5 = *v8;
        v5[1] = v8[1];
        v3 = v41;
        *v8 = v7;
        v8[1] = v36;
      }
      v5 += 2 * a2;
      v41 = ++v3;
    }
    while ( v3 < v44 );
  }
  v9 = (float *)a1;
  v10 = (unsigned int)v44 >> 2;
  v11 = (float *)&a1[2 * a2];
  do
  {
    v12 = v9[1];
    v13 = v9[4 * a2];
    v14 = v9[4 * a2 + 1];
    v15 = *v9;
    v16 = v15 + *v11;
    v17 = v13 + v11[4 * a2];
    *v9 = v16 + v17;
    v9[4 * a2] = v16 - v17;
    v18 = v9[4 * a2 + 1] + v11[4 * a2 + 1];
    v19 = v9[1] + v11[1];
    v9[1] = v19 + v18;
    v9[4 * a2 + 1] = v19 - v18;
    v20 = v11[4 * a2 + 1] - v14;
    v21 = *v11 - v15;
    *v11 = v21 - v20;
    v22 = v11[4 * a2] - v13;
    v11[4 * a2] = v20 + v21;
    v23 = v11[1] - v12;
    v11[1] = v22 + v23;
    v11[4 * a2 + 1] = v23 - v22;
    --v10;
    v9 += 8 * a2;
    v11 += 8 * a2;
  }
  while ( v10 );
  v24 = v44;
  result = 4;
  v43 = 4;
  v46 = 5;
  if ( v44 > 4 )
  {
    while ( 1 )
    {
      v26 = 0;
      v27 = 2 * result;
      v39 = 2 * result;
      v40 = 0;
      if ( result > 0 )
      {
        v28 = v4 * v24;
        result = 0;
        v38 = v28;
        v42 = 0;
        do
        {
          v29 = v26 << v46;
          v35 = *(float *)&s_fftTwiddle[2 * v29];
          v37 = dword_11EE0B4[2 * v29];
          if ( result < v28 )
          {
            v30 = v4 * v39;
            v31 = (float *)&a1[2 * v42 + 2 * v4 * v43];
            v28 = v38;
            do
            {
              v32 = v35 * *v31 - v37 * v31[1];
              v33 = v35 * v31[1] + v37 * *v31;
              *v31 = *(float *)&a1[2 * result] - v32;
              v31[1] = *(float *)&a1[2 * result + 1] - v33;
              v31 += 2 * v30;
              *(float *)&a1[2 * result] = v32 + *(float *)&a1[2 * result];
              *(float *)&a1[2 * result + 1] = v33 + *(float *)&a1[2 * result + 1];
              result += v30;
            }
            while ( result < v38 );
            v4 = a2;
            result = v42;
          }
          v26 = v40 + 1;
          result += v4;
          v34 = ++v40 < v43;
          v42 = result;
        }
        while ( v34 );
        v27 = v39;
        v24 = v44;
      }
      v43 = v27;
      --v46;
      if ( v27 >= v24 )
        break;
      result = v27;
    }
  }
}

/* ---- GaussianRandom  0x005071D0 ----  VERIFIED */
float *__cdecl GaussianRandom(float *a1, float *a2)
{
  int v2;
  double v3;
  long double v4;
  float *result;
  long double v6;
  float v7;
  int v8;

  do
  {
    v2 = rand();
    v7 = (double)v2 * 0.000030517578125 + (double)v2 * 0.000030517578125 - 1.0;
    v8 = rand();
    v3 = (double)v8 * 0.000030517578125 + (double)v8 * 0.000030517578125 - 1.0;
    v4 = v3 * v3 + v7 * v7;
  }
  while ( v4 > 1.0 );
  result = a1;
  v6 = sqrt(__FYL2X__(v4, 0.6931471805599453094) * -2.0 / v4);
  *a1 = v7 * v6;
  *a2 = v6 * v3;
  return result;
}

/* ---- R_WaterExp  no-address ----  [HIGH] */
static double R_WaterExp( double x )
{
	double r;

	__asm
	{
		fld     x
		fldl2e
		fmulp   st(1), st
		fld     st
		frndint
		fxch    st(1)
		fsub    st, st(1)
		f2xm1
		fld1
		faddp   st(1), st
		fscale
		fstp    st(1)
		fstp    r
	}
	return r;
}

/* ---- PickWaterFrequencies  0x00507270 ----  VERIFIED */
int __cdecl PickWaterFrequencies(int a1)
{
  int v1;
  int v2; // kr00_4
  int result;
  int v4;
  int v5;
  double v6;
  double v7;
  double v11;
  int j;
  float v13;
  float v14;
  int v15;
  float v16;
  float i;
  float v18;
  float v19;
  float v20;
  float v21; // [esp+28h] [ebp-8h] BYREF
  float v22; // [esp+2Ch] [ebp-4h] BYREF

  v1 = *(_DWORD *)(a1 + 16);
  v2 = *(_DWORD *)(a1 + 20);
  result = v2 / 2;
  v4 = 0;
  v19 = *(float *)(a1 + 36) * *(float *)(a1 + 36) / *(float *)(a1 + 32) * (*(float *)(a1 + 36) * *(float *)(a1 + 36));
  v16 = 6.2831854820251465 / ((double)v2 * *(float *)(a1 + 24));
  v15 = v2 / -2;
  for ( i = 6.2831854820251465 / ((double)v1 * *(float *)(a1 + 28)); v15 < result; ++v15 )
  {
    v14 = (double)v15 * v16;
    v5 = v1 / -2;
    for ( j = v1 / -2; v5 < v1 / 2; j = v5 )
    {
      v13 = (double)j * i;
      GaussianRandom(&v21, &v22);
      v18 = v14 * v14;
      v6 = v13 * v13 + v18;
      v20 = sqrt(v6) * *(float *)(a1 + 32);
      v7 = v13 * *(float *)(a1 + 44) + v14 * *(float *)(a1 + 40);
      if ( v7 > 0.0 )
      {
        v11 = sqrt(R_WaterExp(-1.0 / (v6 * v19))
                   * *(float *)(a1 + 48) / (v6 * v6 * v6) * (v7 * v7) * 0.5)
            * *(float *)(a1 + 48);
        *(float *)(*(_DWORD *)(a1 + 8) + 8 * v4) = v21 * v11;
        *(float *)(*(_DWORD *)(a1 + 8) + 8 * v4 + 4) = v22 * v11;
        *(float *)(*(_DWORD *)(a1 + 12) + 4 * v4) = sqrt(v20);
      }
      else
      {
        *(_DWORD *)(*(_DWORD *)(a1 + 8) + 8 * v4) = 0;
        *(_DWORD *)(*(_DWORD *)(a1 + 8) + 8 * v4 + 4) = 0;
        *(_DWORD *)(*(_DWORD *)(a1 + 12) + 4 * v4) = 0;
      }
      v1 = *(_DWORD *)(a1 + 16);
      ++v4;
      ++v5;
    }
    result = *(_DWORD *)(a1 + 20) / 2;
  }
  return result;
}

/* ---- WaterFrequenciesAtTime  0x00507430 ----  VERIFIED */
int __cdecl WaterFrequenciesAtTime(int a1, _DWORD *a2, float a3)
{
  int result;
  int v4;
  int j;
  float *v6;
  int v7;
  int i;
  float v9;

  result = (int)a2[5] / 2;
  v4 = 0;
  for ( i = (int)a2[5] / -2; i < result; ++i )
  {
    for ( j = (int)a2[4] / -2; j < (int)a2[4] / 2; ++j )
    {
      v6 = (float *)(a2[3] + 4 * v4);
      if ( (*v6 == 0.0) | __UNORDERED__(*v6, 0.0) )
      {
        *(_DWORD *)(a1 + 8 * v4) = 0;
        *(_DWORD *)(a1 + 8 * v4 + 4) = 0;
      }
      else
      {
        v9 = a3 * *v6 * 162.9746551513671875;
        ftol_tempSpill = (int)v9;
        v7 = (int)v9 & 0x3FF;
        *(float *)(a1 + 8 * v4) = tr_sinTable[((_WORD)v7 + 255) & 0x3FF] * *(float *)(a2[2] + 8 * v4);
        *(float *)(a1 + 8 * v4 + 4) = tr_sinTable[v7] * *(float *)(a2[2] + 8 * v4 + 4);
      }
      ++v4;
    }
    result = (int)a2[5] / 2;
  }
  return result;
}

/* ---- WaterAmplitudesFromFrequencies  0x00507520 ----  VERIFIED */
int __cdecl WaterAmplitudesFromFrequencies(int a1, int *a2)
{
  int v2;
  int v3;
  int v4;
  int v5;
  int v6;
  int result;
  int v8;
  int *v9;

  v2 = *(_DWORD *)(a1 + 20);
  v3 = 0;
  if ( v2 != 1 )
  {
    do
      ++v3;
    while ( v2 != 1 << v3 );
  }
  v4 = *(_DWORD *)(a1 + 16);
  v5 = 0;
  if ( v4 != 1 )
  {
    do
      ++v5;
    while ( v4 != 1 << v5 );
  }
  v6 = 0;
  if ( v2 > 0 )
  {
    do
    {
      /* v5 == log2(cols).  0x00507570. */
      FFT(v5, &a2[2 * v6 * *(_DWORD *)(a1 + 16)], 1);
      ++v6;
    }
    while ( v6 < *(_DWORD *)(a1 + 20) );
  }
  result = *(_DWORD *)(a1 + 16);
  v8 = 0;
  if ( result > 0 )
  {
    v9 = a2;
    do
    {
      /* v3 == log2(rows).  0x00507592. */
      FFT(v3, v9, result);
      result = *(_DWORD *)(a1 + 16);
      ++v8;
      v9 += 2;
    }
    while ( v8 < result );
  }
  return result;
}

/* ---- WaterNormalsFromAmplitudes  0x005075B0 ----  VERIFIED */
int __cdecl WaterNormalsFromAmplitudes(_DWORD *a1, int a2, int a3)
{
  int v3;
  int v4;
  double v5;
  double v6;
  int v7;
  int result;
  int v9;
  _DWORD *v10;
  float *v11;
  float *v12;
  float *v13;
  double v14;
  double v15;
  long double v16; // rtt
  int v17;
  int v18;
  int v19;
  float i;
  int j;
  int v22;
  int v23;
  int v24;
  int v25;

  v3 = *(_DWORD *)(a3 + 16) * *(_DWORD *)(a3 + 20);
  v19 = *(_DWORD *)(a3 + 16);
  v4 = 0;
  v23 = v3;
  for ( i = (float)(v3 * v3); v4 < v3; *(float *)(a2 + 8 * v4 - 8) = sqrt(v5 * v5 + v6 * v6) )
  {
    v5 = *(float *)(a2 + 8 * v4++ + 4);
    v6 = *(float *)(a2 + 8 * v4 - 8);
  }
  v18 = v3 - 1;
  v7 = *(_DWORD *)(a3 + 16);
  result = v3 - v7;
  v22 = result;
  v25 = 0;
  v24 = v7 - 1;
  for ( j = 0; j < *(_DWORD *)(a3 + 20); ++j )
  {
    v9 = v19;
    v10 = a1;
    v11 = (float *)(a2 + 8 * v24);
    v12 = (float *)(a2 + 8 * v22);
    v13 = (float *)(a2 + 8 * v25);
    do
    {
      v14 = *v11 - *v13;
      v15 = *v12 - *v13;
      v11 = v13;
      v12 += 2;
      v13 += 2;
      v16 = 127.0 / sqrt(v15 * v15 + v14 * v14 + i);
      *v10++ = (int)(v14 * v16) + (((int)(v15 * v16) + (((int)((double)v23 * v16) + 65407) << 8) + 127) << 8) + 127;
      --v9;
    }
    while ( v9 );
    a1 = v10;
    v17 = *(_DWORD *)(a3 + 16);
    v25 += v17;
    v24 += v17;
    v22 = v18 & (v23 + v25 - v17);
    result = j + 1;
  }
  return result;
}

/* ---- WatersEquivalent  0x005076F0 ----  VERIFIED */
BOOL __fastcall WatersEquivalent(int a1, int a2)
{
  float adx, ady, bdx, bdy;
  double dot, mag;

  if ( *(_DWORD *)(a2 + 16) != *(_DWORD *)(a1 + 16) )
    return 0;
  if ( *(_DWORD *)(a2 + 20) != *(_DWORD *)(a1 + 20) )
    return 0;
  if ( !((*(float *)(a2 + 24) == *(float *)(a1 + 24))
         | __UNORDERED__(*(float *)(a2 + 24), *(float *)(a1 + 24))) )
    return 0;
  if ( !((*(float *)(a2 + 28) == *(float *)(a1 + 28))
         | __UNORDERED__(*(float *)(a2 + 28), *(float *)(a1 + 28))) )
    return 0;
  if ( !(fabs(*(float *)(a2 + 48) - *(float *)(a1 + 48)) <= 0.0010000000474974513) )
    return 0;
  if ( !(fabs(*(float *)(a2 + 32) - *(float *)(a1 + 32)) <= 0.1) )
    return 0;
  if ( !(fabs(*(float *)(a2 + 36) - *(float *)(a1 + 36)) <= 0.1) )
    return 0;

  /* 0x0050776E-0x005077C7 */
  adx = *(float *)(a1 + 40);
  ady = *(float *)(a1 + 44);
  bdx = *(float *)(a2 + 40);
  bdy = *(float *)(a2 + 44);
  dot = adx * bdx + ady * bdy;
  mag = sqrt((double)(adx * adx + ady * ady) * (double)(bdx * bdx + bdy * bdy));
  if ( !(fabs(dot / (mag + 1e-10) - 1.0) <= 0.0010000000474974513) )
    return 0;
  return 1;
}

/* ---- R_InitWater  0x005077E0 ----  VERIFIED */
float *R_InitWater()
{
  s_numWaterMaps = 0;
  return FFT_Init();
}

/* ---- R_GetWaterTexture  0x005077F0 ----  VERIFIED */
int *__cdecl R_GetWaterTexture(_DWORD *a1)
{
  int v1;
  int v2;
  void *v4;
  int v6;
  int v7;
  void *v8;
  int v9;
  int v10;
  char *v11;
  int v12;
  int v14;
  int *v15;
  int v16;

  v1 = s_numWaterMaps;
  v2 = 0;
  v14 = 0;
  if ( s_numWaterMaps > 0 )
  {
    v4 = &s_waterMaps;
    do
    {
      if ( WatersEquivalent((int)a1, (int)v4) )
        return (int *)((char *)&s_waterMaps + 52 * v2);
      ++v2;
      v4 = (char *)v4 + 52;   /* 0x00507820  add edx, 34h */
    }
    while ( v2 < v1 );
    v14 = v2;
    if ( v2 == 16 )
      ri_Error(1, "\x15" "map uses more than %i waterMap textures", 16);
  }
  v6 = a1[4] * a1[5];
  v16 = 13 * v2;
  v15 = (int *)((char *)&s_waterMaps + 52 * v2);
  qmemcpy(v15, a1, 0x34u);
  v7 = ri_Hunk_Alloc(12 * v6);
  dword_11EDD78[v16] = v7;
  dword_11EDD7C[v16] = v7 + 8 * v6;
  PickWaterFrequencies((int)v15);
  v8 = (void *)ri_Hunk_AllocateTempMemory(4 * a1[4] * a1[5]);
  memset(v8, 0xFFu, 4 * a1[4] * a1[5]);
  v9 = a1[5];
  v10 = a1[4];
  v11 = va("*water%i", v14);
  v12 = R_AllocImage(v11, 3553, v10, v9, 0, 8);
  if ( !R_CreateImageInternal(v12, 0, (unsigned __int8 *)v8, 0xDE1u, 6408) )
  {
    R_FreeImage(v12);
    v12 = 0;
  }
  *v15 = v12;
  ri_Hunk_FreeTempMemory(v8);
  if ( !*v15 )
    return 0;
  ++s_numWaterMaps;
  return v15;
}

/* ---- RB_UploadWaterTexture  0x00507950 ----  VERIFIED */
void __cdecl RB_UploadWaterTexture(_DWORD *water, int a1, int a2)
{
  _DWORD *v3;
  int v4;
  float v5;
  _DWORD pixels[65536]; // [esp+14h] [ebp-C0004h] BYREF
  int v7[131073]; // [esp+40014h] [ebp-80004h] BYREF
  unsigned int retaddr;

  v3 = water;
  v4 = water[1];
  v7[0x20000] = retaddr ^ _security_cookie;
  if ( v4 != tr_frameCount )
  {
    v3[1] = tr_frameCount;
    v5 = (double)a1 * 0.0010000000474974513;
    WaterFrequenciesAtTime((int)v7, v3, v5);
    WaterAmplitudesFromFrequencies((int)v3, v7);
    WaterNormalsFromAmplitudes(pixels, (int)v7, (int)v3);
    if ( glState_currentTmu != a2 )
    {
      qglActiveTextureARB(a2 + 33984);
      glState_currentTmu = a2;
    }
    if ( glState_currentClientTmu != a2 )
    {
      qglClientActiveTextureARB(a2 + 33984);
      glState_currentClientTmu = a2;
    }
    GL_Bind((GLenum *)*v3);
    glTexSubImage2D(
      0xDE1u,
      0,
      0,
      0,
      *(unsigned __int16 *)(*v3 + 64),
      *(unsigned __int16 *)(*v3 + 66),
      0x1908u,
      0x1401u,
      pixels);
  }
}
