/*
 * @fidelity: likely
 * @fidelity-default: unreviewed
 */

#include "../qcommon/qcommon.h"
#include "../qcommon/hexrays_shim.h"
#include "../qcommon/cod1_globals.h"
#include "tr_records.h"
#include "tr_shaderregistry.h"
#include "tr_gl_types.h"
#include "tr_tess.h"


#define r_dynamiclight             ((cvar_t *)(r_dynamiclight))
#define r_lodbias                  ((cvar_t *)(r_lodbias))
#define r_lodscale                 ((cvar_t *)(r_lodscale))
#define r_norefresh                ((cvar_t *)(r_norefresh))


extern int R_RenderView();

/* ---- R_ToggleSmpFrame  0x004E96F0 ----  VERIFIED */
void R_ToggleSmpFrame( void ) {
  *(_DWORD *)(backEndData + 1898240) = 0; //backEndData[tr.smpFrame]->commands.used = 0;

  r_firstSceneDrawSurf = 0;

  r_numdlights = 0;
  r_firstSceneDlight = 0;

  r_numcoronas = 0;
  r_firstSceneCorona = 0;

  r_numentities = 0;
  r_firstSceneEntity = 0;

  r_numpolys = 0;
  r_firstScenePoly = 0;

  r_numpolyverts = 0;

}

/* ---- RE_ClearScene  0x004E9740 ----  VERIFIED */
int RE_ClearScene()
{
  int result;

  r_firstSceneDlight = r_numdlights;
  result = r_numpolys;
  r_firstSceneCorona = r_numcoronas;
  r_firstSceneEntity = r_numentities;
  r_firstScenePoly = r_numpolys;
  return result;
}

/* ---- R_AddPolygonSurfaces  0x004E9770 ----  VERIFIED */
int R_AddPolygonSurfaces()
{
  int result;
  int v1;
  int i;
  int v3;
  char *v4;

  result = tr_refdef_numPolys;
  v1 = tr_refdef_polys;
  for ( i = 0; i < tr_refdef_numPolys; v1 += 16 )
  {
    tr_currentEntityNumber = i & 0x3FF;
    tr_shiftedEntityNumber = tr_currentEntityNumber << 8;
    v3 = *(_DWORD *)(v1 + 4);
    if ( v3 >= 0 && v3 < tr_numShaders[0] )
    {
      v4 = (char *)tr_shaders[v3];
    }
    else
    {
      ri_Printf(2, "R_GetShaderByHandle: out of range hShader '%d'\n", *(_DWORD *)(v1 + 4));
      v4 = tr_defaultShader;
    }
    if ( tr_refdef_numDrawSurfs < 0x10000 )
    {
      *(_DWORD *)(tr_refdef_drawSurfs + 8 * tr_refdef_numDrawSurfs) = tr_shiftedEntityNumber
                                                     | ((*((_DWORD *)v4 + 18) | (storageClass << 12)) << 18)
                                                     | 2;
      *(_DWORD *)(tr_refdef_drawSurfs + 8 * tr_refdef_numDrawSurfs++ + 4) = v1;
    }
    result = tr_refdef_numPolys;
    ++i;
  }
  return result;
}

/* ---- RE_AddPolyToScene  0x004E9830 ----  VERIFIED */
int __cdecl RE_AddPolyToScene(int a1, int a2, const void *a3)
{
  int result;
  int v4;
  int v5;
  int v6;
  int v7;
  int v8;
  _DWORD *v9;
  void *v10;

  result = tr_registered;
  if ( tr_registered )
  {
    v4 = a1;
    if ( !a1 )
      v4 = *((_DWORD *)tr_defaultShader + 17);
    v5 = r_numpolyverts;
    result = max_polyverts;
    v6 = r_numpolyverts + a2;
    if ( r_numpolyverts + a2 <= max_polyverts )
    {
      v7 = r_numpolys;
      if ( r_numpolys < max_polys )
      {
        v8 = backEndData;
        v9 = (_DWORD *)(backEndData + 16 * (r_numpolys + 81776));
        v9[2] = a2;
        v10 = (void *)(v8 + 32 * (v5 + 42936));
        *v9 = 2;
        v9[1] = v4;
        v9[3] = v10;
        result = 32 * a2;
        qmemcpy(v10, a3, 32 * a2);
        r_numpolys = v7 + 1;
        r_numpolyverts = v6;
      }
    }
  }
  return result;
}

/* ---- RE_AddPolysToScene  0x004E98D0 ----  VERIFIED */
void __cdecl RE_AddPolysToScene(int a1, int a2, char *a3, int a4)
{
  int v4;
  char *v5;
  unsigned int v6;
  int *v7;
  bool v8; // cc
  int v9;
  int v11;

  if ( tr_registered )
  {
    if ( a1 )
    {
      v11 = 0;
      if ( a4 > 0 )
      {
        v4 = a2;
        v9 = r_numpolyverts + a2;
        v5 = (char *)(backEndData + 32 * (r_numpolyverts + 42936));
        v6 = 32 * a2;
        v7 = (int *)(16 * r_numpolys + backEndData + 1308424);
        do
        {
          if ( v9 > max_polyverts )
            break;
          if ( r_numpolys >= max_polys )
            break;
          *v7 = v4;
          *(v7 - 1) = a1;
          v7[1] = (int)v5;
          *(v7 - 2) = 2;
          qmemcpy(v5, a3, v6);
          ++r_numpolys;
          v4 = a2;
          v9 += a2;
          v7 += 4;
          v5 += v6;
          r_numpolyverts += a2;
          v8 = ++v11 < a4;
          a3 += v6;
        }
        while ( v8 );
      }
    }
    else
    {
      ri_Printf(2, "WARNING: RE_AddPolysToScene: NULL poly shader\n");
    }
  }
}

/* ---- RE_AddRefEntityToScene  0x004E9A00 ----  VERIFIED */
void __cdecl RE_AddRefEntityToScene(int *a1, int a2)
{
  int v2;
  int v3;
  int v4;

  if ( tr_registered )
  {
    if ( com_statmon->integer )
    {
      if ( r_numentities >= 919 )
        StatMon_Warning(9, 3000, "gfx/2d/warning@models.jpg");
    }
    if ( r_numentities < 1022 )
    {
      v2 = *a1;
      if ( *a1 < 0 || v2 >= 16 )
        ri_Error(1, "\x15" "RE_AddRefEntityToScene: bad reType %i", v2);
      v3 = r_numentities;
      v4 = 696 * r_numentities + backEndData;
      qmemcpy((void *)(v4 + 530176), a1, 0x9Cu);
      *(_BYTE *)(v4 + 530337) = 0;
      *(_DWORD *)(v4 + 530868) = 0;
      *(_DWORD *)(v4 + 530864) = a2;
      *(_DWORD *)(v4 + 530380) = 0;
      r_numentities = v3 + 1;
      ++tr_refdef_num_entities;
    }
    else if ( rendererVisibleModelWarningView != tr_viewCount )
    {
      rendererVisibleModelWarningView = tr_viewCount;
      ri_Printf(2, "too many visible models (more than %i)\n", 1021);
    }
  }
}

/* ---- RE_AddLightToScene  0x004E9B00 ----  VERIFIED */
void __cdecl RE_AddLightToScene(_DWORD *a1, float a2, float a3, float a4, float a5)
{
  double v5;
  int v6;
  double v7;
  int v8;

  if ( tr_registered )
  {
    if ( r_numdlights < 32 && a2 > 0.0 )
    {
      v5 = a2 * a2 * 0.03125;
      v6 = 136 * r_numdlights++ + backEndData + 0x80000;
      *(float *)(v6 + 116) = a2;
      *(float *)(v6 + 16) = v5;
      *(float *)(v6 + 4) = a3;
      *(float *)(v6 + 8) = a4;
      *(float *)(v6 + 12) = a5;
      *(_DWORD *)(v6 + 32) = 1065353216;
      *(_DWORD *)v6 = 2;
      *(_DWORD *)(v6 + 20) = 0;
      *(_DWORD *)(v6 + 24) = 0;
      *(_DWORD *)(v6 + 28) = 0;
      *(float *)(v6 + 36) = tr_identityLight * v5 * a3;
      *(float *)(v6 + 40) = tr_identityLight * v5 * a4;
      v7 = tr_identityLight;
      *(_DWORD *)(v6 + 48) = 1065353216;
      *(_DWORD *)(v6 + 64) = 1065353216;
      *(_DWORD *)(v6 + 52) = 0;
      *(_DWORD *)(v6 + 56) = 0;
      *(_DWORD *)(v6 + 60) = 0;
      *(float *)(v6 + 44) = v7 * v5 * a5;
      *(_DWORD *)(v6 + 68) = *a1;
      *(_DWORD *)(v6 + 72) = a1[1];
      v8 = a1[2];
      *(_DWORD *)(v6 + 80) = 1065353216;
      *(_DWORD *)(v6 + 104) = 1065353216;
      *(_DWORD *)(v6 + 76) = v8;
      *(_DWORD *)(v6 + 96) = 981668463;
      *(_DWORD *)(v6 + 100) = 0;
      *(_DWORD *)(v6 + 108) = 0;
      *(_DWORD *)(v6 + 112) = 1127481344;
    }
  }
}

/* ---- RE_AddCoronaToScene  0x004E9C10 ----  VERIFIED */
void __cdecl RE_AddCoronaToScene(_DWORD *a1, int a2, int a3, int a4, int a5, unsigned int a6, int a7)
{
  _DWORD *v7;

  if ( tr_registered )
  {
    if ( r_numcoronas < 32 )
    {
      if ( a6 >= 0x400 )
      {
        ri_Printf(1, "^3added corona with invalid id %i (should be >= 0 and < %i)\n", a6, 1024);
      }
      else
      {
        v7 = (_DWORD *)(48 * r_numcoronas++ + backEndData + 528640);
        *v7 = *a1;
        v7[1] = a1[1];
        v7[2] = a1[2];
        v7[3] = a2;
        v7[4] = a3;
        v7[5] = a4;
        v7[10] = a6;
        v7[9] = a5;
        v7[11] = a7;
      }
    }
  }
}

/* ---- RE_RenderScene  0x004E9CB0 ----  VERIFIED */
cvar_t *__cdecl RE_RenderScene(int a1)
{
  cvar_t *result;
  int v2;
  int v3;
  int v4;
  double v5;
  int v6;
  int v7;
  int v8;
  int v9;
  int v10;
  int v11;
  int v12;
  int v13;
  int v14;
  int v15;
  long double v16;
  int v17;
  int v18;
  _DWORD v19[154]; // [esp+10h] [ebp-268h] BYREF
  unsigned int retaddr;

  v19[153] = retaddr ^ _security_cookie;
  result = (cvar_t *)tr_registered;
  if ( tr_registered )
  {
    if ( Stream )
      fprintf(Stream, "%s", (int)"====== RE_RenderScene =====\n");
    result = r_norefresh;
    if ( !r_norefresh->integer )
    {
      v18 = ri_Milliseconds();
      if ( !tr_world && (*(_BYTE *)(a1 + 76) & 1) == 0 )
        ri_Error(1, "\x15" "R_RenderScene: NULL worldmodel");
      tr_refdef_x = *(_DWORD *)a1;
      tr_refdef_y = *(_DWORD *)(a1 + 4);
      tr_refdef_width = *(_DWORD *)(a1 + 8);
      v2 = *(_DWORD *)(a1 + 12);
      tr_refdef_height = v2;
      tr_refdef_fov_x = *(float *)(a1 + 16);
      tr_refdef_fov_y = *(float *)(a1 + 20);
      tr_refdef_vieworgX = *(float *)(a1 + 24);
      tr_refdef_vieworgY = *(float *)(a1 + 28);
      tr_refdef_vieworgZ = *(float *)(a1 + 32);
      tr_refdef_viewaxis00 = *(_DWORD *)(a1 + 36);
      tr_refdef_viewaxis01 = *(_DWORD *)(a1 + 40);
      tr_refdef_viewaxis02 = *(_DWORD *)(a1 + 44);
      tr_refdef_viewaxis10 = *(float *)(a1 + 48);
      tr_refdef_viewaxis11 = *(float *)(a1 + 52);
      tr_refdef_viewaxis12 = *(float *)(a1 + 56);
      tr_refdef_viewaxis20 = *(_DWORD *)(a1 + 60);
      tr_refdef_viewaxis21 = *(_DWORD *)(a1 + 64);
      tr_refdef_viewaxis22 = *(_DWORD *)(a1 + 68);
      tr_refdef_time = *(_DWORD *)(a1 + 72);
      tr_refdef_rdflags = *(_DWORD *)(a1 + 76);
      v3 = *(_DWORD *)(a1 + 76);
      if ( (v3 & 8) != 0 )
        dword_16DCA54 = 1;
      dword_16DCA28 = (unsigned __int8)(v3 & 0x10) >> 4;
      tr_refdef_floatTime = (double)tr_refdef_time * 0.001;
      tr_refdef_num_entities = r_numentities - r_firstSceneEntity;
      tr_refdef_numDrawSurfs = r_firstSceneDrawSurf;
      tr_refdef_entitySurfaces = backEndData + 1242880;
      tr_refdef_entities = 696 * r_firstSceneEntity + backEndData + 530176;
      v4 = r_numdlights - r_firstSceneDlight;
      tr_refdef_dlights = 136 * r_firstSceneDlight + backEndData + 0x80000;
      tr_refdef_coronas = 48 * r_firstSceneCorona + backEndData + 528640;
      tr_refdef_coronaCount = r_numcoronas - r_firstSceneCorona;
      tr_refdef_drawSurfs = backEndData;
      tr_refdef_entitySurfaceCount = 0;
      tr_refdef_num_dlights = r_numdlights - r_firstSceneDlight;
      tr_refdef_entityDlightCount = r_numdlights - r_firstSceneDlight;
      tr_refdef_numPolys = r_numpolys - r_firstScenePoly;
      tr_refdef_polys = backEndData + 16 * (r_firstScenePoly + 81776);
      if ( !r_dynamiclight->integer )
        tr_refdef_entityDlightCount = 0;
      if ( r_dynamiclight->integer != 1 )
      {
        v4 = 0;
        tr_refdef_num_dlights = 0;
      }
      if ( r_dynamiclight->integer == 3 )
      {
        ri_Printf(0, "%i dynamic lights in scene\n", v4);
        v2 = tr_refdef_height;
      }
      memset(v19, 0, 0x260u);
      v19[74] = tr_refdef_x;
      v19[75] = dwExStyle - v2 - tr_refdef_y;
      *(float *)&v19[78] = tr_refdef_fov_x;
      *(float *)&v19[79] = tr_refdef_fov_y;
      v19[76] = tr_refdef_width;
      v19[77] = v2;
      v19[65] = 0;
      if ( tr_refdef_fov_x > (double)tr_refdef_fov_y )
        v5 = *(float *)&v19[79];
      else
        v5 = *(float *)&v19[78];
      v6 = *(_DWORD *)(a1 + 28);
      v19[80] = LODWORD(r_lodbias->value);
      v7 = *(_DWORD *)(a1 + 32);
      v19[1] = v6;
      v8 = *(_DWORD *)(a1 + 40);
      v19[2] = v7;
      v9 = *(_DWORD *)(a1 + 44);
      v19[4] = v8;
      v10 = *(_DWORD *)(a1 + 52);
      v19[5] = v9;
      v11 = *(_DWORD *)(a1 + 56);
      v19[7] = v10;
      v12 = *(_DWORD *)(a1 + 64);
      v19[8] = v11;
      v13 = *(_DWORD *)(a1 + 68);
      v19[10] = v12;
      v14 = *(_DWORD *)(a1 + 28);
      v19[11] = v13;
      v15 = *(_DWORD *)(a1 + 32);
      v19[63] = v14;
      v19[64] = v15;
      v16 = tan(v5 * 0.0087266462) / tan(0.6981317400932312) * r_lodscale->value;
      v19[0] = *(_DWORD *)(a1 + 24);
      v17 = *(_DWORD *)(a1 + 36);
      *(float *)&v19[81] = v16;
      v19[3] = v17;
      v19[6] = *(_DWORD *)(a1 + 48);
      v19[9] = *(_DWORD *)(a1 + 60);
      v19[62] = *(_DWORD *)(a1 + 24);
      R_RenderView(v19);
      r_firstSceneDrawSurf = tr_refdef_numDrawSurfs;
      r_firstSceneEntity = r_numentities;
      r_firstSceneDlight = r_numdlights;
      r_firstScenePoly = r_numpolys;
      result = (cvar_t *)(ri_Milliseconds() - v18);
      tr_frontEndMsec += (int)result;
    }
  }
  return result;
}


/* ---- R_GetLodDist  0x004EA0C0 ----  VERIFIED */
double __cdecl R_GetLodDist(float *a1)
{
  float v2;
  float v3;
  float v4;
  float v5;
  float v6;

  v4 = tr_viewParms_originX - a1[17];
  v5 = tr_viewParms_originY - a1[18];
  v6 = tr_viewParms_originZ - a1[19];
  v2 = sqrt(v6 * v6 + v5 * v5 + v4 * v4);
  v3 = tr_viewParms_lodScale * v2 + tr_viewParms_lodBias;
  if ( !((a1[16] == 0.0) | __UNORDERED__(a1[16], 0.0)) )
    return (float)(v3 / a1[16]);
  return v3;
}

