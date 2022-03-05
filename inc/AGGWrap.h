#pragma once

#ifndef AGGWRAP_HEADER
#define AGGWRAP_HEADER

#if defined(AGGWRAP_SHARED) && defined(WIN32)
	#define AGGWRAP_FUNC __stdcall
	#ifdef AGGWRAP_BUILD
		#define AGGWRAP_EXPORTED extern "C" __declspec(dllexport)
	#else
		#define AGGWRAP_EXPORTED extern "C" __declspec(dllimport)
	#endif
#else
	#define AGGWRAP_FUNC
	#define AGGWRAP_EXPORTED extern "C" 
#endif

typedef unsigned char AwByte_t;
typedef int AwBool_t;
typedef float AwAngle_t;
typedef float AwPathCoord_t;
typedef float AwGradientPos_t;
typedef float AwThickness_t;
typedef int AwGenCoord_t;

typedef AwByte_t* AwByte_p;
typedef AwBool_t* AwBool_p;
typedef AwAngle_t* AwAngle_p;
typedef AwPathCoord_t* AwPathCoord_p;
typedef AwGradientPos_t* AwGradientPos_p;
typedef AwThickness_t* AwThickness_p;
typedef AwGenCoord_t* AwGenCoord_p;

#define AGGWRAP_TRUE 1
#define AGGWRAP_FALSE 0

#define AGGWRAP_OPTIONAL
#define AGGWRAP_OUT

typedef void (AGGWRAP_FUNC* AwBufferDestructor_t)(void* pData, void* tag);

typedef struct AwBufferInfo_s {
	void* pData;
	unsigned long nDataSize;
	AwBufferDestructor_t procDestructor;
	void* tag;
} AwBufferInfo_t, *AwBufferInfo_p;

typedef struct AwColor_s {
	AwByte_t r;
	AwByte_t g;
	AwByte_t b;
	AwByte_t a;
} AwColor_t, *AwColor_p;

typedef enum AwScaleMode_e {
	AwScaleMode_OriginalSize,
	AwScaleMode_Fit,
	AwScaleMode_Fill,
	AwScaleMode_Stretch
} AwScaleMode_t;

typedef struct AwPathPoint_s {
	AwPathCoord_t x;
	AwPathCoord_t y;
} AwPathPoint_t, *AwPathPoint_p;

typedef struct AwGradientStop_s {
	AwGradientPos_t pos; // 0 - 1
	AwColor_t clr;
} AwGradientStop_t, *AwGradientStop_p;

typedef struct AwPenParams_s {
	AwThickness_t fThickness;
	AwBool_t bRounded;
} AwPenParams_t, *AwPenParams_p;

typedef enum AwQuality_e {
	AwQuality_Good,
	AwQuality_Fast
} AwQuality_t;

typedef const struct AwPenParams_s* AwPenParams_pc;

typedef void* AwBitmap_h;
typedef void* AwBrush_h;
typedef void* AwPath_h;
typedef void* AwPen_h;
typedef void* AwContext_h;

AGGWRAP_EXPORTED AwBitmap_h AGGWRAP_FUNC AwCreateBitmap(int w, int h);
AGGWRAP_EXPORTED AwBitmap_h AGGWRAP_FUNC AwCreateBitmapOnBuffer(int w, int h, AwBufferInfo_p pinfBuffer);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwGetBitmapSize(AwBitmap_h hBitmap, AGGWRAP_OUT int* pnWidth, AGGWRAP_OUT int* pnHeight);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwGetBitmapDataSize(AwBitmap_h hBitmap, AGGWRAP_OPTIONAL AGGWRAP_OUT int* pnStride, AGGWRAP_OPTIONAL AGGWRAP_OUT int* pnFullSize);
AGGWRAP_EXPORTED AwBitmap_h AGGWRAP_FUNC AwDuplicateBitmap(AwBitmap_h hBitmap);
AGGWRAP_EXPORTED void AGGWRAP_FUNC AwDeleteBitmap(AwBitmap_h hBitmap);

AGGWRAP_EXPORTED AwBrush_h AGGWRAP_FUNC AwCreateSolidBrush(AwByte_t r, AwByte_t g, AwByte_t b, AwByte_t a);
AGGWRAP_EXPORTED AwBrush_h AGGWRAP_FUNC AwCreateLinearGradientBrush(AwGradientStop_p parrStops, int nStops, AwAngle_t fAngle);
AGGWRAP_EXPORTED AwBrush_h AGGWRAP_FUNC AwCreatePatternBrush(AwBitmap_h hBitmap);
AGGWRAP_EXPORTED AwBrush_h AGGWRAP_FUNC AwCreateScaledBrush(AwBitmap_h hBitmap, AwScaleMode_t mode);
AGGWRAP_EXPORTED void AGGWRAP_FUNC AwDeleteBrush(AwBrush_h hBrush);

AGGWRAP_EXPORTED AwPath_h AGGWRAP_FUNC AwCreatePath(void);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwAddPathRectangle(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwAddPathRoundedRectangle(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, 
	AwPathCoord_t fRadius);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwAddPathCircle(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t fRadius);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwAddPathEllipse(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwAddPathArc(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fStartAngle, 
	AwPathCoord_t fSweepAngle);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwAddPathChord(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fStartAngle, 
	AwPathCoord_t fSweepAngle);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwAddPathPieSlice(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fStartAngle, 
	AwPathCoord_t fSweepAngle);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwAddPathLine(AwPath_h hPath, AwPathCoord_t x1, AwPathCoord_t y1, AwPathCoord_t x2, AwPathCoord_t y2);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwAddPathTriangle(AwPath_h hPath, AwPathCoord_t x1, AwPathCoord_t y1, AwPathCoord_t x2, AwPathCoord_t y2, AwPathCoord_t x3, 
	AwPathCoord_t y3);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwAddPathPolyline(AwPath_h hPath, AwPathPoint_p parrPoints, int nPoints);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwAddPathPolygon(AwPath_h hPath, AwPathPoint_p parrPoints, int nPoints);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwAddPath(AwPath_h hTargetPath, AwPath_h hPathToAdd);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwPathMoveTo(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwPathLineTo(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwClosePathShape(AwPath_h hPath);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwClearPath(AwPath_h hPath);
AGGWRAP_EXPORTED void AGGWRAP_FUNC AwDeletePath(AwPath_h hPath);

AGGWRAP_EXPORTED AwPen_h AGGWRAP_FUNC AwCreatePen(AwBrush_h hBrush, AwPenParams_pc pcParams);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwGetPenParams(AwPen_h hPen, AGGWRAP_OUT AwPenParams_p pParams);
AGGWRAP_EXPORTED void AGGWRAP_FUNC AwDeletePen(AwPen_h hPen);

AGGWRAP_EXPORTED AwContext_h AGGWRAP_FUNC AwCreateContextOnBitmap(AwBitmap_h hBitmap);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwSetDrawQuality(AwContext_h hContext, AwQuality_t qual);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwFillContext(AwContext_h hContext, AwBrush_h hBrush);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwFillPath(AwContext_h hContext, AwBrush_h hBrush, AwPath_h hPath);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwFillRectangle(AwContext_h hContext, AwBrush_h hBrush, AwGenCoord_t x, AwGenCoord_t y, AwGenCoord_t w, AwGenCoord_t h);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwDrawPath(AwContext_h hContext, AwPen_h hPen, AwPath_h hPath);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwDrawRectangle(AwContext_h hContext, AwPen_h hPen, AwGenCoord_t x, AwGenCoord_t y, AwGenCoord_t w, AwGenCoord_t h);
AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwDrawLine(AwContext_h hContext, AwPen_h hPen, AwGenCoord_t x1, AwGenCoord_t y1, AwGenCoord_t x2, AwGenCoord_t y2);
AGGWRAP_EXPORTED void AGGWRAP_FUNC AwDeleteContext(AwContext_h hContext);

#endif
