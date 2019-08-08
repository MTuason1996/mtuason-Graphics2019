// This file can be used to contain very basic DX11 Loading, Drawing & Clean Up. (Start Here, or Make your own set of classes)
#pragma once
// Include our DX11 middle ware 
#include "../../Gateware/Interface/G_Graphics/GDirectX11Surface.h"
// Other possible Gateware libraries:
// GMatrix, GAudio, GController, GInput, GLog, GFile... +more
// While these libraries won't directly add any points to your project they can be used to make them more "gamelike"
// To add a library just include the relevant "Interface" & add any related *.cpp files from the "Source" folder to the Gateware filter->.
// Gateware comes with MSDN style docs too: "Gateware\Documentation\html\index.html"

// Include DirectX11 for interface access
#include <d3d11.h>
#include <DirectXMath.h>
#include "StoneHenge.h"
#include "StoneHenge_Texture.h"

#include "VertexShader.csh"
#include "PixelShader.csh"

using namespace DirectX;

// Simple Container class to make life easier/cleaner
class LetsDrawSomeStuff
{
	// variables here
	GW::GRAPHICS::GDirectX11Surface* mySurface = nullptr;
	// Gettting these handles from GDirectX11Surface will increase their internal refrence counts, be sure to "Release()" them when done!
	ID3D11Device *myDevice = nullptr;
	IDXGISwapChain *mySwapChain = nullptr;
	ID3D11DeviceContext *myContext = nullptr;

	// TODO: Add your own D3D11 variables here (be sure to "Release()" them when done!)
	struct Vec2D
	{
		float x, y;
	};
	struct Vec4D
	{
		float x, y, z, w;
	};

	struct Vertex
	{
		Vec4D pos;
		Vec4D color;
		Vec4D normal;
		Vec2D uv;
	};

	ID3D11Buffer* vBuffer = nullptr;
	ID3D11Buffer* iBuffer = nullptr;
	ID3D11Buffer* cBuffer = nullptr;
	ID3D11InputLayout* vLayout = nullptr;
	ID3D11VertexShader* vShader = nullptr;
	ID3D11PixelShader* pShader = nullptr; //hlsl
	D3D11_VIEWPORT myPort;

	// Math
	XMFLOAT4X4 wMatrix;
	HRESULT hr;

public:
	// Init
	LetsDrawSomeStuff(GW::SYSTEM::GWindow* attatchPoint);
	// Shutdown
	~LetsDrawSomeStuff();
	// Draw
	void Render();
};

// Init
LetsDrawSomeStuff::LetsDrawSomeStuff(GW::SYSTEM::GWindow* attatchPoint)
{
	if (attatchPoint) // valid window?
	{
		// Create surface, will auto attatch to GWindow
		if (G_SUCCESS(GW::GRAPHICS::CreateGDirectX11Surface(attatchPoint, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT, &mySurface)))
		{
			// Grab handles to all DX11 base interfaces
			mySurface->GetDevice((void**)&myDevice);
			mySurface->GetSwapchain((void**)&mySwapChain);
			mySurface->GetContext((void**)&myContext);

			// TODO: Create new DirectX stuff here! (Buffers, Shaders, Layouts, Views, Textures, etc...)

			//load onto card
			D3D11_BUFFER_DESC bDesc = {};
			D3D11_SUBRESOURCE_DATA subData = {};
			//ZeroMemory(&bDesc, sizeof(bDesc));
			//ZeroMemory(&subData, sizeof(subData));

			//VertexBuffer
			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.ByteWidth = sizeof(StoneHenge_data);
			bDesc.CPUAccessFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_IMMUTABLE;

			subData.pSysMem = StoneHenge_data;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &vBuffer);

			//IndexBuffer
			 bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			 bDesc.ByteWidth = sizeof(StoneHenge_indicies);
			 bDesc.CPUAccessFlags = 0;
			 bDesc.StructureByteStride = 0;
			 bDesc.Usage = D3D11_USAGE_IMMUTABLE;

			subData.pSysMem = StoneHenge_indicies;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &iBuffer);

			//write, compile and load shaders
			 hr = myDevice->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &vShader);
			 hr = myDevice->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &pShader);

			//describe to d3d11
			 D3D11_INPUT_ELEMENT_DESC ieDesc[] = 
			 {
				 {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				 {"TEXTURE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				 {"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			 };
			hr = myDevice->CreateInputLayout(ieDesc, 3, VertexShader, sizeof(VertexShader), &vLayout);

			bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bDesc.ByteWidth = sizeof(XMFLOAT4X4);
			bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DYNAMIC;

			hr = myDevice->CreateBuffer(&bDesc, nullptr, &cBuffer);

		}
	}
}

// Shutdown
LetsDrawSomeStuff::~LetsDrawSomeStuff()
{
	// Release DX Objects aquired from the surface
	myDevice->Release();
	mySwapChain->Release();
	myContext->Release();

	// TODO: "Release()" more stuff here!
	if (vBuffer) vBuffer->Release();
	if (iBuffer) iBuffer->Release();
	if (vLayout) vLayout->Release();
	if (vShader) vShader->Release();
	if (pShader) pShader->Release();



	if (mySurface) // Free Gateware Interface
	{
		mySurface->DecrementCount(); // reduce internal count (will auto delete on Zero)
		mySurface = nullptr; // the safest way to fly
	}
}

// Draw
void LetsDrawSomeStuff::Render()
{
	if (mySurface) // valid?
	{
		// this could be changed during resolution edits, get it every frame
		ID3D11RenderTargetView *myRenderTargetView = nullptr;
		ID3D11DepthStencilView *myDepthStencilView = nullptr;
		if (G_SUCCESS(mySurface->GetRenderTarget((void**)&myRenderTargetView)))
		{
			// Grab the Z Buffer if one was requested
			if (G_SUCCESS(mySurface->GetDepthStencilView((void**)&myDepthStencilView)))
			{
				myContext->ClearDepthStencilView(myDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0); // clear it to Z exponential Far.
				myDepthStencilView->Release();
			}

			// Set active target for drawing, all array based D3D11 functions should use a syntax similar to below
			ID3D11RenderTargetView* const targets[] = { myRenderTargetView };
			myContext->OMSetRenderTargets(1, targets, myDepthStencilView);

			// Clear the screen to green
			const float d_green[] = { 0.498f, 0.729f, 0, 1 }; // "DirectX Green"
			myContext->ClearRenderTargetView(myRenderTargetView, d_green);
			
			// TODO: Set your shaders, Update & Set your constant buffers, Attach your vertex & index buffers, Set your InputLayout & Topology & Draw!
			//Rasterizer
			//myContext->RSSetViewports(1, &myPort);
			//Input Assembler
			myContext->IASetInputLayout(vLayout);

			UINT strides[] = { sizeof(Vertex) };
			UINT offsets[] = { 0 };
			myContext->IASetVertexBuffers(0, 1, &vBuffer, strides, offsets);
			myContext->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R16_UINT, 0);
			myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			
			//Vertex Shader Stage
			myContext->VSSetShader(vShader, 0, 0);
			//Pixel Shader Stage
			myContext->PSSetShader(pShader, 0, 0);

			// Draw
			myContext->Draw(ARRAYSIZE(StoneHenge_data), 0);

			XMMATRIX temp = XMMatrixIdentity();
			temp = XMMatrixTranslation(0, 0, 3);
			XMStoreFloat4x4(&wMatrix, temp);

			D3D11_MAPPED_SUBRESOURCE gpuBuffer;
			myContext->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			gpuBuffer.pData = 

			// Present Backbuffer using Swapchain object
			// Framerate is currently unlocked, we suggest "MSI Afterburner" to track your current FPS and memory usage.
			mySwapChain->Present(0, 0); // set first argument to 1 to enable vertical refresh sync with display

			// Free any temp DX handles aquired this frame
			myRenderTargetView->Release();
		}
	}
}