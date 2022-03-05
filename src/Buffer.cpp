#include "Buffer.hpp"

static void AGGWRAP_FUNC s_DeleteByteBuffer(void* pData, void* tagIgnored) {
	delete[] (AwByte_p)pData;
}

AGGWrap::Buffer::Buffer(int nSize) {
	m_infBuffer.pData = new AwByte_t[nSize];
	m_infBuffer.procDestructor = s_DeleteByteBuffer;
	m_infBuffer.nDataSize = nSize;
	m_infBuffer.tag = nullptr;
}

AGGWrap::Buffer::Buffer(AwBufferInfo_t& rinfBuf) {
	m_infBuffer = rinfBuf;
}

AGGWrap::Buffer::~Buffer(void) {
	if (m_infBuffer.pData && m_infBuffer.procDestructor)
		m_infBuffer.procDestructor(m_infBuffer.pData, m_infBuffer.tag);
	memset((void*)&m_infBuffer, 0, sizeof(m_infBuffer));
}

void AGGWrap::Buffer::Steal(Buffer& rbuf) {
	if (&rbuf != this) {
		if (m_infBuffer.pData != nullptr)
			throw InvalidOperationException();

		m_infBuffer = rbuf.m_infBuffer;
		memset((void*)&rbuf.m_infBuffer, 0, sizeof(rbuf.m_infBuffer));
	}
}
