//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) The Byte Kitchen. All rights reserved

//
// pch.h
// Header for standard system include files.
//

#define _ITERATOR_DEBUG_LEVEL 0

// Linkage specification
#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "Dwrite.lib")
#pragma comment (lib, "windowscodecs.lib")

#pragma once

// STL
#include <string>
#include <sstream>

// Windows 8
#include "windows.ui.xaml.media.dxinterop.h"
#include <agile.h>
#include <wincodec.h>

// DirectX
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <dwrite_2.h>
#include <DirectXMath.h>

//Internal
#include "Inlines.inl"

// Concurrency
#include <ppltasks.h>