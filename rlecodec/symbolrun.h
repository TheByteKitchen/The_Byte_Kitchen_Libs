//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) The Byte Kitchen. All rights reserved

#pragma once

#include <vector>
#include <memory>

using std::vector;

namespace rlecodec
{
	class symbolrun
	{
		unsigned char* buf, *end, *pos;
		std::shared_ptr<unsigned char> shared_buf;

		template<typename T> friend inline symbolrun& operator<<(symbolrun &sr, const T& v);
		template<typename T> friend inline symbolrun& operator>>(symbolrun &sr, T& v);

		symbolrun();

	public:
		symbolrun(unsigned int bytesz);
		~symbolrun();

		inline unsigned int count()
		{
			return pos - buf;
		}

		inline unsigned int size()
		{
			return end - buf;
		}

		inline unsigned char* seek(unsigned int p)
		{
			if (p >= (unsigned int)(end - buf))
				throw std::invalid_argument("Offset too high");

			pos = buf + p;
			return pos;
		}

		inline std::shared_ptr<unsigned char> data() { return shared_buf; }
	};

	template<typename T> inline symbolrun& operator<<(symbolrun &sr, const T& v)
	{
		auto p = reinterpret_cast<const char*>(&v);
		memcpy(sr.pos, p, sizeof(T));
		sr.pos += sizeof(T);

		return sr;
	};

	template<typename T> inline symbolrun& operator>>(symbolrun &sr, T& v)
	{
		auto p = reinterpret_cast<char*>(&v);
		memcpy(p, sr.pos, sizeof(T));
		sr.pos += sizeof(T);

		return sr;
	};
}
