#pragma once

#ifndef AGGWRAP_HEADER_CONTEXT
#define AGGWRAP_HEADER_CONTEXT

#include "Bitmap.hpp"
#include "Brush.hpp"
#include "Path.hpp"
#include "Pen.hpp"
#include "Font.hpp"

namespace AGGWrap {
	class DrawingContext {
	public:
		DrawingContext(Bitmap& rbmTarget);

		void FillAll(const Brush& rbrush);
		void FillPath(const Brush& rbrush, const Path& rpath);
		void FillRectangle(const Brush& rbrush, AwGenCoord_t x, AwGenCoord_t y, AwGenCoord_t w, AwGenCoord_t h);
		void FillText(const Brush& rbrush, const Font& rfont, AwPathCoord_t x, AwPathCoord_t y, const Array<Codepoint>& rarrCodepoints);
		void FillText(const Brush& rbrush, const Font& rfont, AwPathCoord_t x, AwPathCoord_t y, const char* pcszText);
		
		void DrawPath(const Pen& rpen, const Path& rpath);
		void DrawRectangle(const Pen& rpen, AwGenCoord_t x, AwGenCoord_t y, AwGenCoord_t w, AwGenCoord_t h);
		void DrawLine(const Pen& rpen, AwGenCoord_t x1, AwGenCoord_t y1, AwGenCoord_t x2, AwGenCoord_t y2);

		void Blit(const Bitmap& rbmSource, AwGenCoord_t nSourceX, AwGenCoord_t nSourceY, AwGenCoord_t nSourceW, AwGenCoord_t nSourceH,
			AwGenCoord_t nDestX, AwGenCoord_t nDestY, AwGenCoord_t nDestW, AwGenCoord_t nDestH);

		inline void EnableFastMode(bool bFast) { m_bFast = bFast; }

	private:
		AGGWRAP_NOCOPY(DrawingContext, DrawingContext);
		AGGWRAP_NOASSIGN(DrawingContext);
		Bitmap& m_rbmTarget;
		bool m_bFast;
	};
}

#endif
