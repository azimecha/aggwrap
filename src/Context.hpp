#pragma once

#ifndef AGGWRAP_HEADER_CONTEXT
#define AGGWRAP_HEADER_CONTEXT

#include "Brush.hpp"

namespace AGGWrap {
	class DrawingContext {
	public:
		DrawingContext(Bitmap& rbmTarget);

	private:
		AGGWRAP_NOCOPY(DrawingContext, DrawingContext);
		AGGWRAP_NOASSIGN(DrawingContext);
		Bitmap& m_rbmTarget;
	};
}

#endif
