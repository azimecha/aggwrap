#include "Brush.hpp"
#include <agg_renderer_base.h>
#include <agg_path_storage.h>
#include <agg_span_interpolator_linear.h>
#include <agg_span_gradient.h>
#include <agg_scanline_p.h>
#include <agg_span_allocator.h>
#include <agg_renderer_scanline.h>
#include <agg_image_accessors.h>
#include <agg_span_pattern_rgba.h>

using namespace AGGWrap;

AGGWrap::Brush::~Brush(void) {}

AGGWrap::SolidBrush::SolidBrush(void) {
	m_clr = agg::rgba8(0, 0, 0, 0);
}

AGGWrap::SolidBrush::SolidBrush(Color clr) {
	m_clr = clr;
	m_clrPremul = Premultiply(clr);
}

AGGWrap::SolidBrush::SolidBrush(const SolidBrush& rbr) {
	m_clr = rbr.m_clr;
	m_clrPremul = rbr.m_clrPremul;
}

AGGWrap::SolidBrush::~SolidBrush(void) {}

void AGGWrap::SolidBrush::PerformFill(Renderer& rrend, Rasterizer& rrast, bool bFast) const {
	agg::renderer_scanline_aa_solid<Renderer> rendSolid;
	agg::scanline_p8 sl;

	rendSolid.attach(rrend);
	rendSolid.color(m_clrPremul);

	agg::render_scanlines(rrast, sl, rendSolid);
}

AGGWrap::GradientBrush::~GradientBrush(void) {}

AGGWrap::GradientBrush::GradientBrush(const Array<GradientStop>& rarrStops) {
	m_arrStops = rarrStops;
	m_arrStops.Sort();
}

AGGWrap::GradientBrush::GradientBrush(const GradientBrush& rbr) {
	m_arrStops = rbr.m_arrStops;
}

AGGWrap::LinearGradientBrush::LinearGradientBrush(const Array<GradientStop>& rarrStops, AwAngle_t fAngle) : GradientBrush(rarrStops) {
	m_fAngle = fAngle;
}

AGGWrap::LinearGradientBrush::LinearGradientBrush(const LinearGradientBrush& rbr) : GradientBrush(rbr) {
	m_fAngle = rbr.m_fAngle;
}

AGGWrap::LinearGradientBrush::~LinearGradientBrush(void) {}

int AGGWrap::LinearGradientBrush::GetStopBefore(GradientStop::Position fPosition) const {
	int i;

	for (i = 0; i < GetStopCount(); i++)
		if (GetStopPosition(i) > fPosition)
			return i - 1;

	return GetStopCount() - 1;
}

int AGGWrap::LinearGradientBrush::GetStopAfter(GradientStop::Position fPosition) const {
	int i;

	for (i = 0; i < GetStopCount(); i++)
		if (GetStopPosition(i) >= fPosition)
			return i;

	return GetStopCount() - 1;
}

Color AGGWrap::LinearGradientBrush::GetColorAt(GradientStop::Position fPosition) const {
	int nPrevStop, nNextStop;

	nPrevStop = GetStopBefore(fPosition);
	nNextStop = GetStopAfter(fPosition);

	if (nPrevStop == nNextStop)
		return GetStopColor(nPrevStop);
	else {
		float fRelPos = (fPosition - GetStopPosition(nPrevStop)) / (GetStopPosition(nNextStop) - GetStopPosition(nPrevStop));
		return GetStopColor(nPrevStop).gradient(GetStopColor(nNextStop), fRelPos);
	}
}

template<typename GradientFunction, typename ColorProvider>
static void s_GenerateLinearGradient(GradientFunction& rgrad, ColorProvider& rcp, double d2, agg::trans_affine& rxlat,
	agg::renderer_base<Bitmap::PixelFormat>& rrend, agg::rasterizer_scanline_aa<>& rrast)
{
	agg::scanline_p8 scanline;
	agg::span_interpolator_linear<agg::trans_affine> interpolator(rxlat);
	agg::span_gradient<agg::rgba8, agg::span_interpolator_linear<agg::trans_affine>, GradientFunction, ColorProvider>
		gradient(interpolator, rgrad, rcp, 0.0, d2);
	agg::span_allocator<agg::rgba8> allocator;

	agg::render_scanlines_aa(rrast, scanline, rrend, allocator, gradient);
}

void AGGWrap::LinearGradientBrush::PerformFill(Renderer& rrend, Rasterizer& rrast, bool bFast) const {
	agg::pod_auto_array<agg::rgba8, 256> arrColors;
	for (unsigned nColor = 0; nColor < arrColors.size(); nColor++)
		arrColors[nColor] = GetColorAt((GradientStop::Position)nColor / (GradientStop::Position)arrColors.size());

	agg::trans_affine xlat;
	xlat.reset();

	int nWidth = rrast.max_x() - rrast.min_x();
	int nHeight = rrast.max_y() - rrast.min_y();

	if (fabs(m_fAngle) < 0.01) {
		xlat *= agg::trans_affine_translation(rrast.min_x(), rrast.min_y());
		xlat.invert();

		agg::gradient_y gradY;
		s_GenerateLinearGradient(gradY, arrColors, nHeight, xlat, rrend, rrast);
	} else {
		double d2 = 100.0; // no idea
		xlat *= agg::trans_affine_scaling(sqrt((double)nWidth * nWidth + (double)nHeight * nHeight) / d2);
		xlat *= agg::trans_affine_rotation(m_fAngle);
		xlat *= agg::trans_affine_translation(rrast.min_x(), rrast.min_y());
		xlat.invert();

		agg::gradient_circle gradCircle;
		s_GenerateLinearGradient(gradCircle, arrColors, d2, xlat, rrend, rrast);
	}
}

AGGWrap::BitmapBrush::~BitmapBrush(void) {}

AGGWrap::BitmapBrush::BitmapBrush(const Bitmap& rbm) : m_bmPattern(rbm) {}

AGGWrap::BitmapBrush::BitmapBrush(const BitmapBrush& rbr) : m_bmPattern(rbr.m_bmPattern) {}

AGGWrap::PatternBrush::PatternBrush(const Bitmap& rbm) : BitmapBrush(rbm) {}

AGGWrap::PatternBrush::PatternBrush(const PatternBrush& rbr) : BitmapBrush(rbr) {}

AGGWrap::PatternBrush::~PatternBrush(void) {}

typedef agg::image_accessor_wrap<Bitmap::PixelFormat, agg::wrap_mode_repeat, agg::wrap_mode_repeat> RepeatAccessor;

void AGGWrap::PatternBrush::PerformFill(Renderer& rrend, Rasterizer& rrast, bool bFast) const {
	RepeatAccessor accessor((Bitmap::PixelFormat&)GetBitmap().GetFormatAdaptor());
	agg::span_pattern_rgba<RepeatAccessor> generator(accessor, 0, 0);
	agg::span_allocator<agg::rgba8> allocator;
	agg::scanline_p8 sl;
	agg::render_scanlines_aa(rrast, sl, rrend, allocator, generator);
}

AGGWrap::ScaleBrush::ScaleBrush(const Bitmap& rbm, AwScaleMode_t mode) : BitmapBrush(rbm) {
	m_mode = mode;
}

AGGWrap::ScaleBrush::ScaleBrush(const ScaleBrush& rbr) : BitmapBrush(rbr) {
	m_mode = rbr.m_mode;
}

AGGWrap::ScaleBrush::~ScaleBrush(void) {}

void AGGWrap::ScaleBrush::PerformFill(Renderer& rrend, Rasterizer& rrast, bool bFast) const {
	int x = rrast.min_x();
	int y = rrast.min_y();
	int w = rrast.max_x() - rrast.min_x();
	int h = rrast.max_y() - rrast.min_y();

	CalculateScaledRect(GetBitmap().GetWidth(), GetBitmap().GetHeight(), x, y, w, h, m_mode);

	if (bFast)
		ScaledBitmapFill<agg::span_image_filter_rgba_nn<ClipAccessor, LinearSpanInterpolator>>(GetBitmap(), rrend, rrast, x, y, w, h);
	else
		ScaledBitmapFill<agg::span_image_filter_rgba_bilinear<ClipAccessor, LinearSpanInterpolator>>(GetBitmap(), rrend, rrast, x, y, w, h);
}

AGGWRAP_EXPIMPL AwBrush_h AGGWRAP_FUNC AwCreateSolidBrush(AwByte_t r, AwByte_t g, AwByte_t b, AwByte_t a) {
	SolidBrush* pBrush;

	try {
		pBrush = new SolidBrush(Color(r, g, b, a));
		return (AwBrush_h)SharedObject<Brush>::Create(pBrush, 1);
	} catch (...) {
		if (pBrush != nullptr)
			delete pBrush;
		return nullptr;
	}
}

AGGWRAP_EXPIMPL AwBrush_h AGGWRAP_FUNC AwCreateLinearGradientBrush(AwGradientStop_p parrStops, int nStops, AwAngle_t fAngle) {
	LinearGradientBrush* pBrush;

	try {
		Array<GradientStop> arrStopsConv(nStops);

		for (int i = 0; i < nStops; i++) {
			arrStopsConv[i].StopColor = Color(parrStops[i].clr.r, parrStops[i].clr.g, parrStops[i].clr.b, parrStops[i].clr.a);
			arrStopsConv[i].StopPosition = parrStops[i].pos;
		}

		pBrush = new LinearGradientBrush(arrStopsConv, fAngle);
		return (AwBrush_h)SharedObject<Brush>::Create(pBrush, 1);
	} catch (...) {
		if (pBrush != nullptr)
			delete pBrush;
		return nullptr;
	}
}

AGGWRAP_EXPIMPL AwBrush_h AGGWRAP_FUNC AwCreatePatternBrush(AwBitmap_h hBitmap) {
	PatternBrush* pBrush;

	try {
		pBrush = new PatternBrush(*(Bitmap*)hBitmap);
		return (AwBrush_h)SharedObject<Brush>::Create(pBrush, 1);
	} catch (...) {
		if (pBrush != nullptr)
			delete pBrush;
		return nullptr;
	}
}

AGGWRAP_EXPIMPL AwBrush_h AGGWRAP_FUNC AwCreateScaledBrush(AwBitmap_h hBitmap, AwScaleMode_t mode) {
	ScaleBrush* pBrush;

	try {
		pBrush = new ScaleBrush(*(Bitmap*)hBitmap, mode);
		return (AwBrush_h)SharedObject<Brush>::Create(pBrush, 1);
	} catch (...) {
		if (pBrush != nullptr)
			delete pBrush;
		return nullptr;
	}
}

AGGWRAP_EXPIMPL void AGGWRAP_FUNC AwDeleteBrush(AwBrush_h hBrush) {
	try {
		((SharedObject<Brush>*)hBrush)->RemoveRef();
	} catch (...) { }
}

void AGGWrap::CalculateScaledRect(int nSourceW, int nSourceH, int& rnDestX, int& rnDestY, int& rnDestW, int& rnDestH, AwScaleMode_t mode) {
	float fRatioX, fRatioY, fRatioChosen;
	int nScaledW, nScaledH, nCenterX, nCenterY;

	if ((rnDestW == 0) || (rnDestH == 0))
		return;

	fRatioX = nSourceW / rnDestW;
	fRatioY = nSourceH / rnDestH;

	switch (mode) {
	case AwScaleMode_OriginalSize:
		rnDestW = nSourceW;
		rnDestH = nSourceH;
		return;

	case AwScaleMode_Fill:
		fRatioChosen = (fRatioX < fRatioY) ? fRatioX : fRatioY;
		break;

	case AwScaleMode_Fit:
		fRatioChosen = (fRatioX > fRatioY) ? fRatioX : fRatioY;
		break;

	case AwScaleMode_Stretch:
		return;

	default:
		throw new InvalidEnumValueException();
	}

	nScaledW = nSourceW / fRatioChosen;
	nScaledH = nSourceH / fRatioChosen;

	nCenterX = rnDestX + rnDestW / 2;
	nCenterY = rnDestY + rnDestH / 2;

	rnDestX = nCenterX - nScaledW / 2;
	rnDestY = nCenterY - nScaledH / 2;
	
	rnDestW = nScaledW;
	rnDestH = nScaledH;	
}
