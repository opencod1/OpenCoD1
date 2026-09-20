/*
 * @fidelity: likely
 * @fidelity-default: unreviewed
 */

#include "../qcommon/qcommon.h"
#include "../qcommon/hexrays_shim.h"
#include "../qcommon/cod1_globals.h"
#include "tr_gl_types.h"
#include "tr_tess.h"
#include "tr_smcache.h"

extern int Hunk_AllocAlignInternal();
extern int R_CreateBufferARB();

smcCache_t r_smcCache;

/* ---- SMC_FreeCachedSurface_r  0x004E4EE0 ----  VERIFIED */
int __cdecl SMC_FreeCachedSurface_r(int a1, int a2, int a3, int a4)
{
  int v5;
  char v6;
  int v7;
  int v8;
  _DWORD *v9;
  int v10;
  int result;
  int v12;
  int v13;
  int v14;

  v5 = a3;
  if ( *(_WORD *)(a2 + 4 * a3 + 12) )
  {
    while ( 1 )
    {
      v6 = *(_BYTE *)(a2 + 4 * v5 + 14);
      *(_WORD *)(a2 + 4 * v5 + 12) = 0;
      if ( v6 )
        break;
      v7 = a4 - 1;
      SMC_FreeCachedSurface_r(a1, a2, 2 * v5 + 1, a4 - 1);
      v5 = 2 * v5 + 2;
      a4 = v7;
      if ( !*(_WORD *)(a2 + 4 * v5 + 12) )
        goto LABEL_4;
    }
    v12 = (v5 + 1) << a4;
    v13 = *(_DWORD *)(a2 + 20 * v12 - 168);
    v14 = a2 + 20 * v12 - 184;
    *(_DWORD *)(v13 + 4 * *(_DWORD *)(v14 + 12) + 584) = 0;
    *(_DWORD *)(a1 + 58416) += -1 << (a4 + 5);
    result = *(_DWORD *)(v14 + 4);
    *(_DWORD *)(a1 + 58420) -= *(unsigned __int16 *)(result + 18);
    *(_BYTE *)(a2 + 4 * v5 + 14) = 0;
  }
  else
  {
LABEL_4:
    v8 = 5 * ((v5 + 1) << a4);
    v9 = (_DWORD *)(a2 + 4 * v8 - 184);
    **(_DWORD **)(a2 + 4 * v8 - 180) = *v9;
    v10 = *v9;
    result = v9[1];
    *(_DWORD *)(v10 + 4) = result;
  }
  return result;
}

/* ---- SMC_ForceFreeBlock  0x004E4FA0 ----  VERIFIED */
int __cdecl SMC_ForceFreeBlock(int a1)
{
  int v1;

  v1 = *(_DWORD *)(a1 + 58408);
  if ( *(_DWORD *)(v1 + 8) == tr_frameCount )
    return 0;
  SMC_FreeCachedSurface_r(a1, v1, 0, 4);
  **(_DWORD **)(v1 + 4) = *(_DWORD *)v1;
  *(_DWORD *)(*(_DWORD *)v1 + 4) = *(_DWORD *)(v1 + 4);
  *(_DWORD *)(v1 + 136) = a1 + 58368;
  *(_DWORD *)(v1 + 140) = *(_DWORD *)(a1 + 58372);
  *(_DWORD *)(a1 + 58372) = v1 + 136;
  **(_DWORD **)(v1 + 140) = v1 + 136;
  return 1;
}

/* ---- SMC_GetFreeBlockOfSize  0x004E5010 ----  VERIFIED */
int __cdecl SMC_GetFreeBlockOfSize(int a1, int a2)
{
  int result;
  char *v3;
  char *v4;
  int v5;
  _DWORD *v6;
  int *v7;

  if ( !a2 )
    return SMC_ForceFreeBlock(a1);
  if ( *(_DWORD *)(a1 + 8 * a2 + 58364) != a1 + 8 * a2 + 58360 || (result = SMC_GetFreeBlockOfSize(a1, a2 - 1)) != 0 )
  {
    v3 = *(char **)(a1 + 8 * a2 + 58364);
    **((_DWORD **)v3 + 1) = *(_DWORD *)v3;
    *(_DWORD *)(*(_DWORD *)v3 + 4) = *((_DWORD *)v3 + 1);
    v4 = (char *)(a1 + 456 * ((int)&v3[-a1] / 456));
    if ( a2 == 1 )
    {
      *(_DWORD *)v4 = a1 + 58408;
      *((_DWORD *)v4 + 1) = *(_DWORD *)(a1 + 58412);
      *(_DWORD *)(a1 + 58412) = v4;
      **((_DWORD **)v4 + 1) = v4;
    }
    v5 = a1 + 8 * a2 + 58368;
    v6 = (_DWORD *)(a1 + 8 * a2 + 58372);
    *(_DWORD *)v3 = v5;
    *((_DWORD *)v3 + 1) = *v6;
    *(_DWORD *)(v5 + 4) = v3;
    **((_DWORD **)v3 + 1) = v3;
    v7 = (int *)&v4[20 * ((v3 - v4 - 136) / 20) + 136 + 20 * (1 << (4 - a2))];
    *v7 = v5;
    v7[1] = *v6;
    *(_DWORD *)(v5 + 4) = v7;
    **(_DWORD **)&v4[20 * ((v3 - v4 - 136) / 20) + 140 + 20 * (1 << (4 - a2))] = v7;
    return 1;
  }
  return result;
}

/* ---- SMC_Allocate  0x004E5120 ----  VERIFIED */
char *__cdecl SMC_Allocate(int a1, int a2)
{
  int v2;
  char *result;
  char *v4;
  char *v5;
  int v6;
  int v7;
  int i;
  char *v9;

  v2 = 9 - a2;
  if ( *(_DWORD *)(a1 + 8 * (9 - a2) + 58372) != a1 + 8 * (9 - a2) + 58368
    || (result = (char *)SMC_GetFreeBlockOfSize(a1, 9 - a2)) != 0 )
  {
    v4 = *(char **)(a1 + 8 * v2 + 58372);
    **((_DWORD **)v4 + 1) = *(_DWORD *)v4;
    *(_DWORD *)(*(_DWORD *)v4 + 4) = *((_DWORD *)v4 + 1);
    v5 = (char *)(a1 + 456 * ((int)&v4[-a1] / 456));
    if ( !v2 )
    {
      *(_DWORD *)v5 = a1 + 58408;
      *((_DWORD *)v5 + 1) = *(_DWORD *)(a1 + 58412);
      *(_DWORD *)(a1 + 58412) = v5;
      **((_DWORD **)v5 + 1) = v5;
    }
    v6 = (v4 - v5 - 136) / 20;
    v7 = (v6 + 16) >> (4 - v2);
    v5[4 * v7 + 10] = 1;
    for ( i = v7 - 1; i >= 0; i = (i - 1) >> 1 )
      *(_WORD *)&v5[4 * i + 12] += 1 << a2;
    v9 = &v5[20 * v6];
    *((_DWORD *)v9 + 36) = 32 * (v6 + 16 * ((int)&v4[-a1] / 456));
    return v9 + 136;
  }
  return result;
}

/* ---- R_EvaluateLightingAtPoint  0x004E5210 ----  VERIFIED */
int __cdecl R_EvaluateLightingAtPoint(int a1, float *a2, float *a3, _BYTE *a4)
{
  int result;
  int v5;
  float *v6;
  double v7;
  float *v8;
  double v9;
  double v10;
  double v11;
  int v12;
  bool v13; // cc
  int v14;
  double v15;
  int v16;
  double v17;
  int v18;
  double v19;
  float v20;
  float v21;
  float v22;
  float v23;
  float v24;
  float v25;
  float v26;
  float v27;
  int v28;
  int v29;
  float v30;
  float v31;
  float v32[3]; // [ebp-0x1c] BYREF  -- retail vec3_t
  float v35;
  float v36;
  float v37;
  float v38;

  if ( *(_DWORD *)(tr_world + 280) )
  {
    v28 = 0;
    v35 = *(float *)(tr_world + 248) * *(float *)(a1 + 272) + *(float *)(tr_world + 216);
    v36 = *(float *)(tr_world + 252) * *(float *)(a1 + 272) + *(float *)(tr_world + 220);
    v37 = *(float *)(tr_world + 256) * *(float *)(a1 + 272) + *(float *)(tr_world + 224);
    v5 = *(_DWORD *)(a1 + 204);
    v38 = *(float *)(tr_world + 260) * *(float *)(a1 + 272) + *(float *)(tr_world + 228);
    if ( v5 > 0 )
    {
      v6 = (float *)(a1 + 212);
      v29 = a1 + 212;
      do
      {
        v7 = *v6;
        v8 = (float *)*((_DWORD *)v6 - 1);
        v21 = *v6;
        if ( (v8[20] == 0.0) | __UNORDERED__(v8[20], 0.0) )
        {
          v9 = v8[19] * a3[2] + v8[17] * *a3 + v8[18] * a3[1];
          v20 = v9;
          if ( (v9 < 0.0) | __UNORDERED__(v9, 0.0) )
            v20 = 0.0;
        }
        else
        {
          v32[0] = v8[17] - *a2;
          v32[1] = v8[18] - a2[1];
          v32[2] = v8[19] - a2[2];
          v10 = VectorNormalize(v32);
          v11 = v32[2] * a3[2] + v32[1] * a3[1] + v32[0] * *a3;
          v20 = v11;
          if ( (v11 < 0.0) | __UNORDERED__(v11, 0.0) )
            v20 = 0.0;
          v7 = v21 / (v10 * (v10 * v8[26] + v8[25]) + v8[24]);
          v22 = v7;
          if ( *((_DWORD *)v8 + 28) != 1127481344 )
          {
            v27 = v32[2] * v8[23] + v32[1] * v8[22] + v32[0] * v8[21];
            v30 = (v8[28] + 90.0) * 2.8444445;
            if ( v27 > (double)tr_sinTable[(int)(v30 + 9.313225746154785e-10) & 0x3FF] )
            {
              v12 = 0;
              if ( (v8[27] > 0.0) | __UNORDERED__(0.0, v8[27]) )
              {
                v7 = v22;
                do
                {
                  v7 = v7 * v27;
                  ++v12;
                }
                while ( ((double)v12 < v8[27]) | __UNORDERED__((double)v12, v8[27]) );
              }
              else
              {
                v7 = v22;
              }
            }
            else
            {
              v7 = 0.0;
            }
          }
        }
        v6 = (float *)(v29 + 8);
        v13 = ++v28 < *(_DWORD *)(a1 + 204);
        v29 += 8;
        v35 = (v20 * v8[9] + v8[5]) * v7 + v35;
        v36 = (v20 * v8[10] + v8[6]) * v7 + v36;
        v37 = (v20 * v8[11] + v8[7]) * v7 + v37;
        v38 = (v20 * v8[12] + v8[8]) * v7 + v38;
      }
      while ( v13 );
    }
    v23 = v35 * 255.0;
    if ( (int)(v23 + 9.313225746154785e-10) < 256 )
    {
      if ( (int)(v23 + 9.313225746154785e-10) >= 0 )
        v14 = (int)(v23 + 9.313225746154785e-10);
      else
        LOBYTE(v14) = 0;
    }
    else
    {
      LOBYTE(v14) = -1;
    }
    v15 = v36 * 255.0;
    *a4 = v14;
    v24 = v15;
    v31 = v15;
    if ( (int)(v31 + 9.313225746154785e-10) < 0 || (int)(v24 + 9.313225746154785e-10) <= 255 )
    {
      if ( (int)(v24 + 9.313225746154785e-10) >= 0 )
        v16 = (int)(v24 + 9.313225746154785e-10);
      else
        LOBYTE(v16) = 0;
    }
    else
    {
      LOBYTE(v16) = -1;
    }
    v17 = v37;
    a4[1] = v16;
    v25 = v17 * 255.0;
    if ( (int)(v25 + 9.313225746154785e-10) < 256 )
    {
      if ( (int)(v25 + 9.313225746154785e-10) >= 0 )
        v18 = (int)(v25 + 9.313225746154785e-10);
      else
        LOBYTE(v18) = 0;
    }
    else
    {
      LOBYTE(v18) = -1;
    }
    v19 = v38;
    a4[2] = v18;
    v26 = v19 * 255.0;
    if ( (int)(v26 + 9.313225746154785e-10) < 256 )
    {
      if ( (int)(v26 + 9.313225746154785e-10) >= 0 )
      {
        result = (int)(v26 + 9.313225746154785e-10);
        a4[3] = result;
      }
      else
      {
        result = 0;
        a4[3] = 0;
      }
    }
    else
    {
      result = 255;
      a4[3] = -1;
    }
  }
  else
  {
    result = (int)a4;
    *a4 = tr_identityLightByte;
    a4[1] = tr_identityLightByte;
    a4[2] = tr_identityLightByte;
    a4[3] = -1;
  }
  return result;
}

/* ---- R_CacheStaticModelSurface  0x004E57E0 ----  VERIFIED */
char *__cdecl R_CacheStaticModelSurface(float *a1, int a2, int a3, int a4, float *a5)
{
  unsigned __int16 v7;
  int v8;
  char *v9;
  int v11;
  int v12;
  int v13;
  int v14;
  int v15;
  double v16;
  float *v17;
  int v18;
  float *v19;
  int v20;
  double v21;
  float *v22;
  bool v23; // cc
  int v24;
  _DWORD *v25;
  int v26;
  int v27;
  int *v28;
  int v29;
  int v30;
  _DWORD *v31;
  int v32;
  int v33;
  _DWORD *v34;
  int v35;
  int v36;
  char v37;
  int v38;
  _BYTE *v39;
  int v40;
  _BYTE *v41;
  unsigned int v42;
  char *v43;
  char v44;
  char *v45;
  char *v46;
  char v47;
  unsigned int v48;
  char *v49;
  char v50;
  char *v51;
  int v52;
  char v53;
  float *i;
  int v55;
  char *v56;
  float v57[3]; // [esp+24h] [ebp-Ch] BYREF

  if ( !r_smc_enable->integer )
    return 0;
  v7 = *(_WORD *)(a2 + 18);
  if ( v7 > 0x200u )
    return 0;
  v8 = 5;
  v53 = 5;
  if ( v7 > 0x20u )
  {
    do
      ++v8;
    while ( v7 > 1 << v8 );
    v53 = v8;
  }
  v9 = SMC_Allocate((int)&r_smcCache, v8);
  v56 = v9;
  if ( !v9 )
    return 0;
  dword_11DA660 += 1 << v53;
  dword_11DA664 += *(unsigned __int16 *)(a2 + 18);
  *(_DWORD *)v9 = tr_cachedStaticModelSurfaceType;
  *((_DWORD *)v9 + 4) = a1;
  *((_DWORD *)v9 + 1) = a2;
  *((_DWORD *)v9 + 3) = a3;
  v11 = ri_Hunk_AllocateTempMemory(24 * *(unsigned __int16 *)(a2 + 18));
  v12 = 0;
  v51 = (char *)v11;
  if ( *(_WORD *)(a2 + 18) )
  {
    v13 = 0;
    v14 = v11 + 12;
    do
    {
      *(_DWORD *)(v14 - 12) = *(_DWORD *)(*(_DWORD *)(a2 + 20) + 8 * v12);
      *(_DWORD *)(v14 - 8) = *(_DWORD *)(*(_DWORD *)(a2 + 20) + 8 * v12 + 4);
      v14 += 24;
      *(float *)(v14 - 24) = *(float *)(*(_DWORD *)(a2 + 28) + v13 + 8) * a1[13]
                           + *(float *)(v13 + *(_DWORD *)(a2 + 28) + 4) * a1[10]
                           + *(float *)(v13 + *(_DWORD *)(a2 + 28)) * a1[7]
                           + a1[17];
      *(float *)(v14 - 20) = *(float *)(*(_DWORD *)(a2 + 28) + v13 + 8) * a1[14]
                           + *(float *)(v13 + *(_DWORD *)(a2 + 28) + 4) * a1[11]
                           + *(float *)(v13 + *(_DWORD *)(a2 + 28)) * a1[8]
                           + a1[18];
      v15 = *(_DWORD *)(a2 + 28);
      v16 = *(float *)(v15 + v13 + 8);
      v17 = (float *)(v13 + v15);
      ++v12;
      v13 += 12;
      *(float *)(v14 - 16) = v16 * a1[15] + v17[1] * a1[12] + *v17 * a1[9] + a1[19];
    }
    while ( v12 < *(unsigned __int16 *)(a2 + 18) );
    v9 = v56;
    v11 = (int)v51;
  }
  v18 = *(_DWORD *)(*(_DWORD *)(a2 + 12) + 340);
  switch ( *(_DWORD *)(v18 + 1636) )
  {
    case 1:
      v26 = 0;
      v27 = 16843009 * tr_identityLightByte;
      if ( *(_WORD *)(a2 + 18) )
      {
        v28 = (int *)(v51 + 8);
        do
        {
          *v28 = v27;
          ++v26;
          v28 += 6;
        }
        while ( v26 < *(unsigned __int16 *)(a2 + 18) );
      }
      break;
    case 2:
      v24 = 0;
      if ( *(_WORD *)(a2 + 18) )
      {
        v25 = v51 + 8;
        do
        {
          *v25 = -1;
          ++v24;
          v25 += 6;
        }
        while ( v24 < *(unsigned __int16 *)(a2 + 18) );
      }
      break;
    case 0xA:
      v55 = 0;
      if ( *(_WORD *)(a2 + 18) )
      {
        v19 = (float *)(v11 + 12);
        v52 = 0;
        for ( i = (float *)(v11 + 12); ; v19 = i )
        {
          v20 = *(_DWORD *)(a2 + 24);
          v21 = *(float *)(v20 + v52 + 4);
          v22 = (float *)(v52 + v20);
          v57[0] = v21 * a5[1] + *v22 * *a5 + a5[2] * v22[2];
          v57[1] = a5[5] * v22[2] + *v22 * a5[3] + v22[1] * a5[4];
          v57[2] = *v22 * a5[6] + a5[8] * v22[2] + v22[1] * a5[7];
          VectorNormalize(v57);
          R_EvaluateLightingAtPoint(a4, v19, v57, (_BYTE *)v19 - 4);
          v23 = ++v55 < *(unsigned __int16 *)(a2 + 18);
          v52 += 12;
          i += 6;
          if ( !v23 )
            break;
        }
        v9 = v56;
      }
      break;
    case 0xB:
      v32 = *(_DWORD *)(a4 + 108);
      v33 = 0;
      if ( *(_WORD *)(a2 + 18) )
      {
        v34 = v51 + 8;
        do
        {
          *v34 = v32;
          ++v33;
          v34 += 6;
        }
        while ( v33 < *(unsigned __int16 *)(a2 + 18) );
      }
      break;
    case 0xC:
      v29 = *(_DWORD *)(v18 + 1664);
      v30 = 0;
      if ( *(_WORD *)(a2 + 18) )
      {
        v31 = v51 + 8;
        do
        {
          *v31 = v29;
          ++v30;
          v31 += 6;
        }
        while ( v30 < *(unsigned __int16 *)(a2 + 18) );
      }
      break;
    default:
      break;
  }
  v35 = *(_DWORD *)(*(_DWORD *)(a2 + 12) + 340);
  v36 = *(_DWORD *)(v35 + 1660);
  if ( v36 )
  {
    if ( v36 == 9 && *(_DWORD *)(v35 + 1636) != 12 )
    {
      v37 = *(_BYTE *)(v35 + 1667);
      v38 = 0;
      if ( *(_WORD *)(a2 + 18) )
      {
        v39 = v51 + 11;
        do
        {
          *v39 = v37;
          ++v38;
          v39 += 24;
        }
        while ( v38 < *(unsigned __int16 *)(a2 + 18) );
      }
    }
  }
  else if ( *(_DWORD *)(v35 + 1636) != 2 )
  {
    v40 = 0;
    if ( *(_WORD *)(a2 + 18) )
    {
      v41 = v51 + 11;
      do
      {
        *v41 = -1;
        ++v40;
        v41 += 24;
      }
      while ( v40 < *(unsigned __int16 *)(a2 + 18) );
    }
  }
  switch ( tr_cachedStaticModelSurfaceType )
  {
    case 12:
      v42 = 24 * *(unsigned __int16 *)(a2 + 18);
      v43 = (char *)(tr_cachedStaticModelStorage + 24 * *((_DWORD *)v9 + 2));
      v44 = v42;
      v42 >>= 2;
      qmemcpy(v43, v51, 4 * v42);
      v46 = &v51[4 * v42];
      v45 = &v43[4 * v42];
      v47 = v44;
      goto LABEL_49;
    case 13:
      qglBindBufferARB(34962, tr_cachedStaticModelStorage);
      qglBufferSubDataARB(34962, 24 * *((_DWORD *)v9 + 2), 24 * *(unsigned __int16 *)(a2 + 18), v51);
      qglBindBufferARB(34962, 0);
      break;
    case 14:
      qglUpdateObjectBufferATI(
        tr_cachedStaticModelStorage,
        tr_cachedStaticModelStorageOffset + 24 * *((_DWORD *)v9 + 2),
        24 * *(unsigned __int16 *)(a2 + 18),
        v51,
        34658);
      break;
    case 15:
      v48 = 24 * *(unsigned __int16 *)(a2 + 18);
      v49 = (char *)(tr_cachedStaticModelStorage + 24 * *((_DWORD *)v9 + 2));
      v50 = v48;
      v48 >>= 2;
      qmemcpy(v49, v51, 4 * v48);
      v46 = &v51[4 * v48];
      v45 = &v49[4 * v48];
      v47 = v50;
LABEL_49:
      qmemcpy(v45, v46, v47 & 3);
      break;
    default:
      break;
  }
  ri_Hunk_FreeTempMemory(v51);
  return v9;
}

/* ---- R_UsedCachedStaticModelSurface  0x004E5C90 ----  VERIFIED */
_DWORD *__cdecl R_UsedCachedStaticModelSurface(int a1)
{
  unsigned int v1;
  _DWORD *result;
  _DWORD *v3;

  v1 = (int)((unsigned __int64)(2411209711i64 * (a1 - (int)&r_smcCache)) >> 32) >> 8;
  result = (_DWORD *)((char *)&r_smcCache + 456 * (v1 >> 31) + 456 * v1);
  v3 = (_DWORD *)result[1];
  result[2] = tr_frameCount;
  *v3 = *result;
  *(_DWORD *)(*result + 4) = result[1];
  *result = (_DWORD)&dword_11DA658;
  result[1] = dword_11DA65C;
  dword_11DA65C = (int)result;
  *(_DWORD *)result[1] = (_DWORD)result;
  return result;
}

/* ---- R_AllocStaticModelCacheGeneric  0x004E5CF0 ----  VERIFIED */
int R_AllocStaticModelCacheGeneric()
{
  int result;

  result = ri_Hunk_Alloc(1572864);
  tr_cachedStaticModelStorage = result;
  tr_cachedStaticModelSurfaceType = 12;
  return result;
}

/* ---- R_AllocStaticModelCacheARB  0x004E5D10 ----  VERIFIED */
int __cdecl R_AllocStaticModelCacheARB(void *this)
{
  int result;

  result = R_CreateBufferARB((int)this, 34962, 1572864, 0, tr_vboUsage);
  tr_cachedStaticModelStorage = result;
  if ( result )
  {
    tr_cachedStaticModelStorageSource = storageClass;
    tr_cachedStaticModelSurfaceType = 13;
  }
  else
  {
    result = ri_Hunk_Alloc(1572864);
    tr_cachedStaticModelStorage = result;
    tr_cachedStaticModelSurfaceType = 12;
  }
  return result;
}

/* ---- R_AllocStaticModelCacheATI  0x004E5D70 ----  VERIFIED */
int R_AllocStaticModelCacheATI()
{
  int v0;
  int result;

  v0 = tr_staticVertexMemoryPrimaryUsed;
  tr_cachedStaticModelStorage = 0;
  if ( tr_staticVertexMemoryPrimaryUsed + 1572864 > tr_staticVertexMemoryPrimaryLimit )
  {
    result = tr_staticVertexMemorySecondaryUsed + 1572864;
    if ( tr_staticVertexMemorySecondaryUsed + 1572864 > tr_staticVertexMemorySecondaryLimit )
    {
      tr_cachedStaticModelStorageSource = 0;
      ri_Printf(0, "^3Couldn't allocate memory for the static model surface cache using ATI optimizations");
      result = ri_Hunk_Alloc(1572864);
      tr_cachedStaticModelSurfaceType = 12;
      tr_cachedStaticModelStorage = result;
    }
    else
    {
      tr_cachedStaticModelStorageOffset = tr_staticVertexMemorySecondaryUsed;
      tr_staticVertexMemorySecondaryUsed += 1572864;
      tr_cachedStaticModelStorageSource = 3;
      tr_cachedStaticModelStorage = tr_staticVertexMemorySecondary;
      tr_cachedStaticModelSurfaceType = 14;
    }
  }
  else
  {
    tr_staticVertexMemoryPrimaryUsed += 1572864;
    result = tr_staticVertexMemoryPrimary;
    tr_cachedStaticModelStorageOffset = v0;
    tr_cachedStaticModelStorageSource = 3;
    tr_cachedStaticModelSurfaceType = 14;
    tr_cachedStaticModelStorage = tr_staticVertexMemoryPrimary;
  }
  return result;
}

/* ---- R_AllocStaticModelCacheNV  0x004E5E30 ----  VERIFIED */
int R_AllocStaticModelCacheNV()
{
  int v0;
  int result;
  int v2;

  v0 = tr_staticVertexMemoryPrimaryUsed;
  tr_cachedStaticModelStorage = 0;
  if ( tr_staticVertexMemoryPrimaryUsed + 1572864 >= tr_staticVertexMemoryPrimaryLimit )
  {
    v2 = tr_staticVertexMemorySecondaryUsed;
    if ( tr_staticVertexMemorySecondaryUsed + 1572864 >= tr_staticVertexMemorySecondaryLimit )
    {
      tr_cachedStaticModelStorage = (int)Hunk_AllocAlignInternal(0x180000u, 32);
      result = 3;
      tr_cachedStaticModelStorageSource = 3;
    }
    else
    {
      tr_staticVertexMemorySecondaryUsed += 1572864;
      result = 2;
      tr_cachedStaticModelStorage = v2 + tr_staticVertexMemorySecondary;
      tr_cachedStaticModelStorageSource = 2;
    }
    tr_cachedStaticModelSurfaceType = 15;
  }
  else
  {
    tr_staticVertexMemoryPrimaryUsed += 1572864;
    result = 1;
    tr_cachedStaticModelStorage = v0 + tr_staticVertexMemoryPrimary;
    tr_cachedStaticModelStorageSource = 1;
    tr_cachedStaticModelSurfaceType = 15;
  }
  return result;
}

/* ---- R_InitStaticModelCache  0x004E5EE0 ----  VERIFIED */
_DWORD *__cdecl R_InitStaticModelCache(void *this)
{
  int i;
  _DWORD *result;

  if ( glConfig_ARBVertexBufferObject )
  {
    R_AllocStaticModelCacheARB(this);
  }
  else if ( glConfig_NVVertexArrayRange )
  {
    R_AllocStaticModelCacheNV();
  }
  else if ( glConfig_ATIVertexArrayObject )
  {
    R_AllocStaticModelCacheATI();
  }
  else
  {
    tr_cachedStaticModelStorage = ri_Hunk_Alloc(1572864);
    tr_cachedStaticModelSurfaceType = 12;
  }
  memset(&r_smcCache, 0, 0xE438u);
  dword_11DA658 = (int)&dword_11DA658;
  dword_11DA65C = (int)&dword_11DA658;
  dword_11DA638 = (int)&dword_11DA638;
  dword_11DA63C = (int)&dword_11DA638;
  dword_11DA640 = (int)&dword_11DA640;
  dword_11DA644 = (int)&dword_11DA640;
  dword_11DA648 = (int)&dword_11DA648;
  dword_11DA64C = (int)&dword_11DA648;
  dword_11DA650 = (int)&dword_11DA650;
  dword_11DA654 = (int)&dword_11DA650;
  dword_11DA630 = (int)&dword_11DA630;
  dword_11DA634 = (int)&dword_11DA630;
  for ( i = 0; i < 128; ++i )
  {
    result = (_DWORD *)&r_smcCache.pages[i].surfaces[0];
    result[0] = (_DWORD)&dword_11DA630;
    result[1] = dword_11DA634;
    dword_11DA634 = (int)result;
    *(_DWORD *)result[1] = (_DWORD)result;
  }
  return (_DWORD *)((char *)&r_smcCache.pages[127].surfaces[0] + 456);
}

/* ---- R_StaticModelCacheStats_f  0x004E6100 ----  VERIFIED */
void R_StaticModelCacheStats_f()
{
  ri_Printf(0, "%.2f%% of cache is currently allocated.\n", (double)dword_11DA660 * 0.00152587890625);
  if ( dword_11DA660 )
    ri_Printf(0, "%.2f%% allocated cache vertices are used.\n", (double)dword_11DA664 * 100.0 / (double)dword_11DA660);
}

/* ---- R_StaticModelCacheFlush_f  0x004E6160 ----  VERIFIED */
int R_StaticModelCacheFlush_f()
{
  int i;
  int result = 0;  /* retail sets no return value */

  for ( i = dword_11DA65C; (int *)dword_11DA65C != &dword_11DA658; i = dword_11DA65C )
  {
    SMC_FreeCachedSurface_r((int)&r_smcCache, i, 0, 4);
    **(_DWORD **)(dword_11DA65C + 4) = *(_DWORD *)dword_11DA65C;
    *(_DWORD *)(*(_DWORD *)dword_11DA65C + 4) = *(_DWORD *)(dword_11DA65C + 4);
    result = i + 136;
    *(_DWORD *)(i + 136) = (_DWORD)&dword_11DA630;
    *(_DWORD *)(i + 140) = dword_11DA634;
    dword_11DA634 = i + 136;
    **(_DWORD **)(i + 140) = i + 136;
  }
  return result;
}

