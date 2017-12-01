//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) The Byte Kitchen. All rights reserved

#pragma once
#pragma warning( disable : 4351)

#include "stdafx.h"
#include "depths.h"
#include <random>
#include <algorithm>
#include <limits>

namespace dmfilter
{
	using std::lower_bound;
	using std::begin;
	using std::end;

	// Traits classes
	template<typename T>
	struct dmf_traits;

	template<typename T>
	struct dmf_pol;

	template<>
	struct dmf_traits<unsigned char>
	{
		static unsigned int const Radius = 7; // 3, 5, 7, 13
		static unsigned int const Span = 2 * Radius;
		static unsigned int const Channels = 3;
		static unsigned int const Offset = 4;
		static unsigned char const Max =  UCHAR_MAX;
		static unsigned char const Min = 0;

		static_assert(Channels > 0, "The number of Channels must be positive");
		static_assert(Channels - Offset >= 0, "The number of Channels cannot not exceed the Offset");
	};

	template<>
	struct dmf_pol<unsigned char>
	{
		static bool Illegal(unsigned short val)
		{
			return false;
		}

		static void SetMinMax(unsigned char const val, unsigned char minmax[])
		{
			// Too close to the lower bound
			if (val <= dmf_traits<unsigned char>::Radius)
			{
				minmax[0] = dmf_traits<unsigned char>::Min;
				minmax[1] = dmf_traits<unsigned char>::Span;
			}
			// Too close to the end
			else if (val >= dmf_traits<unsigned char>::Max - dmf_traits<unsigned char>::Radius)
			{
				minmax[0] = dmf_traits<unsigned char>::Max - dmf_traits<unsigned char>::Span;
				minmax[1] = dmf_traits<unsigned char>::Max;
			}
			else
			{
				minmax[0] = val - dmf_traits<unsigned char>::Radius;
				minmax[1] = val + dmf_traits<unsigned char>::Radius;
			}
		}
	};

	template<>
	struct dmf_traits<unsigned short>
	{
		static unsigned int const Radius = 3;
		static unsigned int const Span = 2 * Radius;
		static unsigned int const Channels = 1;		// The nr. of channels to be compressed. RGB: 3, Depth: 1 or 2
		static unsigned int const Offset = 1;		// Offset between values of the same channel. Depth: 1, RGBX: 4
		static unsigned short const Max = DEPTHS_MAXINDEX;
		static unsigned short const Min = 0;

		static_assert(Channels > 0, "The number of Channels must be positive");
		static_assert(Channels - Offset >= 0, "The number of Channels cannot not exceed the Offset");
	};

	template<>
	struct dmf_pol<unsigned short>
	{
		static unsigned int const unknown = 65528;
		static unsigned int const too_far = 32760;
		static unsigned int const too_near = 0;

		static bool Illegal(unsigned short val)
		{
			return val == too_near || val == unknown || val == too_far;
		}

		static void SetMinMax(unsigned short const val, unsigned short minmax[])
		{
			//locate the new value in depths
			auto p = lower_bound(begin(depths), end(depths), val);
			unsigned int idx = p - begin(depths);

			int b, e;
			// Too close to the lower bound
			if (idx <= dmf_traits<unsigned short>::Radius)
			{
				b = dmf_traits<unsigned short>::Min;
				e = dmf_traits<unsigned short>::Span;
			}
			// Too close to the end
			else if (idx >= dmf_traits<unsigned short>::Max - dmf_traits<unsigned short>::Radius)
			{
				b = dmf_traits<unsigned short>::Max - dmf_traits<unsigned short>::Span;
				e = dmf_traits<unsigned short>::Max;
			}
			else
			{
				b = idx - dmf_traits<unsigned short>::Radius;
				e = idx + dmf_traits<unsigned short>::Radius;
			}

			minmax[0] = depths[b];
			minmax[1] = depths[e];
		}
	};

	template<>
	struct dmf_traits<unsigned int>
	{
		static unsigned int const Radius = 3;
		static unsigned int const Span = 2 * Radius;
		static unsigned int const Channels = 1;
		static unsigned int const Offset = 1;
		static unsigned int const Max = UINT_MAX;
		static unsigned int const Min = 0;

		static_assert(Channels > 0, "The number of Channels must be positive");
		static_assert(Channels - Offset >= 0, "The number of Channels cannot not exceed the Offset");
	};

	template<>
	struct dmf_pol<unsigned int>
	{
		static bool Illegal(unsigned int val)
		{
			return false;
		}

		static void SetMinMax(unsigned int const val, unsigned int minmax[])
		{
			// Too close to the lower bound
			if (val <= dmf_traits<unsigned int>::Radius)
			{
				minmax[0] = dmf_traits<unsigned int>::Min;
				minmax[1] = dmf_traits<unsigned int>::Span;
			}
			// Too close to the end
			else if (val >= dmf_traits<unsigned int>::Max - dmf_traits<unsigned int>::Radius)
			{
				minmax[0] = dmf_traits<unsigned int>::Max - dmf_traits<unsigned int>::Span;
				minmax[1] = dmf_traits<unsigned int>::Max;
			}
			else
			{
				minmax[0] = val - dmf_traits<unsigned int>::Radius;
				minmax[1] = val + dmf_traits<unsigned int>::Radius;
			}
		}
	};

	template <typename T, unsigned int nr_elems, bool noisy=true>
	class DiscreteMedianFilter
	{
		static unsigned int const random_perc = 10;
		// Nr of elems in the random fraction
		// Note the compenstation for #channels < offset
		static unsigned int const random_fraction = noisy ? (random_perc * dmf_traits<T>::Offset * nr_elems) / (100 * dmf_traits<T>::Channels) : 0;
		
		std::default_random_engine dre;
		std::uniform_int_distribution<unsigned int> dist;

		T minmaxs[nr_elems][2];

		inline void FilterSinglePixel(T const& in, T& out, unsigned int const idx);

	public:
		DiscreteMedianFilter() : dre(), dist(0, nr_elems), minmaxs() { }

		// Filter changes the State with Delta where the change is larger than Radius
		void Filter(T const* pDelta, T* pState);
	};

	// Hier de Channels en offset verwerken
	template<typename T, unsigned int nr_elems, bool noisy>
	void DiscreteMedianFilter<T, nr_elems, noisy>::Filter(T const* pDelta, T* pState)
	{
		for (unsigned int i = 0; i < nr_elems; i += dmf_traits<T>::Offset)
		{
			// pixels in the channels
			for (unsigned j = 0; j < dmf_traits<T>::Channels; ++j)
			{
				FilterSinglePixel(*pDelta++, *pState++, i + j);
			}
			// pixels out of the channels
			for (unsigned j = 0; j < dmf_traits<T>::Offset - dmf_traits<T>::Channels; ++j)
			{
				++pState;
				++pDelta;
			}
		}
	}

	// In case of depth values, we work with the actual values
	template<typename T, unsigned int nr_elems, bool noisy>
	inline void DiscreteMedianFilter<T, nr_elems, noisy>::FilterSinglePixel(T const& in, T& out, unsigned int const idx)
	{
		auto m = minmaxs[idx];

		if (dmf_pol<T>::Illegal(in))
		{
			out = in;
		}
		if (in < m[0] || in > m[1])
		{
			dmf_pol<T>::SetMinMax(in, m);
			out = in;
		}
		// Noise is a performance killer (divides perf by about 15)
		else if (random_fraction && dist(dre) < random_fraction)
		{
			dmf_pol<T>::SetMinMax(in, m);
			out = in;
		}

		// else: keep current out
	}

}