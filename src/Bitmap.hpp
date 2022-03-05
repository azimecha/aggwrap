#pragma once

#ifndef AGGWRAP_HEADER_BITMAP
#define AGGWRAP_HEADER_BITMAP

#include "AGGWrap.h"
#include "Types.hpp"

#include <agg_basics.h>
#include <agg_pixfmt_rgba.h>

namespace AGGWrap {
	class Bitmap {
	public:
		typedef agg::int8u Sample;
		typedef agg::pixfmt_bgra32_pre PixelFormat;

		static constexpr int SAMPLES_PER_PIXEL = 4;

		Bitmap(int w, int h);
		Bitmap(int w, int h, UniquePointer<Sample>& rupDataToSteal);
		Bitmap(const Bitmap& rbm);

		virtual ~Bitmap(void);

		inline int GetWidth(void) const { return m_nWidth; }
		inline int GetHeight(void) const { return m_nHeight; }
		inline int GetStride(void) const { return m_nWidth * sizeof(Sample) * SAMPLES_PER_PIXEL; }
		inline int GetDataSize(void) const { return GetStride() * m_nHeight; }

		inline PixelFormat& GetFormatAdaptor(void) { return m_fmt; }
		inline const PixelFormat& GetFormatAdaptor(void) const { return m_fmt; }

		inline Sample* GetDataPointer(void) { return m_upData.GetPointer(); }
		inline const Sample* GetDataPointer(void) const { return m_upData.GetPointer(); }

	private:
		AGGWRAP_NOASSIGN(Bitmap);

		int m_nWidth, m_nHeight;
		UniquePointer<Sample> m_upData;
		agg::rendering_buffer m_buf;
		PixelFormat m_fmt;

		void Attach(void);
	};
}

#endif
