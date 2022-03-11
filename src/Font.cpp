#include "Font.hpp"
#include <agg_scanline_storage_bin.h>
#include <utf8.h>

using namespace AGGWrap;

namespace {
	template<typename InnerRasterizer>
	class ManualMinMaxRasterizer : public Rasterizer<InnerRasterizer> {
	private:
		int m_nMinX, m_nMinY, m_nMaxX, m_nMaxY;

	public:
		ManualMinMaxRasterizer(InnerRasterizer& rrast, int x, int y, int w, int h) : Rasterizer(rrast) {
			m_nMinX = x;
			m_nMaxX = x + w;
			m_nMinY = y;
			m_nMaxY = y + h;
		}

		int min_x(void) const override { return m_nMinX; }
		int max_x(void) const override { return m_nMaxX; }
		int min_y(void) const override { return m_nMinY; }
		int max_y(void) const override { return m_nMaxY; }
	};
}

AGGWrap::Font::~Font(void) {}

AGGWrap::Font::Font(const AwFontInfo_t& rinf) : m_infFont(rinf) {}

AGGWrap::BitmapFont::BitmapFont(BufferOf<AwBitmapFontGlyph_t>& rbufGlyphs, const AwFontInfo_t& rinf,
	Codepoint nGlyphZeroChar, Codepoint nGlyphCount, Codepoint nGlyphDefault) 
	: Font(rinf), m_nGlyphZeroChar(nGlyphZeroChar), m_nGlyphCount(nGlyphCount), m_nGlyphDefault(nGlyphDefault)
{
	m_bufGlyphs.Steal(rbufGlyphs);
}

AGGWrap::BitmapFont::~BitmapFont(void) {}

AwPathCoord_t AGGWrap::BitmapFont::CalculateSize(const Array<Codepoint>& rarrCodepoints) const {
	AwPathCoord_t fLength = 0.0f;

	for (int i = 0; i < rarrCodepoints.GetItemCount(); i++)
		fLength += GetGlyph(rarrCodepoints[i]).nWidthPixels;

	return fLength;
}

AGGWrap::AliasedBitmapFont::AliasedBitmapFont(BufferOf<AwBitmapFontGlyph_t>& rbufGlyphs, const AwFontInfo_t& rinf,
	Codepoint nGlyphZeroChar, Codepoint nGlyphCount, Codepoint nGlyphDefault)
	: BitmapFont(rbufGlyphs, rinf, nGlyphZeroChar, nGlyphCount, nGlyphDefault), m_arrGlyphScanlines(nGlyphCount)
{
	IRasterizer::Scanline sl;

	for (int i = 0; i < nGlyphCount; i++) {
		agg::scanline_storage_bin storage;
		const AwBitmapFontGlyph_t& rinfGlyph = GetGlyph(i + nGlyphZeroChar);

		for (AwBitmapFontSize_t y = 0; y < rinf.nHeight; y++) {
			sl.reset(0, rinfGlyph.nWidthPixels);

			for (AwBitmapFontSize_t x = 0; x < rinfGlyph.nWidthPixels; x++) {
				if (GetBitValue(rinfGlyph.pData, (unsigned)(y * rinfGlyph.nWidthPixels + x)))
					sl.add_cell(x, 255);
			}

			if (sl.num_spans() > 0) {
				sl.finalize(y);
				storage.render(sl);
			}

			sl.reset_spans();
		}

		m_arrGlyphScanlines[i] = Array<AwByte_t>(storage.byte_size());
		storage.serialize(m_arrGlyphScanlines[i].GetPointer());
	}
}

AGGWrap::AliasedBitmapFont::~AliasedBitmapFont(void) {}

template<typename Source, typename Dest, typename Scanline>
static void s_CopyScanlines(Source& rgenSource, Dest& rstorDest, Scanline& rsl) {
	if (rgenSource.rewind_scanlines()) {
		while (rgenSource.sweep_scanline(rsl))
			rstorDest.render(rsl);
	}
}

void AGGWrap::AliasedBitmapFont::DrawText(Brush::Renderer& rrend, const Brush& rbr, AwPathCoord_t x, AwPathCoord_t y,
	const Array<Codepoint>& rarrCodepoints, bool bFast) const
{
	AwPathCoord_t w = CalculateSize(rarrCodepoints);
	AwPathCoord_t h = GetInfo().nHeight;

	agg::scanline_storage_bin storage;
	AwPathCoord_t fCurXPos = x;

	for (int i = 0; i < rarrCodepoints.GetItemCount(); i++) {
		Codepoint cp = rarrCodepoints[i];
		const AwBitmapFontGlyph_t& rglyph = GetGlyph(cp);
		const Array<AwByte_t>& rarrScanlineData = m_arrGlyphScanlines[GetGlyphIndex(cp)];

		agg::serialized_scanlines_adaptor_bin adaptor(rarrScanlineData.GetPointer(), rarrScanlineData.GetItemCount(), fCurXPos, y);
		agg::serialized_scanlines_adaptor_bin::embedded_scanline sl;
		sl.reset(0, rglyph.nWidthPixels);
		s_CopyScanlines(adaptor, storage, sl);

		fCurXPos += rglyph.nWidthPixels;
	}

	ManualMinMaxRasterizer<agg::scanline_storage_bin> rast(storage, x, y, w, h);
	rbr.PerformFill(rrend, rast, bFast);
}

Array<Codepoint> AGGWrap::ParseUTF8(const char* pcszUTF8String) {
	Array<Codepoint> arrParsed(utf8len(pcszUTF8String));
	utf8_int32_t nCodepoint;

	for (int i = 0; (i < arrParsed.GetItemCount()) && *pcszUTF8String; i++) {
		pcszUTF8String = utf8codepoint(pcszUTF8String, &nCodepoint);
		arrParsed[i] = nCodepoint;
	}

	return arrParsed;
}

AGGWRAP_EXPIMPL AwFont_h AGGWRAP_FUNC AwCreateMonochromeBitmapFont(AwFontInfo_pc pcinfFont, AwBufferInfo_p pinfGlyphBuffer, int nGlyphZeroChar, int nGlyphCount,
	int nGlyphDefault) 
{
	try {
		BufferOf<AwBitmapFontGlyph_t> bufGlyphs(*pinfGlyphBuffer);
		return (AwFont_h)new AGGWrap::AliasedBitmapFont(bufGlyphs, *pcinfFont, (Codepoint)nGlyphZeroChar, (Codepoint)nGlyphCount, (Codepoint)nGlyphDefault);
	} catch (...) {
		return (AwFont_h)nullptr;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwGetTextSize(AwFont_h hFont, const char* pcszText, AGGWRAP_OPTIONAL AGGWRAP_OUT AwPathCoord_p pfWidth, 
	AGGWRAP_OPTIONAL AGGWRAP_OUT AwPathCoord_p pfHeight) 
{
	try {
		Font& rfont = *(Font*)hFont;

		if (pfWidth) {
			Array<Codepoint> arrCodepoints = ParseUTF8(pcszText);
			*pfWidth = rfont.CalculateSize(arrCodepoints);
		}

		if (pfHeight)
			*pfHeight = rfont.GetInfo().nHeight;

		return AGGWRAP_TRUE;
	} catch (...) {
		return AGGWRAP_FALSE;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwGetFontInfo(AwFont_h hFont, AGGWRAP_OUT AwFontInfo_p pinfFont) {
	try {
		*pinfFont = ((Font*)hFont)->GetInfo();
		return AGGWRAP_TRUE;
	} catch (...) {
		return AGGWRAP_FALSE;
	}
}

AGGWRAP_EXPIMPL void AGGWRAP_FUNC AwDeleteFont(AwFont_h hFont) {
	try {
		delete (Font*)hFont;
	} catch (...) {}
}
