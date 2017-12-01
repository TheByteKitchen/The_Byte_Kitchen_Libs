//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) The Byte Kitchen. All rights reserved

#pragma once

#include "SceneTime.h"

namespace TheByteKitchen
{
	namespace DX
	{
		class IComponent
		{
		public:
			virtual void Initialize()=0;
			virtual void LoadContent()=0;
			virtual void Update( SceneTime sceneTime )=0;
			virtual void Render( SceneTime sceneTime )=0;
		};
	}
}
