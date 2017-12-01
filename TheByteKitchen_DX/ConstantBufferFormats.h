//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved
////
//// Adaptations:
//// Copyright (c) The Byte Kitchen. All rights reserved


#pragma once

#include "pch.h"

namespace TheByteKitchen
{
	namespace DX
	{
		struct MVPConstantBuffer
		{
			DirectX::XMMATRIX model;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX projection;

			void* operator new (size_t size) 
			{ 
				void* p = _aligned_malloc(size, 16);
				if (!p)
					throw std::bad_alloc();
				return p; 
			}
			void operator delete (void *p) { _aligned_free(static_cast<MVPConstantBuffer*>(p)); }
		};

	}
}