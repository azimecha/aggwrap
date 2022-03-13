#pragma once

#ifndef AGGWRAP_HEADER_TTF
#define AGGWRAP_HEADER_TTF

#include "Types.hpp"
#include "Font.hpp"
#include "stb_truetype.h"

namespace AGGWrap {
	class FontFile {
	public:
		virtual ~FontFile(void);

		virtual int GetFontCount(void) const = 0;
		virtual const char* GetFontName(int nFont) const = 0;
		virtual Font* CreateFont(int nFont, int nHeight) const = 0;
	};

	class FontInitializationException : public Exception {
	public:
		const char* GetMessage(void) const override;
		virtual ~FontInitializationException(void);
	};

	class TrueTypeFontFile : public FontFile {
	public:
		TrueTypeFontFile(SharedObject<BufferOf<AwByte_t>>& rsoData);
		virtual ~TrueTypeFontFile(void);

		int GetFontCount(void) const override;
		const char* GetFontName(int nFont) const override;
		Font* CreateFont(int nFont, int nHeight) const override;

	private:
		AGGWRAP_NOCOPY(TrueTypeFontFile, TrueTypeFontFile);
		AGGWRAP_NOASSIGN(TrueTypeFontFile);

		class FontFace {
		public:
			FontFace(void);			
			void Initialize(AwByte_t* pData, int nOffset);

		private:
			Array<char> m_arrName;
			stbtt_fontinfo m_infFont;
			int m_nOffset;

		public:
			inline const char* GetName(void) const { return m_arrName.GetPointer(); }
			inline const stbtt_fontinfo& GetInfo(void) const { return m_infFont; }
			inline int GetOffset(void) const { return m_nOffset; }
		};

		SharedObjectReference<BufferOf<AwByte_t>> m_refBuffer;
		Array<FontFace> m_arrFaces;
	};
}

#endif
