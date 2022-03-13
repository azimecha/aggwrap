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

AwPathPoint_t AGGWrap::BitmapFont::CalculateSize(const Array<Codepoint>& rarrCodepoints) const {
	int nLineHeight = GetInfo().nHeight;

	AwPathPoint_t ptExtent;
	ptExtent.x = 0.0f;
	ptExtent.y = nLineHeight;

	AwPathCoord_t fCurLineWidth = 0.0f;

	for (int i = 0; i < rarrCodepoints.GetItemCount(); i++) {
		Codepoint nCodepoint = rarrCodepoints[i];
		switch (nCodepoint) {
		case '\n':
			ptExtent.y += nLineHeight;
			break;

		case '\t':
			fCurLineWidth += GetTabSize();
			break;

		case '\r':
			if (fCurLineWidth > ptExtent.x)
				ptExtent.x = fCurLineWidth;
			fCurLineWidth = 0.0f;
			break;

		default:
			if (nCodepoint >= ' ')
				fCurLineWidth += GetGlyph(rarrCodepoints[i]).nWidthPixels;
		}
	}

	if (fCurLineWidth > ptExtent.x)
		ptExtent.x = fCurLineWidth;

	return ptExtent;
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
	AwPathPoint_t ptExtent = CalculateSize(rarrCodepoints);

	agg::scanline_storage_bin storage;
	AwPathCoord_t fCurXPos = x;
	AwPathCoord_t fCurYPos = y;

	for (int i = 0; i < rarrCodepoints.GetItemCount(); i++) {
		Codepoint nCodepoint = rarrCodepoints[i];

		switch (nCodepoint) {
		case '\r':
			fCurXPos = x;
			continue;

		case '\n':
			fCurYPos += GetInfo().nHeight;
			continue;

		case '\t':
			fCurXPos += GetTabSize();
			continue;
		}

		if (nCodepoint < ' ') continue;

		const AwBitmapFontGlyph_t& rglyph = GetGlyph(nCodepoint);
		const Array<AwByte_t>& rarrScanlineData = m_arrGlyphScanlines[GetGlyphIndex(nCodepoint)];

		agg::serialized_scanlines_adaptor_bin adaptor(rarrScanlineData.GetPointer(), rarrScanlineData.GetItemCount(), fCurXPos, fCurYPos);
		agg::serialized_scanlines_adaptor_bin::embedded_scanline sl;
		sl.reset(0, rglyph.nWidthPixels);
		s_CopyScanlines(adaptor, storage, sl);

		fCurXPos += rglyph.nWidthPixels;
	}

	ManualMinMaxRasterizer<agg::scanline_storage_bin> rast(storage, x, y, ptExtent.x, ptExtent.y);
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

Array<char> AGGWrap::ConvertUCS2ToUTF8(const wchar_t* pcwzUCS2String) {
	size_t nChars = 0;
	const wchar_t* pcwzCur;

	for (pcwzCur = pcwzUCS2String; *pcwzCur; pcwzCur++)
		nChars += utf8codepointsize((utf8_int32_t)*pcwzCur);
	nChars++; // null

	Array<char> arrUTF8((int)nChars);

	char* pszCur = arrUTF8.GetPointer();
	for (pcwzCur = pcwzUCS2String; *pcwzCur; pcwzCur++)
		utf8catcodepoint(pszCur, *pcwzCur, SIZE_MAX);
	*pszCur = 0;

	return arrUTF8;
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
		Array<Codepoint> arrCodepoints = ParseUTF8(pcszText);
		AwPathPoint_t ptExtent = rfont.CalculateSize(arrCodepoints);

		if (pfWidth)
			*pfWidth = ptExtent.x;

		if (pfHeight)
			*pfHeight = ptExtent.y;

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
