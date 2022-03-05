#include "Context.hpp"

using namespace AGGWrap;

AGGWrap::DrawingContext::DrawingContext(Bitmap& rbmTarget) : m_rbmTarget(rbmTarget) {
	m_bFast = false;
}

void AGGWrap::DrawingContext::FillAll(const Brush& rbrush) {
	FillRectangle(rbrush, 0, 0, m_rbmTarget.GetWidth(), m_rbmTarget.GetHeight());
}

void AGGWrap::DrawingContext::FillPath(const Brush& rbrush, const Path& rpath) {
	agg::rasterizer_scanline_aa<> rast;
	agg::renderer_base<Bitmap::PixelFormat> rend(m_rbmTarget.GetFormatAdaptor());

	agg::path_storage storTemp(rpath.GetStorage());
	rast.add_path(storTemp);
	rbrush.PerformFill(rend, rast, m_bFast);
}

void AGGWrap::DrawingContext::FillRectangle(const Brush& rbrush, AwGenCoord_t x, AwGenCoord_t y, AwGenCoord_t w, AwGenCoord_t h) {
	Path pathRect;
	pathRect.AddRectangle(x, y, w, h);
	FillPath(rbrush, pathRect);
}

void AGGWrap::DrawingContext::DrawPath(const Pen& rpen, const Path& rpath) {
	if (rpen.GetParams().fThickness <= 0.0f)
		return;

	Path pathStroke;
	rpen.StrokePath(rpath, pathStroke);
	FillPath(rpen.GetBrush(), pathStroke);
}

void AGGWrap::DrawingContext::DrawRectangle(const Pen& rpen, AwGenCoord_t x, AwGenCoord_t y, AwGenCoord_t w, AwGenCoord_t h) {
	Path pathRect;
	pathRect.AddRectangle(x, y, w, h);
	DrawPath(rpen, pathRect);
}

void AGGWrap::DrawingContext::DrawLine(const Pen& rpen, AwGenCoord_t x1, AwGenCoord_t y1, AwGenCoord_t x2, AwGenCoord_t y2) {
	Path pathLine;
	pathLine.AddLine(x1, y1, x2, y2);
	DrawPath(rpen, pathLine);
}

AGGWRAP_EXPIMPL AwContext_h AGGWRAP_FUNC AwCreateContextOnBitmap(AwBitmap_h hBitmap) {
	try {
		return (AwContext_h)new DrawingContext(*(AGGWrap::Bitmap*)hBitmap);
	} catch (...) {
		return nullptr;
	}
}

AGGWRAP_EXPORTED AwBool_t AGGWRAP_FUNC AwSetDrawQuality(AwContext_h hContext, AwQuality_t qual) {
	try {
		((DrawingContext*)hContext)->EnableFastMode(qual == AwQuality_Fast);
		return AGGWRAP_TRUE;
	} catch (...) {
		return AGGWRAP_FALSE;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwFillContext(AwContext_h hContext, AwBrush_h hBrush) {
	try {
		((DrawingContext*)hContext)->FillAll(Brush::FromHandle(hBrush));
		return AGGWRAP_TRUE;
	} catch (...) {
		return AGGWRAP_FALSE;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwFillPath(AwContext_h hContext, AwBrush_h hBrush, AwPath_h hPath) {
	try {
		((DrawingContext*)hContext)->FillPath(Brush::FromHandle(hBrush), *(Path*)hPath);
		return AGGWRAP_TRUE;
	} catch (...) {
		return AGGWRAP_FALSE;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwFillRectangle(AwContext_h hContext, AwBrush_h hBrush, AwGenCoord_t x, AwGenCoord_t y, AwGenCoord_t w, AwGenCoord_t h) {
	try {
		((DrawingContext*)hContext)->FillRectangle(Brush::FromHandle(hBrush), x, y, w, h);
		return AGGWRAP_TRUE;
	} catch (...) {
		return AGGWRAP_FALSE;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwDrawPath(AwContext_h hContext, AwPen_h hPen, AwPath_h hPath) {
	try {
		((DrawingContext*)hContext)->DrawPath(Pen::FromHandle(hPen), *(Path*)hPath);
		return AGGWRAP_TRUE;
	} catch (...) {
		return AGGWRAP_FALSE;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwDrawRectangle(AwContext_h hContext, AwPen_h hPen, AwGenCoord_t x, AwGenCoord_t y, AwGenCoord_t w, AwGenCoord_t h) {
	try {
		((DrawingContext*)hContext)->DrawRectangle(Pen::FromHandle(hPen), x, y, w, h);
		return AGGWRAP_TRUE;
	} catch (...) {
		return AGGWRAP_FALSE;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwDrawLine(AwContext_h hContext, AwPen_h hPen, AwGenCoord_t x1, AwGenCoord_t y1, AwGenCoord_t x2, AwGenCoord_t y2) {
	try {
		((DrawingContext*)hContext)->DrawLine(Pen::FromHandle(hPen), x1, y1, x2, y2);
		return AGGWRAP_TRUE;
	} catch (...) {
		return AGGWRAP_FALSE;
	}
}

AGGWRAP_EXPIMPL void AGGWRAP_FUNC AwDeleteContext(AwContext_h hContext) {
	try {
		delete (DrawingContext*)hContext;
	} catch (...) { }
}
