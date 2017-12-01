//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) The Byte Kitchen. All rights reserved

#include "stdafx.h"
#include "symbolrun.h"
#include <iostream>

using namespace std;

namespace rlecodec
{
	symbolrun::symbolrun(unsigned int bytesz)
	{
		shared_buf = shared_ptr<unsigned char>(new unsigned char[bytesz],
			[this](unsigned char* p)
		{
			delete[] p;
		});

		buf = shared_buf.get();
		pos = buf;
		end = buf + bytesz;
	}

	symbolrun::~symbolrun()
	{
		buf = pos = end = 0;
	}
}