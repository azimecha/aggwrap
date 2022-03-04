#include "Path.hpp"
#include <agg_ellipse.h>
#include <agg_arc.h>

using namespace AGGWrap;

void AGGWrap::Path::AddRectangle(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h) {
	m_storage.move_to(x, y);
	m_storage.hline_to(x + w);
	m_storage.vline_to(y + h);
	m_storage.hline_to(x);
	m_storage.vline_to(y);
	m_storage.close_polygon();
}

void AGGWrap::Path::AddRoundedRectangle(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fRadius) {
	// Start after top left corner
	m_storage.move_to(x + fRadius, y);

	// Line to before top right corner
	m_storage.line_to(x + w - fRadius, y);

	// Arc to end of top right corner
	//m_storage.arc_to(fRadius, fRadius, M_PI * 1.5, false, false, x + w, y + fRadius);
	m_storage.curve3(x + w, y, x + w, y + fRadius);

	// Line to start of bottom right corner
	m_storage.line_to(x + w, y + h - fRadius);

	// Bottom right corner
	//m_storage.arc_to(fRadius, fRadius, M_PI * 1.5, false, false, x + w - fRadius, y + h);
	m_storage.curve3(x + w, y + h, x + w - fRadius, y + h);

	// Line to start of bottom left corner
	m_storage.line_to(x + fRadius, y + h);

	// Bottom left corner
	//m_storage.arc_to(fRadius, fRadius, M_PI * 1.5, false, false, x, y + h - fRadius);
	m_storage.curve3(x, y + h, x, y + h - fRadius);

	// Line to start of top left corner
	m_storage.line_to(x, y + fRadius);

	// Top left corner
	//m_storage.arc_to(fRadius, fRadius, M_PI * 1.5, false, false, x + fRadius, y);
	m_storage.curve3(x, y, x + fRadius, y);

	// Done
	m_storage.close_polygon();
}

void AGGWrap::Path::AddCircle(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t fRadius) {
	agg::ellipse ell(x, y, fRadius, fRadius);
	ell.approximation_scale(1);
	m_storage.concat_path(ell);
}

void AGGWrap::Path::AddEllipse(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h) {
	double cx = x + w / 2;
	double cy = y + h / 2;
	double rx = w / 2;
	double ry = h / 2;

	agg::ellipse ell(cx, cy, rx, ry);
	ell.approximation_scale(1);
	m_storage.concat_path(ell);
}

void AGGWrap::Path::AddArc(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fStartAngle, AwPathCoord_t fSweepAngle) {
	float x0 = x;
	float y0 = y;
	float x1 = x + w;
	float y1 = y + h;

	agg::arc arc((x1 + x0) / 2, (y1 + y0) / 2, (x1 - x0) / 2, (y1 - y0) / 2,
		-fStartAngle, -fSweepAngle, false);
	arc.approximation_scale(1);
	m_storage.concat_path(arc);
}

void AGGWrap::Path::AddChord(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fStartAngle, AwPathCoord_t fSweepAngle) {
	AddArc(x, y, w, h, fStartAngle, fSweepAngle);
	m_storage.close_polygon();
}

void AGGWrap::Path::AddPieSlice(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fStartAngle, AwPathCoord_t fSweepAngle) {
	float x0 = x;
	float y0 = y;
	float x1 = x + w;
	float y1 = y + h;
	float cx = (x1 + x0) / 2;
	float cy = (y1 + y0) / 2;

	agg::arc arc(cx, cy, (x1 - x0) / 2, (y1 - y0) / 2, -fStartAngle, -fSweepAngle, false);
	arc.approximation_scale(1);
	m_storage.concat_path(arc);
	m_storage.line_to(x, y);
	m_storage.close_polygon();
}

void AGGWrap::Path::AddLine(AwPathCoord_t x1, AwPathCoord_t y1, AwPathCoord_t x2, AwPathCoord_t y2) {
	m_storage.move_to(x1, y1);
	m_storage.line_to(x2, y2);
}

void AGGWrap::Path::AddTriangle(AwPathCoord_t x1, AwPathCoord_t y1, AwPathCoord_t x2, AwPathCoord_t y2, AwPathCoord_t x3, AwPathCoord_t y3) {
	m_storage.move_to(x1, y1);
	m_storage.line_to(x2, y2);
	m_storage.line_to(x3, y3);
	m_storage.line_to(x1, y1);
	m_storage.close_polygon();
}

void AGGWrap::Path::AddPolyline(AwPathPoint_p parrPoints, int nPoints) {
	if (nPoints == 0) return;

	m_storage.move_to(parrPoints[0].x, parrPoints[0].y);
	for (size_t n = 1; n < nPoints; n++)
		m_storage.line_to(parrPoints[n].x, parrPoints[n].y);
}

void AGGWrap::Path::AddPolygon(AwPathPoint_p parrPoints, int nPoints) {
	if (nPoints == 0) return;

	AddPolyline(parrPoints, nPoints);
	m_storage.move_to(parrPoints[0].x, parrPoints[0].y);
	m_storage.close_polygon();
}

void AGGWrap::Path::AddPath(const Path& rpath) {
	m_storage.concat_path(rpath.m_storage);
}

void AGGWrap::Path::MoveTo(AwPathCoord_t x, AwPathCoord_t y) {
	m_storage.move_to(x, y);
}

void AGGWrap::Path::LineTo(AwPathCoord_t x, AwPathCoord_t y) {
	m_storage.line_to(x, y);
}

void AGGWrap::Path::CloseShape(void) {
	m_storage.close_polygon();
}

void AGGWrap::Path::Clear(void) {
	m_storage.remove_all();
}

AGGWRAP_EXPIMPL AwPath_h AGGWRAP_FUNC AwCreatePath(void) {
	try {
		return (AwPath_h)new Path();
	} catch (...) {
		return nullptr;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwAddPathRectangle(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h) {
	try {
		((Path*)hPath)->AddRectangle(x, y, w, h);
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwAddPathRoundedRectangle(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h,
	AwPathCoord_t fRadius) 
{
	try {
		((Path*)hPath)->AddRoundedRectangle(x, y, w, h, fRadius);
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwAddPathCircle(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t fRadius) {
	try {
		((Path*)hPath)->AddCircle(x, y, fRadius);
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwAddPathEllipse(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h) {
	try {
		((Path*)hPath)->AddEllipse(x, y, w, h);
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwAddPathArc(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fStartAngle,
	AwPathCoord_t fSweepAngle) 
{
	try {
		((Path*)hPath)->AddArc(x, y, w, h, fStartAngle, fSweepAngle);
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwAddPathChord(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fStartAngle,
	AwPathCoord_t fSweepAngle) 
{
	try {
		((Path*)hPath)->AddChord(x, y, w, h, fStartAngle, fSweepAngle);
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwAddPathPieSlice(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fStartAngle,
	AwPathCoord_t fSweepAngle)
{
	try {
		((Path*)hPath)->AddPieSlice(x, y, w, h, fStartAngle, fSweepAngle);
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwAddPathLine(AwPath_h hPath, AwPathCoord_t x1, AwPathCoord_t y1, AwPathCoord_t x2, AwPathCoord_t y2){
	try {
		((Path*)hPath)->AddLine(x1, y1, x2, y2);
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwAddPathTriangle(AwPath_h hPath, AwPathCoord_t x1, AwPathCoord_t y1, AwPathCoord_t x2, AwPathCoord_t y2, AwPathCoord_t x3,
	AwPathCoord_t y3)
{
	try {
		((Path*)hPath)->AddTriangle(x1, y1, x2, y2, x3, y3);
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwAddPathPolyline(AwPath_h hPath, AwPathPoint_p parrPoints, int nPoints){
	try {
		((Path*)hPath)->AddPolyline(parrPoints, nPoints);
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwAddPathPolygon(AwPath_h hPath, AwPathPoint_p parrPoints, int nPoints){
	try {
		((Path*)hPath)->AddPolygon(parrPoints, nPoints);
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwAddPath(AwPath_h hTargetPath, AwPath_h hPathToAdd) {
	try {
		((Path*)hTargetPath)->AddPath(*(Path*)hPathToAdd);
		return true;
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwPathMoveTo(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y) {
	try {
		((Path*)hPath)->MoveTo(x, y);
		return true;
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwPathLineTo(AwPath_h hPath, AwPathCoord_t x, AwPathCoord_t y) {
	try {
		((Path*)hPath)->LineTo(x, y);
		return true;
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwClosePathShape(AwPath_h hPath) {
	try {
		((Path*)hPath)->CloseShape();
		return true;
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwClearPath(AwPath_h hPath) {
	try {
		((Path*)hPath)->Clear();
		return true;
	} catch (...) {
		return false;
	}
}

AGGWRAP_EXPIMPL void AGGWRAP_FUNC AwDeletePath(AwPath_h hPath) {
	try {
		delete (Path*)hPath;
	} catch (...) { }
}
