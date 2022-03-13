#include "Bitmap.hpp"
#include <stb_image.h>

using namespace AGGWrap;

AGGWrap::Bitmap::Bitmap(int w, int h) {
	m_nWidth = w;
	m_nHeight = h;

	BufferOf<Sample> bufNew(w * h * SAMPLES_PER_PIXEL);
	m_bufData.Steal(bufNew);

	Attach();
}

AGGWrap::Bitmap::Bitmap(int w, int h, BufferOf<Sample>& rbufSteal) {
	m_nWidth = w;
	m_nHeight = h;

	m_bufData.Steal(rbufSteal);

	Attach();
}

AGGWrap::Bitmap::Bitmap(const Bitmap& rbm) {
	m_nWidth = rbm.m_nWidth;
	m_nHeight = rbm.m_nHeight;

	BufferOf<Sample> bufCopy(rbm.m_nWidth * rbm.m_nHeight * SAMPLES_PER_PIXEL);
	m_bufData.Steal(bufCopy);
	memcpy(m_bufData.GetDataPointer(), rbm.m_bufData.GetDataPointer(), m_bufData.GetInfo().nDataSize);

	Attach();
}

static void AGGWRAP_FUNC s_FreeLoadedImageData(void* pData, void* tag) {
	stbi_image_free(pData);
}

static void s_SwapRedBlue(Bitmap::PixelFormat::pixel_type* pData, size_t nPixels) {
	for (; nPixels > 0; pData++, nPixels--) {
		register Bitmap::PixelFormat::value_type cTemp = pData->c[0];
		pData->c[0] = pData->c[2];
		pData->c[2] = cTemp;
	}
}

AGGWrap::Bitmap::Bitmap(const FileData* pFileData, int nFileSize) {
	// stb_image provides no way to make images always load premultiplied, or detect if an image is premultiplied already
	// the only option to ensure correct loading is to enable this flag and then always premultiply
	stbi_set_unpremultiply_on_load(1);

	int nChannelsInFile;
	stbi_uc* pPixelData = stbi_load_from_memory((const stbi_uc*)pFileData, nFileSize, &m_nWidth, &m_nHeight, &nChannelsInFile, 4);
	if (pPixelData == (stbi_uc*)nullptr)
		throw FileLoadException();

	BufferOf<Sample> bufData((Sample*)pPixelData, m_nWidth * m_nHeight, s_FreeLoadedImageData, nullptr);
	s_SwapRedBlue((Bitmap::PixelFormat::pixel_type*)pPixelData, (size_t)(m_nWidth * m_nHeight));
	m_bufData.Steal(bufData);

	Attach();
	m_fmt.premultiply();
}

AGGWrap::Bitmap::~Bitmap(void) {}

void AGGWrap::Bitmap::Attach(void) {
	m_buf.attach(m_bufData.GetArrayPointer(), (unsigned)m_nWidth, (unsigned)m_nHeight, GetStride());
	m_fmt.attach(m_buf);
}

AGGWRAP_EXPIMPL AwBitmap_h AGGWRAP_FUNC AwCreateBitmap(int w, int h) {
	try {
		return (AwBitmap_h)new Bitmap(w, h);
	} catch (...) {
		return nullptr;
	}
}

AGGWRAP_EXPIMPL AwBitmap_h AGGWRAP_FUNC AwCreateBitmapOnBuffer(int w, int h, AwBufferInfo_p pinfBuffer) {
	BufferOf<Bitmap::Sample> buf(*pinfBuffer);

	try {
		return (AwBitmap_h)new Bitmap(w, h, buf);
	} catch (...) {
		// don't destroy the client's buffer if we're returning an error
		buf.Detach();
		return nullptr;
	}
}

AGGWRAP_EXPIMPL AwBitmap_h AGGWRAP_FUNC AwLoadBitmap(const void* pImageFileData, AwDataSize_t nImageFileSize) {
	try {
		if (nImageFileSize > INT_MAX)
			throw new FileLoadException();

		return (AwBitmap_h)new Bitmap((const Bitmap::FileData*)pImageFileData, (int)nImageFileSize);
	} catch (...) {
		return (AwBitmap_h)nullptr;
	}
}

AGGWRAP_EXPIMPL AwBitmap_h AGGWRAP_FUNC AwDuplicateBitmap(AwBitmap_h hBitmap) {
	try {
		return (AwBitmap_h)new Bitmap(*(Bitmap*)hBitmap);
	} catch (...) {
		return nullptr;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwGetBitmapSize(AwBitmap_h hBitmap, AGGWRAP_OUT int* pnWidth, AGGWRAP_OUT int* pnHeight) {
	try {
		*pnWidth = ((Bitmap*)hBitmap)->GetWidth();
		*pnHeight = ((Bitmap*)hBitmap)->GetHeight();
		return AGGWRAP_TRUE;
	} catch (...) {
		return AGGWRAP_FALSE;
	}
}

AGGWRAP_EXPIMPL AwBool_t AGGWRAP_FUNC AwGetBitmapDataSize(AwBitmap_h hBitmap, AGGWRAP_OPTIONAL AGGWRAP_OUT int* pnStride, AGGWRAP_OPTIONAL AGGWRAP_OUT int* pnFullSize) {
	try {
		if (pnStride) *pnStride = ((Bitmap*)hBitmap)->GetStride();
		if (pnFullSize) *pnFullSize = ((Bitmap*)hBitmap)->GetDataSize();
		return AGGWRAP_TRUE;
	} catch (...) {
		return AGGWRAP_FALSE;
	}
}

AGGWRAP_EXPIMPL void AGGWRAP_FUNC AwDeleteBitmap(AwBitmap_h hBitmap) {
	try {
		delete (Bitmap*)hBitmap;
	} catch (...) { }
}

AGGWRAP_EXPIMPL void* AGGWRAP_FUNC AwAccessBitmapData(AwBitmap_h hBitmap) {
	try {
		return (void*)((Bitmap*)hBitmap)->GetDataPointer();
	} catch (...) {
		return nullptr;
	}
}
