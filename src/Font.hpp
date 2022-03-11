#pragma once

#ifndef AGGWRAP_HEADER_FONT
#define AGGWRAP_HEADER_FONT

#include "Types.hpp"
#include "Brush.hpp"

namespace AGGWrap {
	typedef int Codepoint;

	class Font {
	public:
		virtual ~Font(void);
		virtual AwPathCoord_t CalculateSize(const Array<Codepoint>& rarrCodepoints) const = 0;
		virtual void DrawText(Brush::Renderer& rrend, const Brush& rbr, AwPathCoord_t x, AwPathCoord_t y, const Array<Codepoint>& rarrCodepoints,
			bool bFast) const = 0;

		inline const AwFontInfo_t& GetInfo(void) const { return m_infFont; }

	protected:
		Font(const AwFontInfo_t& rinf);

	private:
		AwFontInfo_t m_infFont;
	};

	class BitmapFont : public Font {
	protected:
		BitmapFont(BufferOf<AwBitmapFontGlyph_t>& rbufGlyphs, const AwFontInfo_t& rinf, Codepoint nGlyphZeroChar, Codepoint nGlyphCount, 
			Codepoint nGlyphDefault);

	public:
		virtual ~BitmapFont(void);

		AwPathCoord_t CalculateSize(const Array<Codepoint>& rarrCodepoints) const override;

	private:
		AGGWRAP_NOCOPY(BitmapFont, BitmapFont);
		AGGWRAP_NOASSIGN(BitmapFont);

		BufferOf<AwBitmapFontGlyph_t> m_bufGlyphs;
		Codepoint m_nGlyphZeroChar, m_nGlyphCount, m_nGlyphDefault;

	protected:
		inline int GetGlyphIndex(Codepoint nCodepoint) const {
			if (nCodepoint < m_nGlyphZeroChar)
				return m_nGlyphDefault;

			nCodepoint -= m_nGlyphZeroChar;
			if (nCodepoint >= m_nGlyphCount)
				return m_nGlyphDefault;

			return nCodepoint;
		}

		inline const AwBitmapFontGlyph_t& GetGlyph(Codepoint nCodepoint) const {
			return m_bufGlyphs[GetGlyphIndex(nCodepoint)];
		}
	};

	class AliasedBitmapFont : public BitmapFont {
	public:
		AliasedBitmapFont(BufferOf<AwBitmapFontGlyph_t>& rbufGlyphs, const AwFontInfo_t& rinf, Codepoint nGlyphZeroChar, Codepoint nGlyphCount,
			Codepoint nGlyphDefault);

		virtual ~AliasedBitmapFont(void);

		void DrawText(Brush::Renderer& rrend, const Brush& rbr, AwPathCoord_t x, AwPathCoord_t y, const Array<Codepoint>& rarrCodepoints,
			bool bFast) const override;

	private:
		AGGWRAP_NOCOPY(AliasedBitmapFont, AliasedBitmapFont);
		AGGWRAP_NOASSIGN(AliasedBitmapFont);

		Array<Array<AwByte_t>> m_arrGlyphScanlines;
	};

	Array<Codepoint> ParseUTF8(const char* pcszUTF8String);
	static inline bool GetBitValue(AwByte_t* pBits, unsigned nBit) { return (pBits[nBit >> 3] & (1 << (nBit & 7))) != 0; }
}

#endif
