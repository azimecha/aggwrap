#pragma once

#ifndef AGGWRAP_HEADER_TYPES
#define AGGWRAP_HEADER_TYPES

#include "AGGWrap.h"
#include <string.h>

#include <agg_basics.h>
#include <agg_pixfmt_rgba.h>

#define AGGWRAP_NOCOPY(t,tt) t(const tt& robj) = delete
#define AGGWRAP_NOASSIGN(tt) tt& operator=(const tt& robj) = delete

#define AGGWRAP_EXPIMPL extern "C"

namespace AGGWrap {
	template<typename T>
	class UniquePointer {
	public:
		constexpr inline UniquePointer(void) {
			m_pData = nullptr;
			m_procDtor = nullptr;
		}

		inline UniquePointer(UniquePointer<T>& rupSteal) {
			m_pData = rupSteal.m_pData;
			m_procDtor = rupSteal.m_procDtor;
			rupSteal.m_pData = nullptr;
			rupSteal.m_procDtor = nullptr;
		}

		constexpr inline UniquePointer(T* pData, AwDataDestructor_t procDtor) {
			m_pData = pData;
			m_procDtor = procDtor;
		}

		inline void Steal(UniquePointer<T>& rupStealFrom) {
			if (&rupStealFrom != this) {
				m_pData = rupStealFrom.m_pData;
				m_procDtor = rupStealFrom.m_procDtor;
				rupStealFrom.m_pData = nullptr;
				rupStealFrom.m_procDtor = nullptr;
			}
		}

		inline ~UniquePointer(void) {
			T* pData = m_pData;
			AwDataDestructor_t procDtor = m_procDtor;

			m_pData = nullptr; 
			m_procDtor = nullptr;

			if (procDtor)
				procDtor((void*)pData);
		}

		inline T* operator->(void) { return m_pData; }
		inline const T* operator->(void) const { return m_pData; }
		inline T& operator*(void) { return *m_pData; }
		inline const T& operator*(void) const { return *m_pData; }

		inline T* GetPointer(void) { return m_pData; }
		inline const T* GetPointer(void) const { return m_pData; }

	private:
		AGGWRAP_NOASSIGN(UniquePointer<T>);

		T* m_pData;
		AwDataDestructor_t m_procDtor;
	};

	template<typename T>
	void AGGWRAP_FUNC DeleteSingleObject(T* pObject) { delete pObject; }

	template<typename T>
	void AGGWRAP_FUNC DeleteArray(T* parrObjects) { delete[] parrObjects; }

	typedef agg::rgba8 Color;

	class Exception {
	public:
		virtual const char* GetMessage(void) const = 0;
		virtual ~Exception(void);
	};

	class OutOfRangeException : Exception {
	public:
		const char* GetMessage(void) const override;
		virtual ~OutOfRangeException(void);
	};

	template<typename T>
	class Array {
	public:
		constexpr inline Array(void) { 
			m_parrItems = nullptr; 
			m_nItems = 0; 
		}

		inline Array(int nItems) { 
			m_parrItems = new T[nItems]; 
			m_nItems = nItems; 
		}

		inline Array(const Array<T>& rarr) {
			m_parrItems = new T[rarr.m_nItems];
			m_nItems = rarr.m_nItems;
			memcpy((void*)m_parrItems, (const void*)rarr.m_parrItems, rarr.m_nItems);
		}

		inline Array<T>& operator=(const Array<T>& rarr) {
			m_parrItems = new T[rarr.m_nItems];
			m_nItems = rarr.m_nItems;
			memcpy((void*)m_parrItems, (const void*)rarr.m_parrItems, rarr.m_nItems);
		}

		inline ~Array(void) {
			T* parrItems = m_parrItems;

			m_nItems = 0;
			m_parrItems = nullptr;

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
}

#endif
