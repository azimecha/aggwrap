#pragma once

#ifndef AGGWRAP_HEADER_TYPES
#define AGGWRAP_HEADER_TYPES

#include "AGGWrap.h"
#include <string.h>

#include <agg_basics.h>
#include <agg_pixfmt_rgba.h>

#ifdef AGGWRAP_NO_DELETED_FUNC
#define AGGWRAP_NOCOPY(t,tt) t(const tt& robj)
#define AGGWRAP_NOASSIGN(tt) tt& operator=(const tt& robj)
#else
#define AGGWRAP_NOCOPY(t,tt) t(const tt& robj) = delete
#define AGGWRAP_NOASSIGN(tt) tt& operator=(const tt& robj) = delete
#endif

#ifdef AGGWRAP_NO_NULLPTR
#define nullptr ((void*)0)
#endif

#define AGGWRAP_EXPIMPL extern "C"

namespace AGGWrap {
	class Exception {
	public:
		virtual const char* GetMessage(void) const = 0;
		virtual ~Exception(void);
	};

	class InvalidOperationException : public Exception {
	public:
		const char* GetMessage(void) const override;
		virtual ~InvalidOperationException(void);
	};

	template<typename T>
	void AGGWRAP_FUNC DeleteSingleObject(T* pObject) { delete pObject; }

	template<typename T>
	void AGGWRAP_FUNC DeleteArray(T* parrObjects) { delete[] parrObjects; }

	typedef agg::rgba8 Color;

	class OutOfRangeException : Exception {
	public:
		const char* GetMessage(void) const override;
		virtual ~OutOfRangeException(void);
	};

	template<typename T>
	class Array {
	public:
		constexpr inline Array(void) { 
			m_parrItems = (T*)nullptr; 
			m_nItems = 0; 
		}

		inline Array(int nItems) { 
			m_parrItems = new T[nItems]; 
			m_nItems = nItems; 
		}

		Array(const Array<T>& rarr) {
			m_parrItems = new T[rarr.m_nItems];
			m_nItems = rarr.m_nItems;
			memcpy((void*)m_parrItems, (const void*)rarr.m_parrItems, rarr.m_nItems);
		}

		Array<T>& operator=(const Array<T>& rarr) {
			if (&rarr != this) {
				m_parrItems = new T[rarr.m_nItems];
				m_nItems = rarr.m_nItems;
				memcpy((void*)m_parrItems, (const void*)rarr.m_parrItems, rarr.m_nItems);
			}
			return *this;
		}

		~Array(void) {
			T* parrItems = m_parrItems;

			m_nItems = 0;
			m_parrItems = (T*)nullptr;

			if (parrItems) delete[] m_parrItems;
		}

		inline T* operator->(void) { return m_parrItems; }
		inline const T* operator->(void) const { return m_parrItems; }
		inline T& operator*(void) { return *m_parrItems; }
		inline const T& operator*(void) const { return *m_parrItems; }

		inline T* GetPointer(void) { return m_parrItems; }
		inline const T* GetPointer(void) const { return m_parrItems; }

		inline T& operator[](int nItem) {
			if (nItem >= m_nItems)
				throw OutOfRangeException();

			return m_parrItems[nItem];
		}

		inline const T& operator[](int nItem) const {
			if (nItem >= m_nItems)
				throw OutOfRangeException();

			return m_parrItems[nItem];
		}

		inline int GetItemCount(void) const { return m_nItems; }

		inline void Sort(void) { if (m_parrItems) SortArray(m_parrItems, m_nItems); }

	private:
		T* m_parrItems;
		int m_nItems;
	};

	template<typename T>
	void SortArray(T* parrItems, int nItems) {
		bool bUnsorted;
		int i;
		T temp;

		bUnsorted = true;

		while (bUnsorted) {
			bUnsorted = false;
			for (i = 0; i < (nItems - 1); i++) {
				if (parrItems[i] > parrItems[i + 1]) {
					temp = parrItems[i + 1];
					parrItems[i + 1] = parrItems[i];
					parrItems[i] = temp;
					bUnsorted = true;
				}
			}
		}
	}

	class NullPointerException : Exception {
	public:
		const char* GetMessage(void) const override;
		virtual ~NullPointerException(void);
	};

	template<typename T>
	class SharedObject {
	private:
		SharedObject(T* pObject, int nInitRefCount) {
			if (pObject == nullptr)
				throw NullPointerException();

			m_pObject = pObject;
			m_nRefCount = nInitRefCount;
		}

		~SharedObject(void) {
			T* pObject = m_pObject;
			m_pObject = (T*)nullptr;
			delete pObject;
		}

	public:
		static inline SharedObject<T>* Create(T* pObject, int nInitRefCount) {
			return new SharedObject<T>(pObject, nInitRefCount);
		}
		
		inline void AddRef(void) { m_nRefCount++; }
		inline void RemoveRef(void) {
			m_nRefCount--;
			if (m_nRefCount == 0)
				delete this;
		}

		inline T* GetPointer(void) { return m_pObject; }
		inline const T* GetPointer(void) const { return m_pObject; }

		inline T* operator->(void) { return m_pObject; }
		inline const T* operator->(void) const { return m_pObject; }

		inline T& operator*(void) { return *m_pObject; }
		inline const T& operator*(void) const { return *m_pObject; }

	private:
		AGGWRAP_NOCOPY(SharedObject, SharedObject<T>);
		AGGWRAP_NOASSIGN(SharedObject<T>);

		T* m_pObject;
		int m_nRefCount;
	};
}

#endif
