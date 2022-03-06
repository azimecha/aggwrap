#pragma once

#ifndef AGGWRAP_HEADER_BRUSH
#define AGGWRAP_HEADER_BRUSH

#include "Types.hpp"
#include "Bitmap.hpp"
#include <agg_rasterizer_scanline_aa.h>
#include <agg_renderer_base.h>
#include <agg_span_image_filter_rgba.h>
#include <agg_image_accessors.h>
#include <agg_scanline_p.h>
#include <agg_span_allocator.h>
#include <agg_renderer_scanline.h>

namespace AGGWrap {
	class Brush {
	public:
		typedef agg::renderer_base<Bitmap::PixelFormat> Renderer;
		typedef agg::rasterizer_scanline_aa<> Rasterizer;

		virtual ~Brush(void);
		virtual void PerformFill(Renderer& rrend, Rasterizer& rrast, bool bFast) const = 0;

		static inline Brush& FromHandle(AwBrush_h hBrush) {
			return **(SharedObject<Brush>*)hBrush;
		}
	};

	class SolidBrush : public Brush {
	public:
		SolidBrush(void);
		SolidBrush(Color clr);
		SolidBrush(const SolidBrush& rbr);

		virtual ~SolidBrush(void);

		inline Color GetColor(void) const { return m_clr; }

		void PerformFill(Renderer& rrend, Rasterizer& rrast, bool bFast) const override;

	private:
		AGGWRAP_NOASSIGN(SolidBrush);
		Color m_clr, m_clrPremul;
	};

	struct GradientStop {
		typedef float Position;

		Position StopPosition;
		Color StopColor;

		inline bool operator>(const GradientStop& rstopOther) const { return StopPosition < rstopOther.StopPosition; }
		inline bool operator<(const GradientStop& rstopOther) const { return StopPosition > rstopOther.StopPosition; }
		inline bool operator>=(const GradientStop& rstopOther) const { return StopPosition <= rstopOther.StopPosition; }
		inline bool operator<=(const GradientStop& rstopOther) const { return StopPosition >= rstopOther.StopPosition; }
	};

	class GradientBrush : public Brush {
	public:
		virtual ~GradientBrush(void);

		inline int GetStopCount(void) const { return m_arrStops.GetItemCount(); }
		inline GradientStop::Position GetStopPosition(int nStop) const { return m_arrStops[nStop].StopPosition; }
		inline Color GetStopColor(int nStop) const { return m_arrStops[nStop].StopColor; }

	protected:
		GradientBrush(const Array<GradientStop>& rarrStops);
		GradientBrush(const GradientBrush& rbr);

	private:
		AGGWRAP_NOASSIGN(GradientBrush);
		Array<GradientStop> m_arrStops;
	};

	class LinearGradientBrush : public GradientBrush {
	public:
		LinearGradientBrush(const Array<GradientStop>& rarrStops, AwAngle_t fAngle);
		LinearGradientBrush(const LinearGradientBrush& rbr);

		virtual ~LinearGradientBrush(void);

		int GetStopBefore(GradientStop::Position fPosition) const;
		int GetStopAfter(GradientStop::Position fPosition) const;

		Color GetColorAt(GradientStop::Position fPosition) const;

		void PerformFill(Renderer& rrend, Rasterizer& rrast, bool bFast) const override;

	private:
		AGGWRAP_NOASSIGN(LinearGradientBrush);
		AwAngle_t m_fAngle;
	};

	class BitmapBrush : public Brush {
	public:
		virtual ~BitmapBrush(void);

		inline const Bitmap& GetBitmap(void) const { return m_bmPattern; }

	protected:
		BitmapBrush(const Bitmap& rbm);
		BitmapBrush(const BitmapBrush& rbr);

	private:
		AGGWRAP_NOASSIGN(BitmapBrush);
		Bitmap m_bmPattern;
	};

	class PatternBrush : public BitmapBrush {
	public:
		PatternBrush(const Bitmap& rbm);
		PatternBrush(const PatternBrush& rbr);

		virtual ~PatternBrush(void);

		void PerformFill(Renderer& rrend, Rasterizer& rrast, bool bFast) const override;

	private:
		AGGWRAP_NOASSIGN(PatternBrush);
	};

	class ScaleBrush : public BitmapBrush {
	public:
		ScaleBrush(const Bitmap& rbm, AwScaleMode_t mode);
		ScaleBrush(const ScaleBrush& rbr);

		virtual ~ScaleBrush(void);

		void PerformFill(Renderer& rrend, Rasterizer& rrast, bool bFast) const override;

	private:
		AGGWRAP_NOASSIGN(ScaleBrush);
		AwScaleMode_t m_mode;
	};

	typedef agg::image_accessor_clip<Bitmap::PixelFormat> ClipAccessor;
	typedef agg::span_interpolator_linear<agg::trans_affine> LinearSpanInterpolator;

	template<typename ScalingFilter>
	void ScaledBitmapFill(const Bitmap& rbm, Brush::Renderer& rrend, Brush::Rasterizer& rrast, int nDestX, int nDestY, int nDestW, int nDestH,
		int nSrcX = 0, int nSrcY = 0, int nSrcW = -1, int nSrcH = -1)
	{
		agg::scanline_p8 sl;

		if (nSrcW < 0) nSrcW = rbm.GetWidth();
		if (nSrcH < 0) nSrcH = rbm.GetHeight();

		agg::trans_affine xlat;
		xlat.translate(-nSrcX, -nSrcY);
		xlat.scale((float)nDestW / nSrcW, (float)nDestH / nSrcH);
		xlat.translate(nDestX, nDestY);
		xlat.invert();

		LinearSpanInterpolator interpolator(xlat);
		ClipAccessor accessor((Bitmap::PixelFormat&)rbm.GetFormatAdaptor(), Bitmap::PixelFormat::color_type(0, 0, 0, 0));
		ScalingFilter generator(accessor, interpolator);
		agg::span_allocator<Bitmap::PixelFormat::color_type> allocator;

		agg::render_scanlines_aa(rrast, sl, rrend, allocator, generator);
	}

	void CalculateScaledRect(int nSourceW, int nSourceH, int& rnDestX, int& rnDestY, int& rnDestW, int& rnDestH, AwScaleMode_t mode);
}

#endif
