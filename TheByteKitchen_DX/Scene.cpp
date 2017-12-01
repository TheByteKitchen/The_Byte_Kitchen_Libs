//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) The Byte Kitchen. All rights reserved
//// Some parts: Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "Scene.h"

using namespace TheByteKitchen::DX;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Media;

Scene::Scene() : ModelViewProjection(), m_timer() { }

Scene::~Scene() { }

void* Scene::operator new (size_t size) 
{ 
	void* p = _aligned_malloc(size, 16);
	if (!p)
		throw std::bad_alloc();
	return p; 
}

void Scene::operator delete (void *p)
{ _aligned_free(static_cast<Scene*>(p)); }


void Scene::Run()
{
	for (; ;)
	{
		RunOneFrame();
	}
}

void Scene::RunOneFrame()
{
	m_timer.Update();

	SceneTime st = { m_timer.TotalTime(), m_timer.DeltaTime() };

	Update(st);

	Render(st);

	Present();
}

void Scene::RegisterComponent(IComponent* comp)
{
	m_components.push_back(comp);
}

void Scene::ClearComponents()
{
	m_components.clear();
}

void Scene::Initialize() // Components ASYNC!!!
{
	m_swapChain = GraphicsDeviceManager::Current()->SwapChain();

	// Model: default sensible value;
	ModelViewProjection.model = XMMatrixIdentity();

	// View
	XMVECTOR eye = XMVectorSet(0.0f, 0.0f, 2.5f, 0.0f);
	XMVECTOR at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	ModelViewProjection.view = XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up));

	// Projection
	Rect windowBounds = GraphicsDeviceManager::Current()->WindowBounds();
	AspectRatio = windowBounds.Width / windowBounds.Height;
	NearPlane = 0.01f;
	FarPlane = 100.0f;

	float fovAngleY = 70.0f * XM_PI / 180.0f;
	if (AspectRatio < 1.0f)
		fovAngleY /= AspectRatio;

	ModelViewProjection.projection = XMMatrixTranspose(
		XMMatrixPerspectiveFovRH( fovAngleY, AspectRatio, NearPlane, FarPlane )
		);

	// Init components
	concurrency::parallel_for_each(begin(m_components), end(m_components), [](IComponent* comp) {
		comp->Initialize();
	});

	m_timer.Start();
}

void Scene::LoadContent() //Components ASYNC!!!
{
	concurrency::parallel_for_each(begin(m_components), end(m_components), [](IComponent* comp) {
		comp->LoadContent();
	});
}

void Scene::Update(SceneTime time) // Components in specified order
{
	std::for_each(begin(m_components), end(m_components), [&time](IComponent* comp) {
		comp->Update(time);
	});
}

void Scene::Render(SceneTime time) // Components in specified order
{
	std::for_each(begin(m_components), end(m_components), [&time](IComponent* comp) {
		comp->Render(time);
	});
}

void Scene::Present()
{
	// The application may optionally specify "dirty" or "scroll" rects to improve efficiency
	// in certain scenarios.  In this sample, however, we do not utilize those features.
	DXGI_PRESENT_PARAMETERS parameters = {0};
	parameters.DirtyRectsCount = 0;
	parameters.pDirtyRects = nullptr;
	parameters.pScrollRect = nullptr;
	parameters.pScrollOffset = nullptr;

	// The first argument instructs DXGI to block until VSync, putting the application
	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
	// frames that will never be displayed to the screen.
	HRESULT hr = m_swapChain->Present1(1, 0, &parameters);

	// If the device was removed either by a disconnect or a driver upgrade, we 
	// must completely reinitialize the renderer.
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		//Initialize(m_window.Get(), m_swapChainPanel, m_dpi);
	}
	else
	{
		ThrowIfFailed(hr);
	}
}

