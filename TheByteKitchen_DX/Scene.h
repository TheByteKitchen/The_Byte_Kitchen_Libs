//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) The Byte Kitchen. All rights reserved

#pragma once

#include "pch.h"
#include "RunTimer.h"
#include "IComponent.h"
#include "GraphicsDeviceManager.h"
#include "ConstantBufferFormats.h"

using namespace Microsoft::WRL;
using namespace Platform;
using namespace std;
using namespace Windows::UI::Core;

namespace TheByteKitchen
{
	namespace DX
	{
		class Scene abstract
		{
		public:
			void* operator new (size_t size);
			void operator delete (void *p);

			// virtual methods
			virtual void Run();
			virtual void Initialize();
			virtual void LoadContent();
			virtual void Update(SceneTime time);
			virtual void Render(SceneTime time); 

			// public methods
			void RunOneFrame();

		protected:
			MVPConstantBuffer								ModelViewProjection;
			float											AspectRatio;
			float											NearPlane;
			float											FarPlane;
			string											AverageTime;

			// ctors, dtors
			Scene(void);
			virtual ~Scene(void);

			void RegisterComponent(IComponent* comp);
			void Scene::ClearComponents();

		private:
			// private state
			RunTimer					m_timer;
			ComPtr<IDXGISwapChain1>		m_swapChain;
			std::vector<IComponent*>	m_components;

			void Present();
		};
	}
}