#pragma once

#ifndef AGGWRAP_HEADER_PATH
#define AGGWRAP_HEADER_PATH

#include "Types.hpp"
#include <agg_path_storage.h>

namespace AGGWrap {
	class Path {
	public:
		void AddRectangle(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h);
		void AddRoundedRectangle(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fRadius);
		void AddCircle(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t fRadius);
		void AddEllipse(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h);
		void AddArc(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fStartAngle, AwPathCoord_t fSweepAngle);
		void AddChord(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fStartAngle, AwPathCoord_t fSweepAngle);
		void AddPieSlice(AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h, AwPathCoord_t fStartAngle, AwPathCoord_t fSweepAngle);
		void AddLine(AwPathCoord_t x1, AwPathCoord_t y1, AwPathCoord_t x2, AwPathCoord_t y2);
		void AddTriangle(AwPathCoord_t x1, AwPathCoord_t y1, AwPathCoord_t x2, AwPathCoord_t y2, AwPathCoord_t x3, AwPathCoord_t y3);
		void AddPolyline(AwPathPoint_p parrPoints, int nPoints);
		void AddPolygon(AwPathPoint_p parrPoints, int nPoints);

		void AddPath(const Path& rpath);

		void MoveTo(AwPathCoord_t x, AwPathCoord_t y);
		void LineTo(AwPathCoord_t x, AwPathCoord_t y);
		void CloseShape(void);

		void Clear(void);

		inline const agg::path_storage& GetStorage(void) const { return m_storage; }

		template<typename AGGPathType>
		void AddAGGPath(AGGPathType& rpath) {
			m_storage.concat_path(rpath);
		}

		inline Path operator+(const Path& rpath) { 
			Path p(*this);
			p.AddPath(rpath);
			return p;
		}

		inline Path& operator+=(const Path& rpath) {
			AddPath(rpath);
			return *this;
		}

	private:
		agg::path_storage m_storage;
	};
}

#endif
