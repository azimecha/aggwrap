#include "Bitmap.hpp"

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
	try {
		BufferOf<Bitmap::Sample> buf(*pinfBuffer);
		return (AwBitmap_h)new Bitmap(w, h, buf);
	} catch (...) {
		return nullptr;
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
