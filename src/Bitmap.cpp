#include "Bitmap.hpp"

using namespace AGGWrap;

static void AGGWRAP_FUNC s_DeleteSampleArray(void* parrSamples) {
	DeleteArray<Bitmap::Sample>((Bitmap::Sample*)parrSamples);
}

AGGWrap::Bitmap::Bitmap(int w, int h) {
	m_nWidth = w;
	m_nHeight = h;

	m_upData.Steal(UniquePointer<Sample>(new Sample[w * h], s_DeleteSampleArray));

	Attach();
}

AGGWrap::Bitmap::Bitmap(int w, int h, UniquePointer<Sample>& rupDataToSteal) {
	m_nWidth = w;
	m_nHeight = h;

	m_upData.Steal(rupDataToSteal);

	Attach();
}

AGGWrap::Bitmap::Bitmap(const Bitmap& rbm) {
	m_nWidth = rbm.m_nWidth;
	m_nHeight = rbm.m_nHeight;

	m_upData.Steal(UniquePointer<Sample>(new Sample[rbm.m_nWidth * rbm.m_nHeight], s_DeleteSampleArray));
	memcpy(m_upData.GetPointer(), rbm.m_upData.GetPointer(), GetDataSize());

	Attach();
}

AGGWrap::Bitmap::~Bitmap(void) {}

void AGGWrap::Bitmap::Attach(void) {
	m_buf.attach(m_upData.GetPointer(), (unsigned)m_nWidth, (unsigned)m_nHeight, GetStride());
	m_fmt.attach(m_buf);
}

AGGWRAP_EXPIMPL AwBitmap_h AGGWRAP_FUNC AwCreateBitmap(int w, int h) {
	try {
		return (AwBitmap_h)new Bitmap(w, h);
	} catch (...) {
		return nullptr;
	}
}

AGGWRAP_EXPIMPL AwBitmap_h AGGWRAP_FUNC AwCreateBitmapOnBuffer(int w, int h, void* pData, AwDataDestructor_t procDtor) {
	try {
		return (AwBitmap_h)new Bitmap(w, h, UniquePointer<Bitmap::Sample>((Bitmap::Sample*)pData, procDtor));
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
