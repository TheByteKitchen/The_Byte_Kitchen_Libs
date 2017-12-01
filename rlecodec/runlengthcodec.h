//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) The Byte Kitchen. All rights reserved

/*
	BE CAREFUL! THIS IS NOT A GENERAL PURPOSE COMPRESSION LIBRARY
	*/

#pragma once
#pragma warning( disable : 4351)

#include "stdafx.h"
#include "symbolrun.h"
#include <memory>
#include <numeric>
#include <cassert>
#include <ppl.h>

#if _ITERATOR_DEBUG_LEVEL == 0 &&  _DEBUG
#define IT_CHCK
#endif

namespace rlecodec
{
	template<typename T>
	struct rle_traits;

	template<>
	struct rle_traits<unsigned short>
	{
		typedef unsigned short VT;					// Value type, the type of the data to be compressed
		static const unsigned int Channels = 2;		// The nr. of channels to be compressed. RGB: 3, Depth: 1 or 2
		static const unsigned int Offset = 2;		// Offset between values of the same channel. Depth: 1, RGBX: 4
		static const VT Empty = 11;					// The 'empty' symbol. For depth we use 11. 

		static_assert(Channels > 0, "The number of Channels must be positive");
		static_assert(Channels <= Offset, "The number of Channels cannot not exceed the Offset");
	};

	template<>
	struct rle_traits<unsigned int>
	{
		typedef unsigned int VT;
		static const unsigned int Channels = 4;
		static const unsigned int Offset = 4;
		static const VT Empty = 0;

		static_assert(Channels > 0, "The number of Channels must be positive");
		static_assert(Channels <= Offset, "The number of Channels cannot not exceed the Offset");
	};

	template<>
	struct rle_traits<unsigned char>
	{
		typedef unsigned char VT;
		static const unsigned int Channels = 3;
		static const unsigned int Offset = 4;
		static const VT Empty = 0;

		static_assert(Channels > 0, "The number of Channels must be positive");
		static_assert(Channels <= Offset, "The number of Channels cannot not exceed the Offset");
	};

	template<typename T>
	class runlengthcodec
	{
		static const unsigned int C = rle_traits<T>::Channels;
		static const unsigned int O = rle_traits<T>::Offset;
		static const T E = rle_traits<T>::Empty;

		static const double safety;
		static const short single = 1; // enumerates a final single symbol to be compressed
		static const unsigned int runsz = sizeof(short);
		static const unsigned int symsz = sizeof(T);

		symbolrun sr;
		vector<symbolrun> sr_tmp;
		vector<T> vPrev, vDelta;

		inline void delta(vector<T> const &vIn);
		inline void update(vector<T> &vOut);
		inline void runlengthcodec<T>::encodeloop(typename vector<T>::const_iterator it, typename vector<T>::const_iterator end, symbolrun& sr);
		inline unsigned int runlengthcodec<T>::decodeloop(typename vector<T>::iterator& it, unsigned int length, symbolrun& sr);
		inline typename vector<T>::const_iterator& encodelit(typename vector<T>::const_iterator& it, typename vector<T>::const_iterator& end, symbolrun& sr);
		inline typename vector<T>::const_iterator& encoderun(typename vector<T>::const_iterator& it, typename vector<T>::const_iterator& end, symbolrun& sr);
		// Returns a pointer to a buffer with all compressed channels concatenated
		inline void pack();
		// Takes a pointer to a buffer and distributes the data over the channels according to channelsize(s)
		inline void unpack();
	public:
		unsigned int channelsize[C];

		runlengthcodec(unsigned int elem_size) : sr((unsigned int)(elem_size * sizeof(T)* safety)), channelsize(), vPrev(elem_size), vDelta(elem_size)
		{
			for (unsigned i = 0; i < C; ++i)
			{
				sr_tmp.emplace_back((unsigned int)(elem_size * sizeof(T)));
			}
		}

		unsigned int size() { return std::accumulate(cbegin(channelsize), cend(channelsize), 0); }

		std::shared_ptr<unsigned char> data() { return sr.data(); }

		/*
			Creates a delta of the input data and the previous input.
			The 'previous input' of the first input is a zero vector.
			The delta is compressed.
			The input is stored as the previous input.
			*/
		void encode(vector<T> const &vIn);

		/*
			Decodes the compressed data and updates vOut.
			vOut is considered the 'previous output'.
			Therefore, the initial output needs to be zoeroed out.
			This is a responsibility of user code.
			The decompressed data is considered differential data.
			Hence, 'to update' means: A zero value in the decompressed data
			leaves the corresponding value in vOut unchanged.
			*/
		unsigned int decode(std::vector<T>&  vOut);
	};

	template<typename T>
	const double runlengthcodec<T>::safety = 1.5;

	// Sets vDelta[i] to vNew[i], at all i where vOld[i] != vNew[i], sets vDelta[i] = 0 otherwise
	template<typename T>
	inline void runlengthcodec<T>::delta(vector<T> const& vIn)
	{
		auto itib = vIn.cbegin();
		auto itie = vIn.cend();
		auto itpb = vPrev.cbegin();
		auto itdb = vDelta.begin();

		while (itib != itie)
		{
			for (unsigned int i = 0; i < C; ++i)
			{
				*itdb++ = (*itpb++ == *itib) ? E : *itib;
				++itib;
			}

			for (unsigned int i = 0; i < (O - C); ++i)
			{
				*itdb++ = E;
				++itpb;
				++itib;
			}
		}
	}

	template<unsigned int C, typename T>
	class UpdateUnrollChannels
	{
	public:
		static void result(typename vector<T>::iterator& itob, typename vector<T>::const_iterator& itdb)
		{
			if (*itdb != rle_traits<T>::Empty) *itob = *itdb;

			UpdateUnrollChannels<C - 1, T>::result(++itob, ++itdb);
		}
	};

	template<typename T>
	class UpdateUnrollChannels<0, T>
	{
	public:
		static void result(typename vector<T>::iterator& itob, typename vector<T>::const_iterator& itdb) { }
	};

	template<unsigned int O_C, typename T>
	class UpdateUnrollZeroes
	{
	public:
		static void result(typename vector<T>::iterator& itob, typename vector<T>::const_iterator& itdb)
		{
			*itob = rle_traits<T>::Empty;

			UpdateUnrollZeroes<O_C - 1, T>::result(++itob, ++itdb);
		}
	};

	template<typename T>
	class UpdateUnrollZeroes<0, T>
	{
	public:
		static void result(typename vector<T>::iterator itob, typename vector<T>::const_iterator& itdb) { }
	};

	// Sets vOld to vDelta at all indexes i where vDelta[i] != 0
	template<typename T>
	inline void runlengthcodec<T>::update(vector<T> &vOut)
	{
		auto itob = vOut.begin();
		auto itoe = vOut.end();
		auto itdb = vDelta.cbegin();

		while (itob != itoe)
		{
			UpdateUnrollChannels<C, T>::result(itob, itdb);

			UpdateUnrollZeroes<O - C, T>::result(itob, itdb);
		}
	}

	template<typename T>
	inline typename vector<T>::const_iterator& runlengthcodec<T>::encodelit(typename vector<T>::const_iterator& it, typename vector<T>::const_iterator& end, symbolrun& sr)
	{
		/*
		- bij entry is *it 1ste elem van literal, i+offset < end
		- bij return is *it eerste elem nieuwe reeks, of end
		*/

		short len = -1;
		auto lit = it;

		while (it += O, it < end)
		{
			if (*it != *(it - O))
				--len;
			else
			{
				// one step back
				++len;
				it -= O;

				break;
			}
		}

		sr << len;
		do sr << *lit; while (lit += O, lit < it);

		return it;
	}

	template<typename T>
	inline typename vector<T>::const_iterator& runlengthcodec<T>::encoderun(typename vector<T>::const_iterator& it, typename vector<T>::const_iterator& end, symbolrun& sr)
	{
		/*
		- bij aanvang is *it 1ste elem van run, i+offset < end
		- bij return is *it eerste elem nieuwe reeks, of end
		*/

		short len = 1;
		auto sym = *it;

		while (it += O, it < end)
		{
			if (*it == *(it - O))
				++len;
			else
				break;
		}

		sr << len << sym;

		return it;
	}

	template<typename T>
	inline void runlengthcodec<T>::encodeloop(typename vector<T>::const_iterator it, typename vector<T>::const_iterator end, symbolrun& sr)
	{
		while (it < end)
		{
			if (it + O >= end)
			{
				sr << single << *it; // Final single symbol, save it as a run of length 1;
				it += O;

				continue;
			}
#ifdef IT_CHCK
			if (end - (it + O) < 0)
				cout << "******* After this message, memory after 'end' will be dereferenced with offset: " << (it + O) - end << endl;
#endif
			if (*it == *(it + O))
			{
				encoderun(it, end, sr);
			}
			else
			{
				encodelit(it, end, sr);
			}
		}
	}

	template<typename T>
	void runlengthcodec<T>::encode(vector<T> const &vIn)
	{
		assert(vIn.size() >= O);

		// create delta
		delta(vIn);

		// copy input tot previous input
		memcpy((void*)vPrev.data(), (void*)vIn.data(), vIn.size() * sizeof(T));

		// compress the delta
		sr.seek(0);
		memset((char*)channelsize, 0, C * sizeof(unsigned int));

		auto end = vDelta.cend();
		concurrency::parallel_for(unsigned int(0), C, [&](unsigned int i)
		{
			sr_tmp[i].seek(0);

			encodeloop(vDelta.cbegin() + i, end, sr_tmp[i]);

			channelsize[i] = sr_tmp[i].count();
		});

		pack();
	}

	// Packs the sr_tmp[i] into sr.buf, in order of increasing i, returns sr.buf
	template<typename T>
	inline void runlengthcodec<T>::pack()
	{
		auto dst = sr.data().get();
		for (unsigned i = 0; i < C; ++i)
		{
			memcpy(dst, sr_tmp[i].data().get(), channelsize[i]);
			dst += channelsize[i];
		}
	}

	template<typename T>
	inline unsigned int runlengthcodec<T>::decodeloop(typename vector<T>::iterator& it, unsigned int length, symbolrun& sr)
	{
		unsigned int bytecnt = 0;
		short cnt;
		T sym;

		auto prev = it;
		while (bytecnt < length)
		{
			sr >> cnt;
			bytecnt += runsz;

			if (cnt > 0) // run
			{
				sr >> sym;
				bytecnt += symsz;

				for (int i = 0; i < cnt; ++i)
				{
					*it = sym;
					it += O;
				}
			}
			else // literal. case < 0; cnt cannot be 0
			{
				for (int i = 0; i < -cnt; ++i)
				{
					sr >> *it;
					it += O;
					bytecnt += symsz;
				}
			}
		}

		return it - prev;
	}

	template<typename T>
	unsigned int runlengthcodec<T>::decode(std::vector<T>&  vOut)
	{
		unsigned int offset = 0;
		vector<unsigned int> acc(C);

		unpack();

		concurrency::parallel_for(unsigned int(0), C, [&](unsigned int i)
		{
			sr_tmp[i].seek(0);

			acc[i] = decodeloop(vDelta.begin() + i, channelsize[i], sr_tmp[i]);
		});

		update(vOut);

		// Round down by cut off, see if all bytes are there, all went well
		return (unsigned int)(std::accumulate(acc.cbegin(), acc.cend(), 0) / (double)C);
	}

	// Unpacks sr.buf into the sr_tmp[i], channelsize[i] bytes per sr_tmp[i]
	template<typename T>
	inline void runlengthcodec<T>::unpack()
	{
		auto src = sr.data().get();

		for (unsigned i = 0; i < C; ++i)
		{
			memcpy(sr_tmp[i].data().get(), src, channelsize[i]);
			src += channelsize[i];
		}
	}
}
