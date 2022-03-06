#pragma once

#ifndef AGGWRAP_HEADER_BUFFER
#define AGGWRAP_HEADER_BUFFER

#include "Types.hpp"

namespace AGGWrap {
	class Buffer {
	public:
		constexpr inline Buffer(void) : m_infBuffer() {}
		Buffer(int nSize);
		Buffer(AwBufferInfo_t& rinfBuf);

		virtual ~Buffer(void);

		inline const AwBufferInfo_t& GetInfo(void) const { return m_infBuffer; }

		inline void* GetDataPointer(void) { return m_infBuffer.pData; }
		inline const void* GetDataPointer(void) const { return m_infBuffer.pData; }

		void Steal(Buffer& rbuf);

	private:
		AGGWRAP_NOCOPY(Buffer, Buffer);
		AGGWRAP_NOASSIGN(Buffer);

		AwBufferInfo_t m_infBuffer;
	};

	template<typename T>
	class BufferOf : public Buffer {
	public:
		inline BufferOf(void) : Buffer() {}
		inline BufferOf(int nItems) : Buffer(nItems * sizeof(T)) {}
		inline BufferOf(AwBufferInfo_t& rinfBuf) : Buffer(rinfBuf) {}

		virtual ~BufferOf(void) {}

		inline T* GetArrayPointer(void) { return (T*)GetDataPointer(); }
		inline const T* GetArrayPointer(void) const { return (T*)GetDataPointer(); }

		inline T& operator[](int nItem) { 
			if ((nItem < 0) || (nItem >= GetItemCount()))
				throw OutOfRangeException();
			return GetArrayPointer()[nItem]; 
		}

		inline const T& operator[](int nItem) const {
			if ((nItem < 0) || (nItem >= GetItemCount()))
				throw OutOfRangeException();
			return GetArrayPointer()[nItem]; 
		}

		inline int GetItemCount(void) const { return (int)(GetInfo().nDataSize / sizeof(T)); }

	private:
		AGGWRAP_NOCOPY(BufferOf, BufferOf<T>);
		AGGWRAP_NOASSIGN(BufferOf<T>);
	};
}

#endif
