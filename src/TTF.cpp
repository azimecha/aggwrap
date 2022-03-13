#include "TTF.hpp"
#include <agg_path_storage.h>

using namespace AGGWrap;

namespace {
	class IFontCharCallback {
	public:
		virtual void PerformAction(Codepoint nCodepoint, int nGlyph, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h) = 0;
	};

	class TrueTypeFont : public Font {
	public:
		TrueTypeFont(SharedObject<BufferOf<AwByte_t>>& rsoData, int nOffset, int nHeight, const stbtt_fontinfo& rinfFont);
		virtual ~TrueTypeFont(void);

		AwPathPoint_t CalculateSize(const Array<Codepoint>& rarrCodepoints) const override;
		void DrawText(Brush::Renderer& rrend, const Brush& rbr, AwPathCoord_t x, AwPathCoord_t y, const Array<Codepoint>& rarrCodepoints,
			bool bFast) const override;

		void Iterate(const Array<Codepoint>& rarrCodepoints, IFontCharCallback& rcb, AwPathCoord_t x, AwPathCoord_t y) const;
		const stbtt_fontinfo& GetSTBFontInfo(void) const;

	private:
		AGGWRAP_NOCOPY(TrueTypeFont, TrueTypeFont);
		AGGWRAP_NOASSIGN(TrueTypeFont);

		SharedObjectReference<BufferOf<AwByte_t>> m_refData;
		int m_nDataOffset;
		stbtt_fontinfo m_infFont;
		AwPathCoord_t m_fSpaceSize;
		float m_fScale;

		static AwFontInfo_t GetFontInfo(int nHeight, const stbtt_fontinfo& rinfFont);
	};

	class CalculateSizeCallback : public IFontCharCallback {
	public:
		CalculateSizeCallback(AwPathCoord_t fStartX, AwPathCoord_t fStartY);

		void PerformAction(Codepoint nCodepoint, int nGlyph, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h) override;
		AwPathPoint_t GetResult(void) const;

	private:
		AwPathCoord_t m_fStartX, m_fStartY;
		AwPathCoord_t m_fMaxX, m_fMaxY;
	};

	class GetPathCallback : public IFontCharCallback {
	public:
		GetPathCallback(const TrueTypeFont& rfont, agg::path_storage& rstorage, float fScale);

		void PerformAction(Codepoint nCodepoint, int nGlyph, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h) override;

	private:
		const TrueTypeFont& m_rfont;
		agg::path_storage& m_rstorage;
		float m_fScale;
	};

	class GlyphShape {
	public:
		GlyphShape(const stbtt_fontinfo& rinfFont, int nGlyph);
		~GlyphShape(void);

		int GetVertexCount(void) const;
		const stbtt_vertex& GetVertex(int nVertex) const;

	private:
		AGGWRAP_NOCOPY(GlyphShape, GlyphShape);
		AGGWRAP_NOASSIGN(GlyphShape);

		int m_nVertices;
		stbtt_vertex* m_pVertices;
	};

	TrueTypeFont::TrueTypeFont(SharedObject<BufferOf<AwByte_t>>& rsoData, int nOffset, int nHeight, const stbtt_fontinfo& rinfFont)
		: Font(GetFontInfo(nHeight, rinfFont)), m_refData(rsoData), m_infFont(rinfFont)
	{ 
		m_fScale = stbtt_ScaleForPixelHeight(&m_infFont, nHeight);

		int nTAdvance = 0, nTLeftSideBearing;
		stbtt_GetCodepointHMetrics(&m_infFont, 't', &nTAdvance, &nTLeftSideBearing);
		m_fSpaceSize = nTAdvance * m_fScale;
	}

	TrueTypeFont::~TrueTypeFont(void) { }

	AwPathPoint_t TrueTypeFont::CalculateSize(const Array<Codepoint>& rarrCodepoints) const {
		CalculateSizeCallback cb(0.0f, 0.0f);
		Iterate(rarrCodepoints, cb, 0.0f, 0.0f);
		return cb.GetResult();
	}

	void TrueTypeFont::DrawText(Brush::Renderer& rrend, const Brush& rbr, AwPathCoord_t x, AwPathCoord_t y, const Array<Codepoint>& rarrCodepoints, bool bFast) const {
		agg::path_storage storage;
		GetPathCallback cb(*this, storage, m_fScale);
		Iterate(rarrCodepoints, cb, x, y);

		agg::rasterizer_scanline_aa<> rastImpl;
		AGGWrap::Rasterizer<agg::rasterizer_scanline_aa<>> rastWrapper(rastImpl);

		rastImpl.add_path(storage);

		rbr.PerformFill(rrend, rastWrapper, bFast);
	}

	void TrueTypeFont::Iterate(const Array<Codepoint>& rarrCodepoints, IFontCharCallback& rcb, AwPathCoord_t fStartX, AwPathCoord_t fStartY) const {
		AwPathCoord_t x = fStartX;
		AwPathCoord_t y = fStartY;

		for (int i = 0; i < rarrCodepoints.GetItemCount(); i++) {
			int nGlyph = 0, nNextGlyph, nAdvance = 0, nLeftSideBearing;
			Codepoint nCodepoint = rarrCodepoints[i];
			AwPathCoord_t w;

			switch (nCodepoint) {
			case '\r':
				x = fStartX;
				w = 0;
				break;

			case '\n':
				y += GetInfo().nHeight;
				w = 0;
				break;

			case '\t':
				w = AGGWRAP_TAB_SIZE * m_fSpaceSize;
				break;

			case ' ':
				w = m_fSpaceSize;
				break;

			default:
				nGlyph = stbtt_FindGlyphIndex(&m_infFont, nCodepoint);
				if (nGlyph == 0) continue;

				stbtt_GetGlyphHMetrics(&m_infFont, nGlyph, &nAdvance, &nLeftSideBearing);

				if (i < (rarrCodepoints.GetItemCount() - 1)) {
					nNextGlyph = stbtt_FindGlyphIndex(&m_infFont, rarrCodepoints[i + 1]);
					if (nNextGlyph != 0)
						nAdvance += stbtt_GetGlyphKernAdvance(&m_infFont, nGlyph, nNextGlyph);
				}

				w = nAdvance * m_fScale;
				break;
			}

			rcb.PerformAction(nCodepoint, nGlyph, x, y, w, GetInfo().nHeight);
			x += w;
		}
	}

	const stbtt_fontinfo& TrueTypeFont::GetSTBFontInfo(void) const {
		return m_infFont;
	}

	AwFontInfo_t TrueTypeFont::GetFontInfo(int nHeight, const stbtt_fontinfo& rinfFont) {
		AwFontInfo_t inf;
		memset((void*)&inf, 0, sizeof(inf));
		inf.nHeight = nHeight;
		return inf;
	}
	
	CalculateSizeCallback::CalculateSizeCallback(AwPathCoord_t fStartX, AwPathCoord_t fStartY) {
		m_fStartX = fStartX;
		m_fStartY = fStartY;
		m_fMaxX = fStartX;
		m_fMaxY = fStartY;
	}

	void CalculateSizeCallback::PerformAction(Codepoint nCodepoint, int nGlyph, AwPathCoord_t x, AwPathCoord_t y, AwPathCoord_t w, AwPathCoord_t h) {
		x += w;
		y += h;

		if (x > m_fMaxX) m_fMaxX = x;
		if (y > m_fMaxY) m_fMaxY = y;
	}

	AwPathPoint_t CalculateSizeCallback::GetResult(void) const {
		AwPathPoint_t pt;
		pt.x = m_fMaxX - m_fStartX;
		pt.y = m_fMaxY - m_fStartY;
		return pt;
	}

	GetPathCallback::GetPathCallback(const TrueTypeFont& rfont, agg::path_storage& rstorage, float fScale) 
		: m_rfont(rfont), m_rstorage(rstorage) { m_fScale = fScale; }

	void GetPathCallback::PerformAction(Codepoint nCodepoint, int nGlyph, AwPathCoord_t fCharX, AwPathCoord_t fCharY, AwPathCoord_t fCharW, AwPathCoord_t fCharH) {
		switch (nCodepoint) {
		case '\r':
		case '\n':
		case '\t':
		case ' ':
			return;
		}

		GlyphShape shape(m_rfont.GetSTBFontInfo(), nGlyph);
		bool bPenDown = false;

		for (int i = 0; i < shape.GetVertexCount(); i++) {
			const stbtt_vertex& rvtx = shape.GetVertex(i);
			float x = rvtx.x * m_fScale + fCharX;
			float y = fCharH - rvtx.y * m_fScale + fCharY;
			float cx = rvtx.cx * m_fScale + fCharX;
			float cy = fCharH - rvtx.cy * m_fScale + fCharY;
			float cx1 = rvtx.cx1 * m_fScale + fCharX;
			float cy1 = fCharH - rvtx.cy1 * m_fScale + fCharY;

			switch (rvtx.type) {
			case STBTT_vmove:
				if (bPenDown) m_rstorage.close_polygon();
				m_rstorage.move_to(x, y);
				bPenDown = false;
				break;

			case STBTT_vline:
				m_rstorage.line_to(x, y);
				bPenDown = true;
				break;

			case STBTT_vcurve:
				m_rstorage.curve3(cx, cy, x, y);
				bPenDown = true;
				break;

			case STBTT_vcubic:
				m_rstorage.curve4(cx, cy, cx1, cy1, x, y);
				bPenDown = true;
				break;
			}
		}

		if (bPenDown)
			m_rstorage.close_polygon();
	}

	GlyphShape::GlyphShape(const stbtt_fontinfo& rinfFont, int nGlyph) {
		m_nVertices = 0;
		m_pVertices = (stbtt_vertex*)nullptr;

		m_nVertices = stbtt_GetGlyphShape(&rinfFont, nGlyph, &m_pVertices);
		if (m_nVertices < 0)
			m_nVertices = 0;
	}

	GlyphShape::~GlyphShape(void) {
		if (m_pVertices != nullptr) {
			delete m_pVertices;
			m_pVertices = (stbtt_vertex*)nullptr;
		}
	}

	int GlyphShape::GetVertexCount(void) const {
		return m_nVertices;
	}

	const stbtt_vertex& GlyphShape::GetVertex(int nVertex) const {
		if ((nVertex < 0) || (nVertex >= m_nVertices))
			throw OutOfRangeException();
		return m_pVertices[nVertex];
	}
}

AGGWrap::FontFile::~FontFile(void) {}

AGGWrap::TrueTypeFontFile::TrueTypeFontFile(SharedObject<BufferOf<AwByte_t>>& rsoData) : m_refBuffer(rsoData) {
	unsigned char* pData = (unsigned char*)m_refBuffer.GetSharedObject().GetObject().GetDataPointer();

	int nFonts = stbtt_GetNumberOfFonts(pData);
	if (nFonts < 0)
		throw FontInitializationException();

	m_arrFaces = Array<FontFace>(nFonts);

	for (int i = 0; i < nFonts; i++) {
		int nOffset = stbtt_GetFontOffsetForIndex(pData, i);
		if (nOffset < 0)
			throw FontInitializationException();

		m_arrFaces[i].Initialize(pData, nOffset);
	}
}

AGGWrap::TrueTypeFontFile::~TrueTypeFontFile(void) {}

int AGGWrap::TrueTypeFontFile::GetFontCount(void) const {
	return m_arrFaces.GetItemCount();
}

const char* AGGWrap::TrueTypeFontFile::GetFontName(int nFont) const {
	return m_arrFaces[nFont].GetName();
}

Font* AGGWrap::TrueTypeFontFile::CreateFont(int nFont, int nHeight) const {
	return new TrueTypeFont((SharedObject<BufferOf<AwByte_t>>&)m_refBuffer.GetSharedObject(), m_arrFaces[nFont].GetOffset(), nHeight,
		m_arrFaces[nFont].GetInfo());
}

AGGWrap::TrueTypeFontFile::FontFace::FontFace(void) : m_arrName(1) {
	m_arrName[0] = 0;
	memset((void*)&m_infFont, 0, sizeof(m_infFont));
}

void AGGWrap::TrueTypeFontFile::FontFace::Initialize(AwByte_t* pData, int nOffset) {
	m_nOffset = nOffset;

	int nResult = stbtt_InitFont(&m_infFont, pData, nOffset);
	if (nResult < 0) 
		throw FontInitializationException();

	int nLength = 0;
	const wchar_t* pcwzName = (const wchar_t*)stbtt_GetFontNameString(&m_infFont, &nLength, STBTT_PLATFORM_ID_MICROSOFT, STBTT_MS_EID_UNICODE_FULL,
		STBTT_MS_LANG_ENGLISH, 4);
	if (pcwzName != (const wchar_t*)nullptr)
		m_arrName = ConvertUCS2ToUTF8(pcwzName);
}

const char* AGGWrap::FontInitializationException::GetMessage(void) const {
	return "Error initializing font. The font file may be corrupt.";
}

AGGWrap::FontInitializationException::~FontInitializationException(void) {}

AGGWRAP_EXPIMPL AwFontSet_h AGGWRAP_FUNC AwCreateFontSetOnTTF(AwBufferInfo_p pinfDataBuffer) {
	BufferOf<AwByte_t>* pbuf = (BufferOf<AwByte_t>*)nullptr;
	SharedObject<BufferOf<AwByte_t>>* psoBuf = (SharedObject<BufferOf<AwByte_t>>*)nullptr;

	try {
		pbuf = new BufferOf<AwByte_t>(*pinfDataBuffer);
		psoBuf = SharedObject<BufferOf<AwByte_t>>::Create(pbuf, 0);
		return (AwFontSet_h)(Font*)new TrueTypeFontFile(*psoBuf);
	} catch (...) {
		// detach everything so the client's data buffer doesn't get destroyed if we failed
		if (psoBuf) {
			psoBuf->Detach();
			psoBuf->RemoveRef(); // will delete the shared object itself
		}

		if (pbuf) {
			pbuf->Detach();
			delete pbuf;
		}

		return (AwFontSet_h)nullptr;
	}
}

AGGWRAP_EXPIMPL int AGGWRAP_FUNC AwGetFontSetSize(AwFontSet_h hSet) {
	try {
		return ((FontFile*)hSet)->GetFontCount();
	} catch (...) {
		return -1;
	}
}

AGGWRAP_EXPIMPL const char* AGGWRAP_FUNC AwGetFontNameFromSet(AwFontSet_h hSet, int nFont) {
	try {
		return ((FontFile*)hSet)->GetFontName(nFont);
	} catch (...) {
		return (const char*)nullptr;
	}
}

AGGWRAP_EXPIMPL AwFont_h AGGWRAP_FUNC AwCreateFontFromSet(AwFontSet_h hSet, int nFont, int nHeightPixels) {
	try {
		return (AwFont_h)((FontFile*)hSet)->CreateFont(nFont, nHeightPixels);
	} catch (...) {
		return (AwFont_h)nullptr;
	}
}

AGGWRAP_EXPIMPL void AGGWRAP_FUNC AwDeleteFontSet(AwFontSet_h hSet) {
	try {
		delete (FontFile*)hSet;
	} catch (...) {}
}
