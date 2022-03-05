#pragma once

#ifndef AGGWRAP_HEADER_PEN
#define AGGWRAP_HEADER_PEN

#include "Brush.hpp"
#include "Path.hpp"

namespace AGGWrap {
	class Pen {
	public:
		Pen(SharedObject<Brush>& rsoBrush, const AwPenParams_t& rpar);
		Pen(const Pen& rpen);

		~Pen(void);

		inline const Brush& GetBrush(void) const { return *m_rsoBrush; }
		inline const AwPenParams_t& GetParams(void) const { return m_par; }

		void StrokePath(const Path& rpathSource, Path& rpathDest) const;

		static inline Pen& FromHandle(AwPen_h hPen) {
			return *(Pen*)hPen;
		}

	private:
		AGGWRAP_NOASSIGN(Pen);
		SharedObject<Brush>& m_rsoBrush;
		AwPenParams_t m_par;
	};
}

#endif
