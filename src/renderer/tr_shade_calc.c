/*
 * @fidelity: likely
 * @fidelity-default: unreviewed
 */

#include "../qcommon/qcommon.h"
#include "../qcommon/hexrays_shim.h"
#include "../qcommon/cod1_globals.h"
#include "tr_records.h"
#include "tr_gl_types.h"
#include "tr_orientation.h"   /* backEnd.or 0x016D8DA8 -- one object, not 16 shards */
#include "tr_tess.h"

float rbSpecularLightOrigin = -960.0f;   /* 0x00571130 */
float flt_571134           =  1980.0f;   /* 0x00571134 */
float flt_571138           =    96.0f;   /* 0x00571138 */

int __cdecl RB_AddQuadStampExt( float *origin, float *left, float *up, int *color, float s1, float t1, float s2, float t2 );
extern int RB_ProjectionShadowDeform();
void VectorNormalizeFast( float *v );

float *__cdecl RB_CalcTransformTexCoords( float *tcMod, float *texCoords );

#define tess_numVertexes             tess_numVertexes
#define tess_shaderTime              tess_shaderTime
#define tess_vertexComponentCount    tess_vertexComponentCount
#define tess_xyz_base                ((float *)tess_xyz)
#define backEnd_currentEntity        backEnd_currentEntity
#define tr_sinTable                  tr_sinTable
#define myftol_tmp                   ftol_tempSpill
#define FUNCTABLE_SIZE               1024
#define FUNCTABLE_MASK               1023

/* ---- TableForFunc  0x005133D0 ----  VERIFIED */
float *__cdecl TableForFunc(void *func)
{
  switch ( (unsigned int)func )
  {
    case 1u:
      return tr_sinTable;
    case 2u:
      return tr_squareTable;
    case 3u:
      return tr_triangleTable;
    case 4u:
      return tr_sawToothTable;
    case 5u:
      return tr_inverseSawToothTable;
    default:
      break;
  }
  ri_Error(1,"\x15" "TableForFunc called with invalid function '%d' in shader '%s'\n", func, tess_shader);
  return NULL;
}

/* ---- EvalWaveForm  0x00513430 ----  VERIFIED */
double __cdecl EvalWaveForm(int a1)
{
  float *v1;
  float v3;

  v1 = TableForFunc(*(void **)a1);
  v3 = (*(float *)(a1 + 16) * tess_shaderTime + *(float *)(a1 + 12)) * 1024.0;
  ftol_tempSpill = (int)v3;
  return *(float *)(a1 + 8) * v1[(int)v3 & 0x3FF] + *(float *)(a1 + 4);
}

/* ---- EvalWaveFormClamped  0x00513470 ----  VERIFIED */
double __cdecl EvalWaveFormClamped(int a1)
{
  float *v1;
  double result;
  float v3;

  v1 = TableForFunc(*(void **)a1);
  v3 = (*(float *)(a1 + 16) * tess_shaderTime + *(float *)(a1 + 12)) * 1024.0;
  ftol_tempSpill = (int)v3;
  result = *(float *)(a1 + 8) * v1[(int)v3 & 0x3FF] + *(float *)(a1 + 4);
  if ( (result < 0.0) | __UNORDERED__(result, 0.0) )
    return 0.0;
  if ( result > 1.0 )
    return 1.0;
  return result;
}

/* ---- RB_CalcStretchTexCoords  0x005134E0 ----  VERIFIED */
float *__cdecl RB_CalcStretchTexCoords(int a1, float *a2)
{
  float *v2;
  double v3;
  double v4;
  float v6;
  float v7[12]; // [esp+4h] [ebp-44h] BYREF -- twelve floats

  v2 = TableForFunc(*(void **)a1);
  v6 = (*(float *)(a1 + 16) * tess_shaderTime + *(float *)(a1 + 12)) * 1024.0;
  ftol_tempSpill = (int)v6;
  v3 = *(float *)(a1 + 8) * v2[ftol_tempSpill & 0x3FF] + *(float *)(a1 + 4);
  v7[8] = 0.0;
  v7[7] = 0.0;
  v4 = 1.0 / v3;
  v7[6] = v4;
  v7[10] = 0.5 - 0.5 * v4;
  v7[9] = v4;
  v7[11] = v7[10];
  return RB_CalcTransformTexCoords(v7, a2);
}

/* ---- RB_CalcDeformVertexes  0x00513570 ----  VERIFIED */
float *__cdecl RB_CalcDeformVertexes(int a1)
{
  void *v1;
  float *v2;
  float *v3;
  double v4;
  float *result;
  int v6;
  double v7;
  double v8; // rt0
  float *v9;
  int v10;
  float *v11;
  double v12;
  float v13;
  float v14;
  float v15;
  float v16;
  float v17;
  float v18;

  v1 = *(void **)(a1 + 16);
  v2 = (float *)tess_xyz;
  if ( (*(float *)(a1 + 32) == 0.0) | __UNORDERED__(*(float *)(a1 + 32), 0.0) )
  {
    v3 = TableForFunc(v1);
    v13 = (*(float *)(a1 + 32) * tess_shaderTime + *(float *)(a1 + 28)) * 1024.0;
    ftol_tempSpill = (int)v13;
    v4 = *(float *)(a1 + 24) * v3[(int)v13 & 0x3FF];
    result = (float *)tess_numVertexes;
    v6 = 0;
    v14 = v4 + *(float *)(a1 + 20);
    if ( tess_numVertexes > 0 )
    {
      result = dword_1827F68;
      do
      {
        ++v6;
        v7 = v14 * *(result - 2);
        result += 3;
        v17 = v14 * *(result - 3);
        v8 = v14 * *(result - 4);
        *v2 = v7 + *v2;
        v2[1] = v8 + v2[1];
        v2[2] = v17 + v2[2];
        v2 += tess_vertexComponentCount;
      }
      while ( v6 < tess_numVertexes );
    }
  }
  else
  {
    v9 = TableForFunc(v1);
    result = (float *)tess_numVertexes;
    v10 = 0;
    if ( tess_numVertexes > 0 )
    {
      v11 = dword_1827F68;
      do
      {
        v15 = ((v2[2] + v2[1] + *v2) * *(float *)(a1 + 36) + *(float *)(a1 + 28) + tess_shaderTime * *(float *)(a1 + 32))
            * 1024.0;
        ftol_tempSpill = (int)v15;
        ++v10;
        v11 += 3;
        v12 = v9[(int)v15 & 0x3FF] * *(float *)(a1 + 24) + *(float *)(a1 + 20);
        v16 = v12 * *(v11 - 4);
        v18 = v12 * *(v11 - 3);
        *v2 = v12 * *(v11 - 5) + *v2;
        v2[1] = v16 + v2[1];
        v2[2] = v18 + v2[2];
        result = (float *)tess_vertexComponentCount;
        v2 += tess_vertexComponentCount;
      }
      while ( v10 < tess_numVertexes );
    }
  }
  return result;
}

/* ---- RB_CalcFlapVertexes  0x005136E0 ----  VERIFIED */
float *__cdecl RB_CalcFlapVertexes(int a1, int a2)
{
  void *v2;
  float *v3;
  float *v4;
  float *v5;
  double v6;
  float *result;
  int v8;
  double v9;
  float *v10;
  int v11;
  float *v12;
  double v13;
  float v14;
  float v15;
  float v16;
  float v17;
  float v18;
  float v19;
  float v20;

  v2 = *(void **)(a2 + 16);
  v3 = (float *)tess_xyz;
  v4 = (float *)(4 * a1 + ((int)(char *)tess_texCoords0));
  if ( (*(float *)(a2 + 32) == 0.0) | __UNORDERED__(*(float *)(a2 + 32), 0.0) )
  {
    v5 = TableForFunc(v2);
    v14 = (*(float *)(a2 + 32) * tess_shaderTime + *(float *)(a2 + 28)) * 1024.0;
    ftol_tempSpill = (int)v14;
    v6 = *(float *)(a2 + 24) * v5[(int)v14 & 0x3FF];
    result = (float *)tess_numVertexes;
    v8 = 0;
    v15 = v6 + *(float *)(a2 + 20);
    if ( tess_numVertexes > 0 )
    {
      result = dword_1827F68;
      do
      {
        ++v8;
        v9 = v15 * *v4;
        result += 3;
        v4 += 2;
        v17 = v9 * *(result - 4);
        v19 = v9 * *(result - 3);
        *v3 = v9 * *(result - 5) + *v3;
        v3[1] = v17 + v3[1];
        v3[2] = v19 + v3[2];
        v3 += tess_vertexComponentCount;
      }
      while ( v8 < tess_numVertexes );
    }
  }
  else
  {
    v10 = TableForFunc(v2);
    result = (float *)tess_numVertexes;
    v11 = 0;
    if ( tess_numVertexes > 0 )
    {
      v12 = dword_1827F68;
      do
      {
        v16 = ((*v3 + v3[2] + v3[1]) * *(float *)(a2 + 36) + *(float *)(a2 + 28) + tess_shaderTime * *(float *)(a2 + 32))
            * 1024.0;
        ftol_tempSpill = (int)v16;
        ++v11;
        v12 += 3;
        v4 += 2;
        v13 = (v10[(int)v16 & 0x3FF] * *(float *)(a2 + 24) + *(float *)(a2 + 20)) * *(v4 - 2);
        v18 = v13 * *(v12 - 4);
        v20 = v13 * *(v12 - 3);
        *v3 = v13 * *(v12 - 5) + *v3;
        v3[1] = v18 + v3[1];
        v3[2] = v20 + v3[2];
        result = (float *)tess_vertexComponentCount;
        v3 += tess_vertexComponentCount;
      }
      while ( v11 < tess_numVertexes );
    }
  }
  return result;
}

/* ---- RB_CalcDeformSyncNormals  0x00513870 ----  VERIFIED */
int __cdecl RB_CalcDeformSyncNormals(int a1)
{
  float *v1;
  float *v2;
  int result;
  float *v4;
  double v5;
  double v6;
  int v7;
  float v8;
  float v9;
  float v10;
  float v11;

  v1 = (float *)tess_xyz;
  v2 = TableForFunc(*(void **)(a1 + 16));
  result = 0;
  v7 = 0;
  if ( tess_numVertexes > 0 )
  {
    v4 = dword_1827F68;
    do
    {
      v5 = *(v4 - 1) + *v4 + *(v4 - 2);
      v9 = 1.0 - v5 * *(v4 - 2);
      v10 = 1.0 - v5 * *(v4 - 1);
      v11 = 1.0 - v5 * *v4;
      v8 = ((v1[2] + v1[1] + *v1) * *(float *)(a1 + 36) + *(float *)(a1 + 28) + tess_shaderTime * *(float *)(a1 + 32))
         * 1024.0;
      ftol_tempSpill = (int)v8;
      v6 = -((v2[((unsigned __int16)(int)v8 + 1) & 0x3FF] - v2[(int)v8 & 0x3FF])
           * *(float *)(a1 + 24)
           * 1024.0
           * *(float *)(a1 + 36));
      *(v4 - 2) = v9 * v6 + *(v4 - 2);
      *(v4 - 1) = v10 * v6 + *(v4 - 1);
      *v4 = v6 * v11 + *v4;
      VectorNormalizeFast(v4 - 2);
      v1 += tess_vertexComponentCount;
      v4 += 3;
      ++v7;
    }
    while ( v7 < tess_numVertexes );
    result = 0;
  }
  tess_stageTangentsValid = 0;
  tess_stageBitangentsValid = 0;
  return result;
}

/* ---- RB_CalcDeformNormals  0x00513990 ----  VERIFIED */
int __cdecl RB_CalcDeformNormals(int a1)
{
  int result;
  float *v5;
  float *v6;
  float x, y, z, t;
  float noise;
  int v22;

  result = 0;
  v5 = (float *)tess_xyz;
  v22 = 0;
  if ( tess_numVertexes > 0 )
  {
    v6 = dword_1827F68;
    do
    {
      t = tess_shaderTime * *(float *)(a1 + 32);
      z = v5[2] * 0.98000002f;
      y = v5[1] * 0.98000002f;
      x = *v5 * 0.98000002f;
      noise = 0.0f;
      noise = Com_NoiseGet4f(x, y, z, t);
      *(v6 - 2) = noise * *(float *)(a1 + 24) + *(v6 - 2);

      t = tess_shaderTime * *(float *)(a1 + 32);
      z = v5[2] * 0.98000002f;
      y = v5[1] * 0.98000002f;
      x = *v5 * 0.98000002f + 100.0f;
      noise = 0.0f;
      noise = Com_NoiseGet4f(x, y, z, t);
      *(v6 - 1) = noise * *(float *)(a1 + 24) + *(v6 - 1);

      t = tess_shaderTime * *(float *)(a1 + 32);
      z = v5[2] * 0.98000002f;
      y = v5[1] * 0.98000002f;
      x = *v5 * 0.98000002f + 200.0f;
      noise = 0.0f;
      noise = Com_NoiseGet4f(x, y, z, t);
      *v6 = noise * *(float *)(a1 + 24) + *v6;

      VectorNormalizeFast(v6 - 2);
      v5 += tess_vertexComponentCount;
      v6 += 3;
      ++v22;
    }
    while ( v22 < tess_numVertexes );
    result = 0;
  }
  tess_stageTangentsValid = 0;
  tess_stageBitangentsValid = 0;
  return result;
}

/* ---- RB_CalcBulgeVertexes  0x00513AD0 ----  [HIGH] */
int __cdecl RB_CalcBulgeVertexes(float *a1)
{
  int result;
  int v2;
  float *v3;
  float *v4;
  float *v5;
  double v6;
  float v7;

  result = tess_numVertexes;
  v2 = 0;
  v3 = (float *)tess_texCoords0;
  v4 = (float *)tess_xyz;
  v7 = (double)backEnd_refdef_time * a1[12] * 0.0010000000474974513;
  if ( tess_numVertexes > 0 )
  {
    v5 = dword_1827F68;
    do
    {
      ++v2;
      v6 = tr_sinTable[(unsigned __int64)((a1[10] * *v3 + v7) * 162.9746551513671875) & 0x3FF]
         * a1[11];
      v3 += 2;
      v5 += 3;
      *v4 = v6 * *(v5 - 5) + *v4;
      v4[1] = v6 * *(v5 - 4) + v4[1];
      v4[2] = v6 * *(v5 - 3) + v4[2];
      result = tess_vertexComponentCount;
      v4 += tess_vertexComponentCount;
    }
    while ( v2 < tess_numVertexes );
  }
  return result;
}

/* ---- RB_CalcMoveVertexes  0x00513B70 ----  [HIGH] */
float *__cdecl RB_CalcMoveVertexes(int a1)
{
  float *v1;
  int v2;
  double v3;
  float *result;
  float v5;
  float v6;
  float v7;
  float i;

  v1 = TableForFunc(*(void **)(a1 + 16));
  v5 = (*(float *)(a1 + 32) * tess_shaderTime + *(float *)(a1 + 28)) * 1024.0;
  ftol_tempSpill = (int)v5;
  v2 = 0;
  v3 = *(float *)(a1 + 24) * v1[(int)v5 & 0x3FF] + *(float *)(a1 + 20);
  result = (float *)tess_xyz;
  v6 = v3 * *(float *)(a1 + 4);
  v7 = v3 * *(float *)(a1 + 8);
  for ( i = v3 * *(float *)(a1 + 12); v2 < tess_numVertexes; result += tess_vertexComponentCount )
  {
    ++v2;
    *result = v6 + *result;
    result[1] = v7 + result[1];
    result[2] = i + result[2];
  }
  return result;
}

/* ---- DeformText  0x00513C20 ----  VERIFIED */
void __cdecl DeformText(const char *a1)
{
  float origin[3];        /* var_C /var_8 /var_4   [esp+40h..48h] */
  float width[3];         /* var_18/var_14/var_10  [esp+34h..3Ch] */
  float height[3];        /* var_24/var_20/var_1C  [esp+28h..30h]
                           * -- holds `mid` first, `height` afterwards */
  float step[3];
  unsigned char color[4]; /* var_38, four 0xFF bytes at 0x00513E94-0x00513EA4 */
  float bottom, top;
  float t0, scale, f;
  const float *xyz;
  int   len, i, ch;
  float fcol, frow;

  t0       = tess_stageNormals[2] * 0.0f;
  width[0] = tess_stageNormals[1] * -1.0f - t0;
  width[1] = t0 - tess_stageNormals[0] * -1.0f;
  width[2] = tess_stageNormals[0] * 0.0f - tess_stageNormals[1] * 0.0f;

  height[0] = tess_xyz[0];
  height[1] = tess_xyz[1];
  height[2] = tess_xyz[2];
  bottom = 999999.0f;
  top    = -999999.0f;
  if ( tess_xyz[2] < 999999.0f )
    bottom = tess_xyz[2];
  if ( tess_xyz[2] > -999999.0f )
    top = tess_xyz[2];

  for ( i = 1; i < 4; i++ )
  {
    xyz = &tess_xyz[tess_vertexComponentCount * i];
    height[0] += xyz[0];
    height[1] += xyz[1];
    height[2] += xyz[2];
    if ( xyz[2] < bottom )
      bottom = xyz[2];
    if ( xyz[2] > top )
      top = xyz[2];
  }

  origin[0] = height[0] * 0.25f;
  height[0] = 0.0f;
  origin[1] = height[1] * 0.25f;
  height[1] = 0.0f;
  origin[2] = height[2] * 0.25f;
  height[2] = ( top - bottom ) * 0.5f;

  scale    = height[2] * -0.75f;
  width[0] = scale * width[0];
  width[1] = scale * width[1];
  width[2] = scale * width[2];

  len = strlen( a1 );
  f   = (float)( len - 1 );

  tess_numIndexes = 0;
  tess_numVertexes = 0;

  origin[0] = f * width[0] + origin[0];
  origin[1] = f * width[1] + origin[1];
  origin[2] = f * width[2] + origin[2];

  color[0] = 0xFF;
  color[1] = 0xFF;
  color[2] = 0xFF;
  color[3] = 0xFF;

  if ( len > 0 )
  {
    step[0] = width[0] + width[0];
    step[1] = width[1] + width[1];
    step[2] = width[2] + width[2];

    for ( i = 0; i < len; i++ )
    {
      ch = (unsigned char)a1[i];
      if ( ch != 32 )
      {
        frow = (float)( ch >> 4 ) * 0.0625f;
        fcol = (float)( ch & 0xF ) * 0.0625f;
        RB_AddQuadStampExt( origin, width, height, (int *)color,
                            fcol, frow, fcol + 0.0625f, frow + 0.0625f );
      }
      origin[0] = origin[0] - step[0];
      origin[1] = origin[1] - step[1];
      origin[2] = origin[2] - step[2];
    }
  }
}

/* ---- GlobalPositionToLocal  0x00513F90 ----  VERIFIED */
float *__cdecl GlobalPositionToLocal(float *result, float *a2)
{
  double v2;
  double v3;
  double v4;

  v2 = *a2 - flt_16D8DA8;
  v3 = a2[1] - flt_16D8DAC;
  v4 = a2[2] - flt_16D8DB0;
  *result = flt_16D8DBC * v4 + flt_16D8DB8 * v3 + flt_16D8DB4 * v2;
  result[1] = flt_16D8DC8 * v4 + flt_16D8DC4 * v3 + flt_16D8DC0 * v2;
  result[2] = flt_16D8DD4 * v4 + flt_16D8DD0 * v3 + flt_16D8DCC * v2;
  return result;
}

/* ---- GlobalVectorToLocal  0x00514010 ----  VERIFIED */
float *__cdecl GlobalVectorToLocal(float *result, float *a2)
{
  *a2 = flt_16D8DB4 * *result + flt_16D8DBC * result[2] + flt_16D8DB8 * result[1];
  a2[1] = flt_16D8DC0 * *result + flt_16D8DC8 * result[2] + flt_16D8DC4 * result[1];
  a2[2] = flt_16D8DCC * *result + flt_16D8DD4 * result[2] + flt_16D8DD0 * result[1];
  return result;
}

/* ---- AutospriteDeform  0x00514080 ----  VERIFIED */
GLint AutospriteDeform()
{
  float *v0;
  int v1;
  GLint result;
  float *v3;
  int *v4;
  unsigned int v5;
  float *v6;
  double v7;
  double v8;
  double v9;
  double v10;
  long double v11;
  double v12;
  double v13;
  float v14[3]; /* [ebp-3Ch] BYREF */
  float v17[3]; /* [ebp-30h] BYREF */
  float v20[3]; /* [ebp-24h] BYREF */
  float v23[3]; /* [ebp-18h] BYREF */
  float v26[3]; /* [ebp-0Ch] BYREF */

  if ( (tess_numVertexes & 3) != 0 )
    ri_Printf(2, "Autosprite shader %s had odd vertex count\n", (const char *)tess_shader);
  if ( tess_numIndexes != 6 * (tess_numVertexes >> 2) )
    ri_Printf(2, "Autosprite shader %s had odd index count\n", (const char *)tess_shader);
  v0 = (float *)backEnd_currentEntity;
  v1 = tess_numVertexes;
  tess_numVertexes = 0;
  tess_numIndexes = 0;
  if ( (_UNKNOWN *)backEnd_currentEntity == &tr_worldEntity )
  {
    v20[0] = backEnd_viewParms_axis10;
    v20[1] = backEnd_viewParms_axis11;
    v20[2] = backEnd_viewParms_axis12;
    result = LODWORD(backEnd_viewParms_axis21);
    v23[0] = backEnd_viewParms_axis20;
    v23[1] = backEnd_viewParms_axis21;
    v23[2] = backEnd_viewParms_axis22;
  }
  else
  {
    GlobalVectorToLocal(&backEnd_viewParms_axis10, v20);
    result = (GLint)GlobalVectorToLocal(&backEnd_viewParms_axis20, v23);
  }
  v3 = (float *)tess_xyz;
  if ( v1 > 0 )
  {
    result = tess_vertexComponentCount;
    v4 = tess_vertexColors;
    v5 = ((unsigned int)(v1 - 1) >> 2) + 1;
    while ( 1 )
    {
      v6 = &v3[3 * result];
      v26[0] = (v3[2 * result] + v3[result] + *v3 + *v6) * 0.25;
      v26[1] = (v3[result + 1] + v3[2 * result + 1] + v6[1] + v3[1]) * 0.25;
      v26[2] = (v3[result + 2] + v3[2 * result + 2] + v6[2] + v3[2]) * 0.25;
      v7 = v3[2] - v26[2];
      v8 = v3[1] - v26[1];
      v9 = v7 * v7 + v8 * v8;
      v10 = *v3 - v26[0];
      v11 = sqrt(v9 + v10 * v10);
      v14[0] = v20[0] * (v11 * 0.70700001716613769531);
      v14[1] = v20[1] * (v11 * 0.70700001716613769531);
      v14[2] = v20[2] * (v11 * 0.70700001716613769531);
      v17[0] = v23[0] * (v11 * 0.70700001716613769531);
      v17[1] = v23[1] * (v11 * 0.70700001716613769531);
      v17[2] = v11 * 0.70700001716613769531 * v23[2];
      if ( backEnd_viewParms_isMirror )
      {
        v14[0] = 0.0 - v14[0];
        v14[1] = 0.0 - v14[1];
        v14[2] = 0.0 - v14[2];
      }
      if ( !((v0[16] == 0.0) | __UNORDERED__(v0[16], 0.0)) )
      {
        v12 = sqrt(v0[7] * v0[7] + v0[8] * v0[8] + v0[9] * v0[9]);
        if ( (v12 == 0.0) | __UNORDERED__(v12, 0.0) )
          v13 = 0.0;
        else
          v13 = 1.0 / v12;
        v14[0] = v14[0] * v13;
        v14[1] = v14[1] * v13;
        v14[2] = v14[2] * v13;
        v17[0] = v17[0] * v13;
        v17[1] = v17[1] * v13;
        v17[2] = v13 * v17[2];
      }
      RB_AddQuadStampExt(v26, v14, v17, v4, 0.0f, 0.0f, 1.0f, 1.0f);
      result = tess_vertexComponentCount;
      v4 += 4;
      v3 += 4 * tess_vertexComponentCount;
      if ( !--v5 )
        break;
      v0 = (float *)backEnd_currentEntity;
    }
  }
  return result;
}

/* ---- Autosprite2Deform  0x00514330 ----  [HIGH] */
int Autosprite2Deform()
{
  int v0;
  int result;
  float *v2;
  float *v3;
  float *v4;
  int v5;
  int v6;
  int v7;
  int v8;
  float v9;
  int v10;
  double v11;
  float *v12;
  int v13;
  int v14;
  int v15;
  int v16;
  double v17;
  float *v18;
  float *v19;
  float v20;
  float v21;
  float v22;
  float v23;
  float v24;
  float v25;
  float v26;
  float v27;
  float *v28;
  float *v29;
  float v30;
  float v31;
  int v32;
  float v33;
  float v34;
  float v35;
  float v36;
  float v37;
  float v38;
  float v39;
  float v40;
  float v41;
  float v42;
  float v43;
  float v44;
  float v45;
  float v46;
  float v47;
  float v48;
  float v49;
  float v50;
  float v51;
  float v52;
  float v53[3]; // [ebp-0x4c] BYREF  -- retail vec3_t
  float v56[3]; // [esp+2Ch..34h] BYREF -- ONE vec3
  float v59;
  float v60;
  float v61;
  float *v62;
  float *v63;
  float *v64;
  float *v65;
  float v66;
  float v67;
  float v68;
  float v69;
  float v70;
  float v71;

  v0 = tess_numVertexes;
  if ( (tess_numVertexes & 3) != 0 )
  {
    ri_Printf(2, "Autosprite2 shader %s had odd vertex count\n", (const char *)tess_shader);
    v0 = tess_numVertexes;
  }
  if ( tess_numIndexes != 6 * (v0 >> 2) )
    ri_Printf(2, "Autosprite2 shader %s had odd index count\n", (const char *)tess_shader);
  if ( (_UNKNOWN *)backEnd_currentEntity == &tr_worldEntity )
  {
    v56[0] = backEnd_viewParms_axis00;
    v56[1] = backEnd_viewParms_axis01;
    v56[2] = backEnd_viewParms_axis02;
  }
  else
  {
    GlobalVectorToLocal(&backEnd_viewParms_axis00, v56);
  }
  result = 0;
  v32 = 0;
  if ( tess_numIndexes > 0 )
  {
    v2 = v63;
    v3 = v64;
    v4 = v62;
    while ( 1 )
    {
      v5 = (unsigned __int16)tess_indexes[result];
      v6 = tess_vertexComponentCount * (unsigned __int16)word_17A7F62[result];
      v7 = tess_vertexComponentCount * v5;
      v35 = *(float *)&tess_xyz[v6] - *(float *)&tess_xyz[v7];
      v41 = flt_17BFF64[v6] - flt_17BFF64[v7];
      v47 = *(float *)&dword_17BFF68[v6] - *(float *)&dword_17BFF68[v7];
      v8 = 4 * v7 + ((int)(char *)tess_xyz);
      v9 = 3.4028234663852885981e38;
      v34 = 3.4028234663852885981e38;
      v33 = 3.4028234663852885981e38;
      v28 = (float *)v8;
      v20 = v47 * v47 + v41 * v41 + v35 * v35;
      if ( (v20 < 3.4028234663852885981e38) | __UNORDERED__(v20, 3.4028234663852885981e38) )
      {
        v34 = 3.4028234663852885981e38;
        v9 = v47 * v47 + v41 * v41 + v35 * v35;
        v3 = v4;
        v65 = v2;
        v4 = (float *)v8;
        v2 = (float *)(4 * v6 + ((int)(char *)tess_xyz));
        v33 = v9;
      }
      v10 = tess_vertexComponentCount * (unsigned __int16)word_17A7F64[v32];
      v36 = *(float *)&tess_xyz[v10] - *(float *)&tess_xyz[v6];
      v11 = flt_17BFF64[v10] - flt_17BFF64[v6];
      v12 = (float *)(4 * v10 + ((int)(char *)tess_xyz));
      v42 = v11;
      v48 = v12[2] - *(float *)&dword_17BFF68[v6];
      v21 = v48 * v48 + v42 * v42 + v36 * v36;
      if ( (v21 < (double)v33) | __UNORDERED__(v21, v33) )
      {
        v34 = v9;
        v9 = v48 * v48 + v42 * v42 + v36 * v36;
        v3 = v4;
        v65 = v2;
        v4 = (float *)(4 * v6 + ((int)(char *)tess_xyz));
        v2 = v12;
        v33 = v9;
      }
      else if ( (v21 < 3.4028234663852885981e38) | __UNORDERED__(v21, 3.4028234663852885981e38) )
      {
        v3 = (float *)(4 * v6 + ((int)(char *)tess_xyz));
        v65 = v12;
        v34 = v48 * v48 + v42 * v42 + v36 * v36;
      }
      v37 = *v28 - *v12;
      v43 = v28[1] - v12[1];
      v49 = v28[2] - v12[2];
      v22 = v49 * v49 + v43 * v43 + v37 * v37;
      if ( (v22 < (double)v33) | __UNORDERED__(v22, v33) )
      {
        v34 = v9;
        v9 = v49 * v49 + v43 * v43 + v37 * v37;
        v3 = v4;
        v65 = v2;
        v4 = v12;
        v2 = v28;
        v33 = v9;
      }
      else if ( (v22 < (double)v34) | __UNORDERED__(v22, v34) )
      {
        v3 = v12;
        v65 = v28;
        v34 = v49 * v49 + v43 * v43 + v37 * v37;
      }
      v13 = tess_vertexComponentCount * (unsigned __int16)word_17A7F68[v32];
      v14 = tess_vertexComponentCount * (unsigned __int16)word_17A7F66[v32];
      v38 = *(float *)&tess_xyz[v13] - *(float *)&tess_xyz[v14];
      v44 = flt_17BFF64[v13] - flt_17BFF64[v14];
      v50 = *(float *)&dword_17BFF68[v13] - *(float *)&dword_17BFF68[v14];
      v15 = 4 * v14 + ((int)(char *)tess_xyz);
      v29 = (float *)v15;
      v23 = v50 * v50 + v44 * v44 + v38 * v38;
      if ( (v23 < (double)v33) | __UNORDERED__(v23, v33) )
      {
        v34 = v9;
        v9 = v50 * v50 + v44 * v44 + v38 * v38;
        v3 = v4;
        v65 = v2;
        v4 = (float *)v15;
        v2 = (float *)(4 * v13 + ((int)(char *)tess_xyz));
        v33 = v9;
      }
      else if ( (v23 < (double)v34) | __UNORDERED__(v23, v34) )
      {
        v3 = (float *)v15;
        v65 = (float *)(4 * v13 + ((int)(char *)tess_xyz));
        v34 = v50 * v50 + v44 * v44 + v38 * v38;
      }
      v16 = tess_vertexComponentCount * (unsigned __int16)word_17A7F6A[v32];
      v39 = *(float *)&tess_xyz[v16] - *(float *)&tess_xyz[v13];
      v17 = flt_17BFF64[v16] - flt_17BFF64[v13];
      v18 = (float *)(4 * v16 + ((int)(char *)tess_xyz));
      v45 = v17;
      v51 = v18[2] - *(float *)&dword_17BFF68[v13];
      v24 = v51 * v51 + v45 * v45 + v39 * v39;
      if ( (v24 < (double)v33) | __UNORDERED__(v24, v33) )
      {
        v34 = v9;
        v9 = v51 * v51 + v45 * v45 + v39 * v39;
        v3 = v4;
        v65 = v2;
        v4 = (float *)(4 * v13 + ((int)(char *)tess_xyz));
        v2 = v18;
        v33 = v9;
      }
      else if ( (v24 < (double)v34) | __UNORDERED__(v24, v34) )
      {
        v3 = (float *)(4 * v13 + ((int)(char *)tess_xyz));
        v65 = v18;
        v34 = v51 * v51 + v45 * v45 + v39 * v39;
      }
      v40 = *v29 - *v18;
      v46 = v29[1] - v18[1];
      v52 = v29[2] - v18[2];
      v25 = v52 * v52 + v46 * v46 + v40 * v40;
      if ( (v25 < (double)v33) | __UNORDERED__(v25, v33) )
      {
        v3 = v4;
        v65 = v2;
        v4 = v18;
        v2 = v29;
        v33 = v52 * v52 + v46 * v46 + v40 * v40;
      }
      else
      {
        if ( !((v25 < (double)v34) | __UNORDERED__(v25, v34)) )
          goto LABEL_34;
        v9 = v52 * v52 + v46 * v46 + v40 * v40;
        v3 = v18;
        v65 = v29;
      }
      v34 = v9;
LABEL_34:
      v66 = (*v2 + *v4) * 0.5;
      v67 = (v4[1] + v2[1]) * 0.5;
      v68 = (v4[2] + v2[2]) * 0.5;
      v69 = (*v65 + *v3) * 0.5;
      v70 = (v65[1] + v3[1]) * 0.5;
      v71 = (v65[2] + v3[2]) * 0.5;
      v59 = v69 - v66;
      v60 = v70 - v67;
      v61 = v71 - v68;
      v53[0] = v60 * v56[2] - v61 * v56[1];
      v53[1] = v61 * v56[0] - v59 * v56[2];
      v53[2] = v59 * v56[1] - v60 * v56[0];
      VectorNormalize(v53);
      v26 = sqrt(v33) * 0.5;
      v30 = -v26;
      *v4 = v30 * v53[0] + v66;
      v4[1] = v30 * v53[1] + v67;
      v4[2] = v30 * v53[2] + v68;
      *v2 = v53[0] * v26 + v66;
      v2[1] = v53[1] * v26 + v67;
      v2[2] = v53[2] * v26 + v68;
      v27 = sqrt(v34) * 0.5;
      v19 = v65;
      *v3 = v53[0] * v27 + v69;
      v3[1] = v53[1] * v27 + v70;
      v3[2] = v53[2] * v27 + v71;
      v31 = -v27;
      *v19 = v31 * v53[0] + v69;
      v19[1] = v31 * v53[1] + v70;
      v19[2] = v31 * v53[2] + v71;
      result = v32 + 6;
      v32 += 6;
      if ( v32 >= tess_numIndexes )
        return result;
      result = v32;
    }
  }
  return result;
}

/* ---- RB_DeformTessGeometry  0x005149B0 ----  VERIFIED */
int __cdecl RB_DeformTessGeometry(void)
{
  int result;
  int v4;
  float *v5;
  int i;

  result = tess_shader;
  v4 = 0;
  for ( i = 0; i < *(_DWORD *)(tess_shader + 172); ++i )
  {
    v5 = (float *)(result + v4 + 176);
    switch ( *(_DWORD *)v5 )
    {
      case 1:
        RB_CalcDeformVertexes((int)v5);
        break;
      case 2:
        RB_CalcFlapVertexes(0, (int)v5);
        break;
      case 3:
        RB_CalcFlapVertexes(1, (int)v5);
        break;
      case 4:
        RB_CalcDeformNormals((int)v5);
        break;
      case 5:
        RB_CalcDeformSyncNormals((int)v5);
        break;
      case 6:
        RB_CalcBulgeVertexes(v5);
        break;
      case 7:
        RB_CalcMoveVertexes((int)v5);
        break;
      case 8:
        RB_ProjectionShadowDeform();
        break;
      case 9:
        AutospriteDeform();
        break;
      case 0xA:
        Autosprite2Deform();
        break;
      case 0xB:
      case 0xC:
      case 0xD:
      case 0xE:
      case 0xF:
      case 0x10:
      case 0x11:
      case 0x12:
        DeformText(&backEnd_refdef_text[32 * *(_DWORD *)v5 - 352]);
        break;
      default:
        break;
    }
    result = tess_shader;
    v4 += 52;
  }
  return result;
}

/* ---- RB_CalcColorFromEntity  0x00514AC0 ----  [HIGH] */
void __fastcall RB_CalcColorFromEntity( unsigned char *dstColors )
{
	int i;
	int *pColors = ( int * ) dstColors;
	int c;

	if ( !backEnd_currentEntity ) {
		return;
	}

	c = *( int * )( backEnd_currentEntity + 108 );

	for ( i = 0; i < tess_numVertexes; i++, pColors++ )
	{
		*pColors = c;
	}
}

/* ---- RB_CalcColorFromOneMinusEntity  0x00514B00 ----  [HIGH] */
int __fastcall RB_CalcColorFromOneMinusEntity(_DWORD *a1)
{
  int result;
  int v2;

  result = backEnd_currentEntity;
  if ( backEnd_currentEntity )
  {
    LOBYTE(v2) = -1 - *(_BYTE *)(backEnd_currentEntity + 108);
    BYTE1(v2) = -1 - *(_BYTE *)(backEnd_currentEntity + 109);
    BYTE2(v2) = -1 - *(_BYTE *)(backEnd_currentEntity + 110);
    BYTE3(v2) = -1 - *(_BYTE *)(backEnd_currentEntity + 111);
    for ( result = 0; result < tess_numVertexes; ++a1 )
    {
      *a1 = v2;
      ++result;
    }
  }
  return result;
}

/* ---- RB_CalcAlphaFromEntity  0x00514B70 ----  [HIGH] */
void __cdecl RB_CalcAlphaFromEntity( unsigned char *dstColors )
{
	int i;

	if ( !backEnd_currentEntity ) {
		return;
	}

	dstColors += 3;

	for ( i = 0; i < tess_numVertexes; i++, dstColors += 4 )
	{
		*dstColors = *( unsigned char * )( backEnd_currentEntity + 111 );
	}
}

/* ---- RB_CalcAlphaFromOneMinusEntity  0x00514BB0 ----  [HIGH] */
void __cdecl RB_CalcAlphaFromOneMinusEntity( unsigned char *dstColors )
{
	int i;

	if ( !backEnd_currentEntity ) {
		return;
	}

	dstColors += 3;

	for ( i = 0; i < tess_numVertexes; i++, dstColors += 4 )
	{
		*dstColors = 0xff - *( unsigned char * )( backEnd_currentEntity + 111 );
	}
}

/* ---- RB_CalcWaveColor  0x00514C00 ----  VERIFIED */
int __cdecl RB_CalcWaveColor(_DWORD *a1, int a2)
{
  double v4;
  float *v5;
  int result;
  float t;
  float noise;
  float v9;
  float v10;
  int v11;

  if ( *(_DWORD *)a2 == 6 )
  {
    t = (tess_shaderTime + *(float *)(a2 + 12)) * *(float *)(a2 + 16);
    noise = 0.0f;
    noise = Com_NoiseGet4f(0.0f, 0.0f, 0.0f, t);
    v4 = noise * *(float *)(a2 + 8) + *(float *)(a2 + 4);
  }
  else
  {
    v5 = TableForFunc(*(void **)a2);
    v9 = (*(float *)(a2 + 16) * tess_shaderTime + *(float *)(a2 + 12)) * 1024.0;
    ftol_tempSpill = (int)v9;
    v4 = (*(float *)(a2 + 8) * v5[ftol_tempSpill & 0x3FF] + *(float *)(a2 + 4)) * tr_identityLight;
  }
  if ( v4 < 0.0 )
  {
    v4 = 0.0;
  }
  else if ( v4 > 1.0 )
  {
    v4 = 1.0;
  }
  v10 = v4 * 255.0;
  ftol_tempSpill = (int)v10;
  BYTE2(v11) = ftol_tempSpill;
  BYTE1(v11) = BYTE2(v11);
  LOBYTE(v11) = BYTE2(v11);
  result = 0;
  for ( BYTE3(v11) = -1; result < tess_numVertexes; ++a1 )
  {
    *a1 = v11;
    ++result;
  }
  return result;
}

/* ---- RB_CalcWaveAlpha  0x00514D00 ----  VERIFIED */
int __cdecl RB_CalcWaveAlpha(int a1, int a2)
{
  unsigned __int64 v2; // rax
  int v3;

  v2 = (unsigned __int64)(EvalWaveFormClamped(a1) * 255.0);
  v3 = 0;
  if ( tess_numVertexes > 0 )
  {
    HIDWORD(v2) = a2 + 3;
    do
    {
      *(_BYTE *)HIDWORD(v2) = v2;
      ++v3;
      HIDWORD(v2) += 4;
    }
    while ( v3 < tess_numVertexes );
  }
  return v2;
}

/* ---- RB_CalcEnvironmentTexCoords  0x00514D40 ----  [HIGH] */
int __cdecl RB_CalcEnvironmentTexCoords(int a1)
{
  int result;
  int v3;
  float *v4;
  float *v5;
  double v6;
  double v7;
  double v8;
  float v9[3]; // [esp+Ch..14h] BYREF -- ONE vec3
  float v12;

  result = tess_numVertexes;
  v3 = 0;
  v4 = (float *)tess_xyz;
  if ( tess_numVertexes > 0 )
  {
    v5 = dword_1827F68;
    do
    {
      v9[0] = flt_16D8DD8 - *v4;
      v9[1] = flt_16D8DDC - v4[1];
      v9[2] = flt_16D8DE0 - v4[2];
      VectorNormalizeFast(v9);
      ++v3;
      v6 = v9[0] * *(v5 - 2);
      v5 += 3;
      a1 += 8;
      v7 = v6 + v9[1] * *(v5 - 4) + v9[2] * *(v5 - 3);
      v8 = v7 * *(v5 - 4) + v7 * *(v5 - 4) - v9[1];
      v12 = v7 * *(v5 - 3) + v7 * *(v5 - 3) - v9[2];
      *(float *)(a1 - 8) = (v8 + 1.0) * 0.5;
      *(float *)(a1 - 4) = 0.5 - v12 * 0.5;
      result = tess_vertexComponentCount;
      v4 += tess_vertexComponentCount;
    }
    while ( v3 < tess_numVertexes );
  }
  return result;
}

/* ---- RB_CalcCubeMapEyeToVertexTexCoords  0x00514E10 ----  VERIFIED */
int __fastcall RB_CalcCubeMapEyeToVertexTexCoords(int a1)
{
  __int64 v1; // rax

  v1 = (unsigned int)tess_xyz;
  if ( tess_numVertexes > 0 )
  {
    do
    {
      ++HIDWORD(v1);
      a1 += 12;
      *(float *)(a1 - 12) = *(float *)v1 - flt_16D8DD8;
      *(float *)(a1 - 8) = *(float *)(v1 + 4) - flt_16D8DDC;
      *(float *)(a1 - 4) = *(float *)(v1 + 8) - flt_16D8DE0;
      LODWORD(v1) = v1 + 4 * tess_vertexComponentCount;
    }
    while ( SHIDWORD(v1) < tess_numVertexes );
  }
  return v1;
}

/* ---- RB_CalcCubeMapVertexToEyeTexCoords  0x00514E60 ----  [HIGH] */
int __fastcall RB_CalcCubeMapVertexToEyeTexCoords(int a1)
{
  __int64 v1; // rax

  v1 = (unsigned int)tess_xyz;
  if ( tess_numVertexes > 0 )
  {
    do
    {
      ++HIDWORD(v1);
      a1 += 12;
      *(float *)(a1 - 12) = flt_16D8DD8 - *(float *)v1;
      *(float *)(a1 - 8) = flt_16D8DDC - *(float *)(v1 + 4);
      *(float *)(a1 - 4) = flt_16D8DE0 - *(float *)(v1 + 8);
      LODWORD(v1) = v1 + 4 * tess_vertexComponentCount;
    }
    while ( SHIDWORD(v1) < tess_numVertexes );
  }
  return v1;
}

/* ---- RB_CalcCubeMapReflectionTexCoords  0x00514EB0 ----  [HIGH] */
float *__fastcall RB_CalcCubeMapReflectionTexCoords(int a1, int a2)
{
  float *result;
  int v3;
  float *v4;
  double v5;
  double v6;
  double v7;
  double v8;
  float v9;
  float v10;
  float v11;

  result = (float *)tess_numVertexes;
  v3 = 0;
  v4 = (float *)tess_xyz;
  if ( tess_numVertexes > 0 )
  {
    result = dword_1827F68;
    do
    {
      ++v3;
      v5 = flt_16D8DD8 - *v4;
      result += 3;
      a2 += 12;
      v6 = flt_16D8DDC - v4[1];
      v11 = flt_16D8DE0 - v4[2];
      v7 = v6 * *(result - 4) + v5 * *(result - 5) + v11 * *(result - 3);
      v8 = v7 + v7;
      v9 = v8 * *(result - 4);
      v10 = v8 * *(result - 3);
      *(float *)(a2 - 12) = v8 * *(result - 5) - v5;
      *(float *)(a2 - 8) = v9 - v6;
      *(float *)(a2 - 4) = v10 - v11;
      v4 += tess_vertexComponentCount;
    }
    while ( v3 < tess_numVertexes );
  }
  return result;
}

/* ---- RB_CalcCubeMapLightVectorTexCoords  0x00514F60 ----  VERIFIED */
int __fastcall RB_CalcCubeMapLightVectorTexCoords(int a1)
{
  double v2;
  double v5;
  double v6;
  __int64 v7; // rax
  float v9;
  float v10;
  float v11;

  v2 = *(float *)(backEnd_currentDlight + 68);
  if ( *(float *)(backEnd_currentDlight + 80) == 0.0f )
  {
    v5 = *(float *)(backEnd_currentDlight + 72);
    v6 = *(float *)(backEnd_currentDlight + 76);
  }
  else
  {
    v2 = v2 - flt_16D8DA8;
    v5 = *(float *)(backEnd_currentDlight + 72) - flt_16D8DAC;
    v6 = *(float *)(backEnd_currentDlight + 76) - flt_16D8DB0;
  }
  v7 = (unsigned int)tess_xyz;
  v9 = flt_16D8DBC * v6 + flt_16D8DB8 * v5 + flt_16D8DB4 * v2;
  v10 = flt_16D8DC8 * v6 + flt_16D8DC4 * v5 + flt_16D8DC0 * v2;
  v11 = flt_16D8DD4 * v6 + flt_16D8DD0 * v5 + flt_16D8DCC * v2;
  if ( tess_numVertexes > 0 )
  {
    do
    {
      ++HIDWORD(v7);
      a1 += 12;
      *(float *)(a1 - 12) = v9 - *(float *)v7;
      *(float *)(a1 - 8) = v10 - *(float *)(v7 + 4);
      *(float *)(a1 - 4) = v11 - *(float *)(v7 + 8);
      LODWORD(v7) = v7 + 4 * tess_vertexComponentCount;
    }
    while ( SHIDWORD(v7) < tess_numVertexes );
  }
  return v7;
}

/* ---- RB_CalcCubeMapLightHalfAngleTexCoords  0x00515050 ----  VERIFIED */
int __cdecl RB_CalcCubeMapLightHalfAngleTexCoords(int a1)
{
  double v2;
  double v6;
  double v7;
  int result;
  int v9;
  float *v10;
  float v11[3]; // [esp+Ch..14h]  BYREF -- ONE vec3 (light vector)
  float v14[3]; // [esp+18h..20h] BYREF -- ONE vec3 (eye vector)
  float v17;
  float v18;
  float i;

  v2 = *(float *)(backEnd_currentDlight + 68);
  if ( *(float *)(backEnd_currentDlight + 80) == 0.0f )
  {
    v6 = *(float *)(backEnd_currentDlight + 72);
    v7 = *(float *)(backEnd_currentDlight + 76);
  }
  else
  {
    v2 = v2 - flt_16D8DA8;
    v6 = *(float *)(backEnd_currentDlight + 72) - flt_16D8DAC;
    v7 = *(float *)(backEnd_currentDlight + 76) - flt_16D8DB0;
  }
  result = tess_numVertexes;
  v9 = 0;
  v10 = (float *)tess_xyz;
  v17 = flt_16D8DBC * v7 + flt_16D8DB8 * v6 + flt_16D8DB4 * v2;
  v18 = flt_16D8DC8 * v7 + flt_16D8DC4 * v6 + flt_16D8DC0 * v2;
  for ( i = flt_16D8DD4 * v7 + flt_16D8DD0 * v6 + flt_16D8DCC * v2; v9 < tess_numVertexes; v10 += tess_vertexComponentCount )
  {
    v14[0] = flt_16D8DD8 - *v10;
    v14[1] = flt_16D8DDC - v10[1];
    v14[2] = flt_16D8DE0 - v10[2];
    VectorNormalizeFast(v14);
    v11[0] = v17 - *v10;
    v11[1] = v18 - v10[1];
    v11[2] = i - v10[2];
    VectorNormalizeFast(v11);
    ++v9;
    a1 += 12;
    *(float *)(a1 - 12) = v11[0] + v14[0];
    *(float *)(a1 - 8) = v11[1] + v14[1];
    *(float *)(a1 - 4) = v11[2] + v14[2];
    result = tess_vertexComponentCount;
  }
  return result;
}

/* ---- RB_CalcCubeMapSunHalfAngleTexCoords  0x005151B0 ----  [HIGH] */
int __cdecl RB_CalcCubeMapSunHalfAngleTexCoords(int a1)
{
  int result;
  int v3;
  float *i;
  float v5[3]; // [esp+Ch..14h] BYREF -- ONE vec3

  result = tess_numVertexes;
  v3 = 0;
  for ( i = (float *)tess_xyz; v3 < tess_numVertexes; i += tess_vertexComponentCount )
  {
    v5[0] = flt_16D8DD8 - *i;
    v5[1] = flt_16D8DDC - i[1];
    v5[2] = flt_16D8DE0 - i[2];
    VectorNormalizeFast(v5);
    ++v3;
    a1 += 12;
    *(float *)(a1 - 12) = *(float *)&tr_sunDirection + v5[0];
    *(float *)(a1 - 8) = *(float *)&dword_16C57D4 + v5[1];
    *(float *)(a1 - 4) = *(float *)&dword_16C57D8 + v5[2];
    result = tess_vertexComponentCount;
  }
  return result;
}

/* ---- RB_CalcCubeMapTbnTexCoords  0x00515250 ----  VERIFIED */
_DWORD *__cdecl RB_CalcCubeMapTbnTexCoords(_DWORD *a1, int a2)
{
  _DWORD *result;  /* the bitangent cursor; retail's EAX */
  int v4;
  _DWORD *tan;
  _DWORD *nrm;

  if ( !tess_stageTangentsValid || !tess_stageBitangentsValid )
    RB_CalcTangentSpace();
  result = (_DWORD *)tess_numVertexes;
  v4 = 0;
  if ( tess_numVertexes > 0 )
  {
    result = (_DWORD *)((char *)tess_stageBitangents + 4 * a2);
    tan    = (_DWORD *)((char *)tess_stageTangents   + 4 * a2);
    nrm    = (_DWORD *)((char *)tess_stageNormals    + 4 * a2);
    do
    {
      *a1 = *tan;
      a1[1] = *result;
      a1[2] = *nrm;
      ++v4;
      result += 3;
      tan += 3;
      nrm += 3;
      a1 += 3;
    }
    while ( v4 < tess_numVertexes );
  }
  return result;
}

/* ---- RB_CalcCubeMapDot3ReflectTexCoords  0x005152B0 ----  VERIFIED */
float *__cdecl RB_CalcCubeMapDot3ReflectTexCoords(int a1, float *a2)
{
  float *result;  /* the bitangent cursor; retail's EAX */
  int v5;
  double v6;
  float *tan;
  float *nrm;
  float *xyz;

  if ( !tess_stageTangentsValid || !tess_stageBitangentsValid )
    RB_CalcTangentSpace();
  result = (float *)tess_numVertexes;
  v5 = 0;
  if ( tess_numVertexes > 0 )
  {
    result = (float *)((char *)tess_stageBitangents + 4 * a1);
    tan    = (float *)((char *)tess_stageTangents   + 4 * a1);
    nrm    = (float *)((char *)tess_stageNormals    + 4 * a1);
    xyz    = (float *)((char *)tess_xyz             + 4 * a1);
    do
    {
      *a2 = *tan;
      a2[1] = *result;
      a2[2] = *nrm;
      v6 = backEnd_viewParms_originX[a1] - *xyz;
      ++v5;
      result += 3;
      tan += 3;
      nrm += 3;
      xyz += 3;
      a2 += 4;
      *(a2 - 1) = v6;
    }
    while ( v5 < tess_numVertexes );
  }
  return result;
}

/* ---- RB_CalcSwapTexCoords  0x00515320 ----  [HIGH] */
void __cdecl RB_CalcSwapTexCoords( float *st )
{
	int i;

	for ( i = 0; i < tess_numVertexes; i++, st += 2 )
	{
		float s = st[0];
		float t = st[1];

		st[0] = t;
		st[1] = 1.0f - s;
	}
}

/* ---- RB_CalcTurbulentTexCoords  0x00515360 ----  [HIGH] */
void __cdecl RB_CalcTurbulentTexCoords( float *st, const float *wf )
{
	int i;
	float now;
	const float *xyz;

	now = wf[3]  + tess_shaderTime * wf[4] ;

	xyz = tess_xyz_base;
	for ( i = 0; i < tess_numVertexes; i++, st += 2, xyz += tess_vertexComponentCount )
	{
		st[0] += tr_sinTable[ (int)( ( ( xyz[0] + xyz[2] ) * ( 1.0f / 1024.0f ) + now )
		                             * FUNCTABLE_SIZE ) & FUNCTABLE_MASK ] * wf[2] ;
		st[1] += tr_sinTable[ (int)( ( xyz[1] * ( 1.0f / 1024.0f ) + now )
		                             * FUNCTABLE_SIZE ) & FUNCTABLE_MASK ] * wf[2];
	}
}

/* ---- RB_CalcScaleTexCoords  0x00515410 ----  VERIFIED */
void __cdecl RB_CalcScaleTexCoords( float *st, const float *scale )
{
	int i;

	for ( i = 0; i < tess_numVertexes; i++, st += 2 )
	{
		st[0] *= scale[0];
		st[1] *= scale[1];
	}
}

/* ---- RB_CalcScrollTexCoords  0x00515440 ----  [HIGH] */
void __cdecl RB_CalcScrollTexCoords( const float *scrollSpeed, float *st )
{
	int i;
	float timeScale = tess_shaderTime;
	float adjustedScrollS, adjustedScrollT;

	adjustedScrollS = scrollSpeed[0] * timeScale;
	adjustedScrollT = scrollSpeed[1] * timeScale;

	adjustedScrollS = adjustedScrollS - floor( adjustedScrollS );
	adjustedScrollT = adjustedScrollT - floor( adjustedScrollT );

	for ( i = 0; i < tess_numVertexes; i++, st += 2 )
	{
		st[0] += adjustedScrollS;
		st[1] += adjustedScrollT;
	}
}

/* ---- RB_CalcTransformTexCoords  0x005154C0 ----  VERIFIED */
float *__cdecl RB_CalcTransformTexCoords(float *result, float *a2)
{
  int i;
  double v3;
  double v4;

  for ( i = 0; i < tess_numVertexes; *(a2 - 1) = v4 * result[9] + v3 * result[7] + result[11] )
  {
    v3 = *a2;
    ++i;
    v4 = a2[1];
    a2 += 2;
    *(a2 - 2) = v4 * result[8] + v3 * result[6] + result[10];
  }
  return result;
}

/* ---- RB_CalcRotateTexCoords  0x00515510 ----  [HIGH] */
float *__cdecl RB_CalcRotateTexCoords(float a1, float *a2)
{
  unsigned __int64 v2; // rax
  double v3;
  double v4;
  float v6;
  float v7[17]; // [esp+4h] [ebp-44h] BYREF
  float v8;

  v2 = (unsigned __int64)(-(tess_shaderTime * a1) * 2.8444445133209228516);
  v3 = tr_sinTable[v2 & 0x3FF];
  LODWORD(v2) = ((_WORD)v2 + 256) & 0x3FF;
  v4 = tr_sinTable[(_DWORD)v2];
  v7[6] = tr_sinTable[(_DWORD)v2];
  v7[8] = -v3;
  v6 = 0.5 * v4;
  v8 = v3 * 0.5;
  v7[10] = 0.5 - v6 + v8;
  v7[7] = v3;
  v7[9] = v4;
  v7[11] = 0.5 - v8 - v6;
  return RB_CalcTransformTexCoords(v7, a2);
}

/* ---- RB_CalcCubeMapNegateTexCoords  0x005155B0 ----  VERIFIED */
float *__cdecl RB_CalcCubeMapNegateTexCoords(float *result)
{
  int v1;
  double v2;

  v1 = 3 * tess_numVertexes;
  if ( 3 * tess_numVertexes > 0 )
  {
    do
    {
      v2 = *result++;
      --v1;
      *(result - 1) = -v2;
    }
    while ( v1 );
  }
  return result;
}

/* ---- GetSTVectors  0x005155D0 ----  VERIFIED */
double __cdecl GetSTVectors(float *result, int a2, int a3, int a4)
{
  double v4;
  double v5;
  int v6;
  double v7;
  int v8;
  int v9;
  double v10;

  v4 = *(float *)&dword_17DFF64[2 * a3] - *(float *)&dword_17DFF64[2 * a4];
  v5 = *(float *)&dword_17DFF64[2 * a4] - *(float *)&dword_17DFF64[2 * a2];
  v6 = a2 * tess_vertexComponentCount;
  v7 = *(float *)&dword_17DFF64[2 * a2] - *(float *)&dword_17DFF64[2 * a3];
  v8 = a3 * tess_vertexComponentCount;
  v9 = a4 * tess_vertexComponentCount;
  *result = v4 * *(float *)&tess_xyz[v6]
          + v5 * *(float *)&tess_xyz[a3 * tess_vertexComponentCount]
          + v7 * *(float *)&tess_xyz[a4 * tess_vertexComponentCount];
  result[1] = v4 * flt_17BFF64[v6] + v5 * flt_17BFF64[v8] + v7 * flt_17BFF64[v9];
  v10 = v4 * *(float *)&dword_17BFF68[v6] + v5 * *(float *)&dword_17BFF68[v8] + v7 * *(float *)&dword_17BFF68[v9];
  result[2] = v10;
  VectorNormalizeFast(result);
  return v10;
}

/* ---- RB_CalcTangentSpace  0x00515680 ----  [HIGH] */
int RB_CalcTangentSpace()
{
  int i;
  int v1;
  int v2;
  int v3;
  double v4;
  int v5;
  int v6;
  int v7;
  int result;
  int v9;
  float *v10;
  float v11[3]; // [esp+10h..18h] BYREF -- ONE vec3
  int v14;
  _DWORD v15[8192]; // [esp+20h] [ebp-8000h] BYREF
  float *nrm;
  float *bit;

  memset(tess_stageTangents, 0, 12 * tess_numVertexes);
  memset(tess_stageBitangents, 0, 12 * tess_numVertexes);
  memset(v15, 0, 4 * tess_numVertexes);
  for ( i = 0; i < tess_numIndexes; *(float *)(v7 + ((int)(char *)tess_stageTangents + 8)) = v11[2] + flt_1857F68[3 * v2] )
  {
    v1 = (unsigned __int16)tess_indexes[i];
    v2 = (unsigned __int16)word_17A7F64[i];
    v3 = (unsigned __int16)word_17A7F62[i];
    v14 = v2;
    GetSTVectors(v11, v1, v3, v2);
    v4 = v11[0];
    ++v15[v1];
    ++v15[v3];
    ++v15[v2];
    v5 = 12 * v1;
    i += 3;
    *(float *)(v5 + ((int)(char *)tess_stageTangents)) = v4 + *(float *)&tess_stageTangents[3 * v1];
    *(float *)(v5 + ((int)(char *)tess_stageTangents + 4)) = v11[1] + *((float *)&unk_1857F64 + 3 * v1);
    *(float *)(v5 + ((int)(char *)tess_stageTangents + 8)) = v11[2] + flt_1857F68[3 * v1];
    v6 = 12 * v3;
    *(float *)(v6 + ((int)(char *)tess_stageTangents)) = v11[0] + *(float *)&tess_stageTangents[3 * v3];
    *(float *)(v6 + ((int)(char *)tess_stageTangents + 4)) = v11[1] + *((float *)&unk_1857F64 + 3 * v3);
    *(float *)(v6 + ((int)(char *)tess_stageTangents + 8)) = v11[2] + flt_1857F68[3 * v3];
    v7 = 12 * v2;
    *(float *)(v7 + ((int)(char *)tess_stageTangents)) = v11[0] + *(float *)&tess_stageTangents[3 * v2];
    *(float *)(v7 + ((int)(char *)tess_stageTangents + 4)) = v11[1] + *((float *)&unk_1857F64 + 3 * v2);
  }
  result = tess_numVertexes;
  v9 = 0;
  if ( tess_numVertexes > 0 )
  {
    v10 = flt_1857F68;
    nrm = tess_stageNormals;
    bit = tess_stageBitangents;
    do
    {
      if ( (int)v15[v9] > 1 )
        VectorNormalize(v10 - 2);
      bit[0] = *v10 * nrm[1] - nrm[2] * *(v10 - 1);
      bit[1] = *(v10 - 2) * nrm[2] - nrm[0] * *v10;
      bit[2] = nrm[0] * *(v10 - 1) - *(v10 - 2) * nrm[1];
      VectorNormalize(bit);
      result = tess_numVertexes;
      ++v9;
      v10 += 3;
      nrm += 3;
      bit += 3;
    }
    while ( v9 < tess_numVertexes );
  }
  tess_stageTangentsValid = 1;
  tess_stageBitangentsValid = 1;
  return result;
}

/* ---- RB_CalcCubeMapBumpmapFrameTexCoords  0x005158A0 ----  [HIGH] */
float *__cdecl RB_CalcCubeMapBumpmapFrameTexCoords(float *a1)
{
  float *result;  /* the tangent cursor; retail's EAX */
  int v3;
  double v4;
  double v5;
  double v6;
  float *bit;
  float *nrm;

  if ( !tess_stageTangentsValid || !tess_stageBitangentsValid )
    RB_CalcTangentSpace();
  result = (float *)tess_numVertexes;
  v3 = 0;
  if ( tess_numVertexes > 0 )
  {
    result = tess_stageTangents;
    bit    = tess_stageBitangents;
    nrm    = tess_stageNormals;
    do
    {
      v4 = *a1;
      ++v3;
      v5 = a1[1];
      v6 = a1[2];
      a1 += 3;
      *(a1 - 3) = v6 * result[2] + v4 * result[0] + v5 * result[1];
      *(a1 - 2) = v6 * bit[2]    + v5 * bit[1]    + v4 * bit[0];
      *(a1 - 1) = v6 * nrm[2]    + v5 * nrm[1]    + v4 * nrm[0];
      result += 3;
      bit += 3;
      nrm += 3;
    }
    while ( v3 < tess_numVertexes );
  }
  return result;
}

/* ---- myftol  0x00515940 ----  VERIFIED */
int __cdecl myftol(float a1)
{
  ftol_tempSpill = (int)a1;
  return (int)a1;
}

/* ---- RB_CalcSpecularAlpha  0x00515950 ----  VERIFIED */
int __cdecl RB_CalcSpecularAlpha(int a1)
{
  unsigned __int64 v2; // rax
  _BYTE *v3;
  float *v4;
  float *v5;
  int v6;
  double v7;  /* normal . lightDir */
  double v8;  /* reflected[0] */
  double v9;
  double v10;  /* reflected[1] */
  double v11;  /* reflected[2] */
  double v12;  /* viewer[0], never spilled by retail */
  double v13;
  int v15;  /* viewer.viewer, read as bits for Q_rsqrt */
  float v16;
  float lightDir[3]; // [esp+14h] [ebp-18h] BYREF -- ONE vec3
  float v20;  /* viewer[1] */
  float v21;  /* viewer[2] */
  int   rsqBits;  /* the Q_rsqrt seed, stored then reread */
  float rsq;

  LODWORD(v2) = tess_numVertexes;
  v3 = (_BYTE *)(a1 + 3);
  v4 = (float *)tess_xyz;
  if ( tess_numVertexes > 0 )
  {
    v5 = dword_1827F68;
    v6 = tess_numVertexes;
    do
    {
      lightDir[0] = rbSpecularLightOrigin - *v4;
      lightDir[1] = flt_571134 - v4[1];
      lightDir[2] = flt_571138 - v4[2];
      VectorNormalizeFast(lightDir);

      v7 = lightDir[2] * *v5 + lightDir[1] * *(v5 - 1) + lightDir[0] * *(v5 - 2);
      v8 = v7 * *(v5 - 2) + v7 * *(v5 - 2) - lightDir[0];
      v9 = v7;
      v10 = v7 * *(v5 - 1) + v7 * *(v5 - 1) - lightDir[1];
      v11 = v9 * *v5 + v9 * *v5 - lightDir[2];

      v12 = flt_16D8DD8 - *v4;
      v20 = flt_16D8DDC - v4[1];
      v13 = flt_16D8DE0 - v4[2];
      v21 = v13;
      *(float *)&v15 = v13 * v21 + v20 * v20 + v12 * v12;

      rsqBits = 1597463007 - (v15 >> 1);             /* 0x5F3759DF */
      rsq = COERCE_FLOAT(rsqBits);
      v16 = (1.5 - *(float *)&v15 * 0.5 * rsq * rsq) * rsq
          * (v21 * v11 + v20 * v10 + v12 * v8);

      if ( v16 < 0.0 )
      {
        LODWORD(v2) = 0;
      }
      else
      {
        v2 = (unsigned __int64)(v16 * v16 * (v16 * v16) * 255.0);
        if ( (int)v2 > 255 )
          LODWORD(v2) = 255;
      }
      *v3 = v2;
      v5 += 3;
      v3 += 4;
      --v6;
      v4 += tess_vertexComponentCount;
    }
    while ( v6 );
  }
  return v2;
}

/* ---- RB_CalcDiffuseColor  0x00515AD0 ----  [HIGH] */
int __cdecl RB_CalcDiffuseColor(int a1)
{
  int v1;
  int result;
  float *v3;
  _BYTE *v4;
  int v5;
  double v6;
  int v11;
  int v12;
  float v13;
  float v14;
  float v15;
  float v16;
  float v17;
  float v18;
  float v19;
  float v20;
  float v21;
  float v22;
  float v23;
  float v24;
  float v25;

  v23 = *(float *)(backEnd_currentEntity + 176);
  v24 = *(float *)(backEnd_currentEntity + 180);
  v25 = *(float *)(backEnd_currentEntity + 184);
  v20 = *(float *)(backEnd_currentEntity + 192);
  v1 = *(_DWORD *)(backEnd_currentEntity + 188);
  v21 = *(float *)(backEnd_currentEntity + 196);
  v22 = *(float *)(backEnd_currentEntity + 200);
  v19 = *(float *)(backEnd_currentEntity + 172);
  result = tess_numVertexes;
  v17 = *(float *)(backEnd_currentEntity + 164);
  v18 = *(float *)(backEnd_currentEntity + 168);
  if ( tess_numVertexes > 0 )
  {
    v3 = flt_1827F64;
    v4 = (_BYTE *)(a1 + 2);
    v5 = tess_numVertexes;
    do
    {
      v6 = v19 * v3[1] + v17 * *(v3 - 1) + v18 * *v3;
      v13 = v6;
      if ( v6 > 0.0 )
      {
        v14 = v13 * v20 + v23;
        ftol_tempSpill = (int)v14;
        v11 = (int)v14;
        if ( v11 > 255 )
          LOBYTE(v11) = -1;
        *(v4 - 2) = v11;
        v15 = v13 * v21 + v24;
        ftol_tempSpill = (int)v15;
        v12 = (int)v15;
        if ( v12 > 255 )
          LOBYTE(v12) = -1;
        *(v4 - 1) = v12;
        v16 = v13 * v22 + v25;
        ftol_tempSpill = (int)v16;
        result = (int)v16;
        if ( result > 255 )
          result = 255;
        *v4 = result;
        v4[1] = -1;
      }
      else
      {
        *(_DWORD *)(v4 - 2) = v1;
      }
      v4 += 4;
      v3 += 3;
      --v5;
    }
    while ( v5 );
  }
  return result;
}

