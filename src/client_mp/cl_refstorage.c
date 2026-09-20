/*
 * @fidelity: verified
 */

#include <string.h>

#include "../qcommon/qcommon.h"
#include "cl_refapi.h"

glconfig_t cls_glconfig;

unsigned char   unk_1565008[376832];        /* 0x5C000 bytes, 0x01565008 */

unsigned char   cl_active[1231064];         /* 0x12C8D8 bytes, 0x01432960 */

int             tr_registered[20213];       /* 0x13BD4 bytes, 0x016C4D40 */
unsigned char   unk_16D89C0[6732];          /* 0x1A4C bytes, 0x016D89C0 */
short           tess_indexes[1097780];      /* 0x218068 bytes, 0x017A7F60 */
unsigned char   tr_lightVisCache[2097152];        /* 0x200000 bytes, 0x00CA28D0 */

typedef struct {
    const char *name;
    int         a;
    int         surfaceBits;
    int         b;
} cod1_surfacetype_t;

cod1_surfacetype_t dword_571790[24] = {
    { (const char *) 13, -1, -1, 0 },
    { "bark",        0, 0x00100000, 0x00 },
    { "brick",       0, 0x00200000, 0x00 },
    { "carpet",      0, 0x00300000, 0x00 },
    { "cloth",       0, 0x00400000, 0x00 },
    { "concrete",    0, 0x00500000, 0x00 },
    { "dirt",        0, 0x00600000, 0x00 },
    { "flesh",       0, 0x00700000, 0x00 },
    { "foliage",     1, 0x00800000, 0x02 },
    { "glass",       1, 0x00900000, 0x10 },
    { "grass",       0, 0x00A00000, 0x00 },
    { "gravel",      0, 0x00B00000, 0x00 },
    { "ice",         0, 0x00C00000, 0x00 },
    { "metal",       0, 0x00D00000, 0x00 },
    { "mud",         0, 0x00E00000, 0x00 },
    { "paper",       0, 0x00F00000, 0x00 },
    { "plaster",     0, 0x01000000, 0x00 },
    { "rock",        0, 0x01100000, 0x00 },
    { "sand",        0, 0x01200000, 0x00 },
    { "snow",        0, 0x01300000, 0x00 },
    { "water",       1, 0x01400000, 0x20 },
    { "wood",        0, 0x01500000, 0x00 },
    { "asphalt",     0, 0x01600000, 0x00 },
    { "opaqueglass", 0, 0x00900000, 0x00 }
};

/* The renderer's own glConfig, 0x016C3A60, one name per field ------------------------------------------------------------------ */

extern int      glConfig_renderer_string;           /* +0x00 016C3A60 */
extern int      glConfig_vendor_string;             /* +0x04 016C3A64 */
extern int      glConfig_version_string;            /* +0x08 016C3A68 */
extern int      glConfig_extensions_string;         /* +0x0C 016C3A6C */
extern int      glConfig_wextensions_string;        /* +0x10 016C3A70 */
extern int      glConfig_maxTextureSize;            /* +0x14 016C3A74 */
extern int      Value;                              /* +0x18 016C3A78 maxActiveTextures */
extern int      glConfig_maxLights;                      /* +0x1C 016C3A7C maxLights */
extern int      glConfig_colorBits;                 /* +0x20 016C3A80 */
extern int      glConfig_depthBits;                 /* +0x24 016C3A84 */
extern int      glConfig_stencilBits;               /* +0x28 016C3A88 */
extern int      glConfig_deviceSupportsGamma;       /* +0x2C 016C3A8C */
extern int      glConfig_maxAnisotropy;                      /* +0x30 */
extern int      glConfig_maxTextureFilterAnisotropy;                      /* +0x34 */
extern int      glConfig_textureEnvAddAvailable;    /* +0x38 016C3A98 */
extern int      glConfig_textureCubeMap;                      /* +0x3C */
extern int      glConfig_textureEnvCombine;                      /* +0x40 */
extern int      glConfig_textureEnvDot3;                      /* +0x44 */
extern int      glConfig_ARBVertexBufferObject;                      /* +0x48 */
extern int      glConfig_ARBVertexProgram;                      /* +0x4C */
extern int      glConfig_rescaleNormal;                      /* +0x50 */
extern int      glConfig_NVFogAvailable;            /* +0x54 016C3AB4 */
extern int      glConfig_NVFogMode;                      /* +0x58 */
extern int      glConfig_NVVertexArrayRange;                      /* +0x5C smpActive */
extern int      glConfig_NVRegisterCombiners;                      /* +0x64 */
extern int      glConfig_NVTextureShader;                      /* +0x68 */
extern int      glConfig_maxPNTrianglesTessellationLevel;                      /* +0x6C */
extern int      glConfig_pnTrianglesNormalMode;                      /* +0x70 */
extern int      glConfig_pnTrianglesPointMode;                      /* +0x74 */
extern int      glConfig_ATIVertexArrayObject;                      /* +0x78 */
extern int      glConfig_ATIElementArray;                      /* +0x7C */
extern int      glConfig_ATIFragmentShader;                      /* +0x80 */
extern int      dwStyle;                            /* +0x84 016C3AE4 vidWidth */
extern int      dwExStyle;                          /* +0x88 016C3AE8 vidHeight */
extern float    glConfig_windowAspect;              /* +0x8C 016C3AEC, win_glimp.c */
extern int      glConfig_displayFrequency;                      /* +0x90 displayFrequency */
extern int      glConfig_isFullscreen;                      /* +0x94 isFullscreen */
extern int      glConfig_stereoEnabled;                      /* +0x98 stereoEnabled */

extern int      cls_glconfig_vidWidth;      /* 0x015CA614, cls.glconfig +0x84 */
extern int      cls_glconfig_vidHeight;     /* 0x015CA618, cls.glconfig +0x88 */
extern int      dword_15CA628;              /* 0x015CA628, cls.glconfig +0x98 */

extern int      whiteShader;                /* 0x015CA630 */
extern int      dword_15CA634;              /* 0x015CA634, the console shader */
extern int      dword_57C0F8;               /* 0x0057C0F8 */
extern int      dword_57C0FC;               /* 0x0057C0FC */
extern int      dword_57C100;               /* 0x0057C100 */

field_t         g_consoleField;             /* 0x0142F640 */
field_t         chatField;                  /* 0x014303A0 */
field_t         historyEditLines[32];       /* 0x014304C0, 9088 == 32 * 284 */

extern void     StatMon_Reset( void );      /* qcommon/statmonitor.c, 0x00451A60 */

/* ---- CL_GlConfigFromRenderer  no-address ---- */
static void CL_GlConfigFromRenderer( void )
{
    cls_glconfig.renderer_string        = (char *) glConfig_renderer_string;
    cls_glconfig.vendor_string          = (char *) glConfig_vendor_string;
    cls_glconfig.version_string         = (char *) glConfig_version_string;
    cls_glconfig.extensions_string      = (char *) glConfig_extensions_string;
    cls_glconfig.wextensions_string     = (char *) glConfig_wextensions_string;

    cls_glconfig.maxTextureSize         = glConfig_maxTextureSize;
    cls_glconfig.maxActiveTextures      = Value;
    cls_glconfig.maxLights              = glConfig_maxLights;

    cls_glconfig.colorBits              = glConfig_colorBits;
    cls_glconfig.depthBits              = glConfig_depthBits;
    cls_glconfig.stencilBits            = glConfig_stencilBits;
    cls_glconfig.deviceSupportsGamma    = glConfig_deviceSupportsGamma;

    cls_glconfig.unk_30                 = glConfig_maxAnisotropy;
    cls_glconfig.unk_34                 = glConfig_maxTextureFilterAnisotropy;
    cls_glconfig.textureEnvAddAvailable = glConfig_textureEnvAddAvailable;
    cls_glconfig.unk_3C                 = glConfig_textureCubeMap;
    cls_glconfig.unk_40                 = glConfig_textureEnvCombine;
    cls_glconfig.unk_44                 = glConfig_textureEnvDot3;
    cls_glconfig.unk_48                 = glConfig_ARBVertexBufferObject;
    cls_glconfig.unk_4C                 = glConfig_ARBVertexProgram;
    cls_glconfig.unk_50                 = glConfig_rescaleNormal;
    cls_glconfig.NVFogAvailable         = glConfig_NVFogAvailable;
    cls_glconfig.unk_58                 = glConfig_NVFogMode;
    cls_glconfig.unk_5C                 = glConfig_NVVertexArrayRange;
    cls_glconfig.unk_64                 = glConfig_NVRegisterCombiners;
    cls_glconfig.unk_68                 = glConfig_NVTextureShader;
    cls_glconfig.unk_6C                 = glConfig_maxPNTrianglesTessellationLevel;
    cls_glconfig.unk_70                 = glConfig_pnTrianglesNormalMode;
    cls_glconfig.unk_74                 = glConfig_pnTrianglesPointMode;
    cls_glconfig.unk_78                 = glConfig_ATIVertexArrayObject;
    cls_glconfig.unk_7C                 = glConfig_ATIElementArray;
    cls_glconfig.unk_80                 = glConfig_ATIFragmentShader;

    cls_glconfig.vidWidth               = dwStyle;
    cls_glconfig.vidHeight              = dwExStyle;
    cls_glconfig.windowAspect           = glConfig_windowAspect;
    cls_glconfig.displayFrequency       = glConfig_displayFrequency;
    cls_glconfig.isFullscreen           = glConfig_isFullscreen;
    cls_glconfig.stereoEnabled          = glConfig_stereoEnabled;

    cls_glconfig_vidWidth  = cls_glconfig.vidWidth;
    cls_glconfig_vidHeight = cls_glconfig.vidHeight;
    dword_15CA628          = cls_glconfig.stereoEnabled;
}

/* ---- CL_InitRenderer  0x00411540 ---- */
void CL_InitRenderer( void )
{
    CL_BssDiff( "CL_InitRef -> CL_InitRenderer" );

    if ( !cls_rendererBound || !re.BeginRegistration ) {
        Com_Printf( "CL_InitRenderer: refexport_t is not bound; "
                    "skipping renderer registration\n" );
        return;
    }

    if ( Cvar_Get( "cl_skipRenderer", "0", 0 )->integer ) {
        Com_Printf( "CL_InitRenderer: cl_skipRenderer is set; "
                    "renderer NOT initialised\n" );
        return;
    }

    re.BeginRegistration( "", &cls_glconfig );

    CL_GlConfigFromRenderer();

    Com_Printf( "CL_InitRenderer: %d x %d, %d bpp, %s\n",
                cls_glconfig.vidWidth, cls_glconfig.vidHeight,
                cls_glconfig.colorBits,
                cls_glconfig.isFullscreen ? "fullscreen" : "windowed" );

    whiteShader   = re.RegisterShader( "white", 2 );
    dword_15CA634 = re.RegisterShader( "console", 2 );

    dword_57C0F8  = cls_glconfig.vidWidth - 32;
    g_consoleField.widthInPixels = cls_glconfig.vidWidth - 32;

    *(int *)&g_consoleField.charHeight = dword_57C100;
    *(int *)&g_consoleField.charWidth  = dword_57C0FC;

    StatMon_Reset();

    g_consoleField.fixedWidth = 1;
}

int dword_142F780[768];

float yaw;          /* 0x01432A44 cl.snap.ps.viewangles[YAW] */
float leanFrac;     /* 0x014329C0 cl.snap.ps.leanFraction */

int cl_snap_valid[2112];

int cl_gameState_stringOffsets[6049];   /* 0x5E84 bytes, 0x01434A7C */

/* g_color_table (0x00541950) is universal/q_shared.c's, in the .rdata block it
 * shares with the colorBlack run. */

const char byte_5676E8[45] = "\x15" "CL_ReadDemoMessage: demoMsglen > MAX_MSGLEN";

const char *svc_strings[256] = {
	"svc_bad",
	"svc_nop",
	"svc_gamestate",
	"svc_configstring",
	"svc_baseline",
	"svc_serverCommand",
	"svc_download",
	"svc_snapshot",
};

const char *off_57B4B4[6] = { "_up", "_dn", "_lf", "_rt", "_ft", "_bk" };

char cl_cdkeychecksum[10] = "    ";

short cin_sqrTable[256];

char clc_reliableCommands[65536];

unsigned char unk_15F7080[131712];

unsigned char byte_155F260[64];

unsigned char   cin_file[65536];               /* 0x0077E910 cin.file */
int             cin_mcomp[256];          /* 0x0078EB10 cin.mcomp[256] */
int             cin_qStatus0[32768];        /* 0x0078EF10 cin.qStatus[0] */
int             cin_linbuf[524288];       /* 0x0057E910 cin.linbuf */
unsigned char   cin_vq4[131072];         /* 0x007CEF28 vq4 */
unsigned char   cin_vq8[524288];         /* 0x007EF728 vq8 */
