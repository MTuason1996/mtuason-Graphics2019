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
	struct Vertex
	{
		XMFLOAT4 pos;
		XMFLOAT2 uv;
		XMFLOAT4 normal;
		XMFLOAT4 color;
	};

	ID3D11Buffer* vBuffer = nullptr;
	ID3D11Buffer* iBuffer = nullptr;
	ID3D11Buffer* cBuffer = nullptr;
	ID3D11InputLayout* vLayout = nullptr;
	ID3D11VertexShader* vShader = nullptr;
	ID3D11PixelShader* pShader = nullptr; //hlsl

	// Math
	struct WVP
	{
	XMFLOAT4X4 wMatrix;
	XMFLOAT4X4 vMatrix;
	XMFLOAT4X4 pMatrix;
	}myMatrices;

	float aspectR = 1.0f;

	Vertex * vertices;
	int numVertices = 0;
	int * indices = nullptr;
	int numIndices = 0;
	float scale = 5.0f;

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
			ZeroMemory(&bDesc, sizeof(bDesc));
			ZeroMemory(&subData, sizeof(subData));

			Vertex pyramid[12] = 
			{
				// pos							uv		normal			color
				//front
				{ {0, 1.0f, 0, 1},				{0, 0},	{0, 0, 0, 1},	{1, 0, 0, 1} },
				{ {0.25f, -0.25f, -0.25f, 1},	{0, 0},	{0, 0, 0, 1},	{0, 1, 0, 1} },
				{ {-0.25f, -0.25f, -0.25f, 1},	{0, 0},	{0, 0, 0, 1},	{0, 0, 1, 1} },

				//right
				{ {0, 1.0f, 0, 1},				{0, 0},	{0, 0, 0, 1},	{1, 0, 0, 1} },
				{ {0.25f, -0.25f, 0.25f, 1},	{0, 0},	{0, 0, 0, 1},	{0, 1, 0, 1} },
				{ {0.25f, -0.25f, -0.25f, 1},	{0, 0},	{0, 0, 0, 1},	{0, 0, 1, 1} },

				//back
				{ {0, 1.0f, 0, 1},				{0, 0},	{0, 0, 0, 1},	{1, 0, 0, 1} },
				{ {-0.25f, -0.25f, 0.25f, 1},	{0, 0},	{0, 0, 0, 1},	{0, 1, 0, 1} },
				{ {0.25f, -0.25f, 0.25f, 1},	{0, 0},	{0, 0, 0, 1},	{0, 0, 1, 1} },

				//left
				{ {0, 1.0f, 0, 1},				{0, 0},	{0, 0, 0, 1},	{1, 0, 0, 1} },
				{ {-0.25f, -0.25f, -0.25f, 1},	{0, 0},	{0, 0, 0, 1},	{0, 1, 0, 1} },
				{ {-0.25f, -0.25f, 0.25f, 1},	{0, 0},	{0, 0, 0, 1},	{0, 0, 1, 1} },

			};

			//VertexBuffer
			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.ByteWidth = sizeof(pyramid);
			bDesc.CPUAccessFlags = 0;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = pyramid;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &vBuffer);

			////IndexBuffer
			// bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			// bDesc.ByteWidth = sizeof(StoneHenge_indicies);
			// bDesc.CPUAccessFlags = 0;
			// bDesc.StructureByteStride = 0;
			// bDesc.Usage = D3D11_USAGE_IMMUTABLE;

			//subData.pSysMem = StoneHenge_indicies;

			//hr = myDevice->CreateBuffer(&bDesc, &subData, &iBuffer);

			//write, compile and load shaders
			 hr = myDevice->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &vShader);
			 hr = myDevice->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &pShader);

			//describe to d3d11
			 D3D11_INPUT_ELEMENT_DESC ieDesc[] = 
			 {
				 {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				 {"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				 {"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				 {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			 };
			hr = myDevice->CreateInputLayout(ieDesc, 4, VertexShader, sizeof(VertexShader), &vLayout);

			//constant buffer
			bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bDesc.ByteWidth = sizeof(WVP);
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
	if (cBuffer) cBuffer->Release();



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
			const float color[] = { 0, 1, 0.5f, 1 };
			myContext->ClearRenderTargetView(myRenderTargetView, color);

			// TODO: Set your shaders, Update & Set your constant buffers, Attach your vertex & index buffers, Set your InputLayout & Topology & Draw!
			//Input Assembler
			myContext->IASetInputLayout(vLayout);

			UINT strides[] = { sizeof(Vertex) };
			UINT offsets[] = { 0 };
			ID3D11Buffer* tempVB[] = { vBuffer };
			myContext->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
			//myContext->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R16_UINT, 0);
			myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//Vertex Shader Stage
			myContext->VSSetShader(vShader, 0, 0);
			//Pixel Shader Stage
			myContext->PSSetShader(pShader, 0, 0);

			// Draw
			myContext->Draw(12, 0);

			//World Matrix
			static float rot = 0; rot += 0.01f;
			XMMATRIX temp = XMMatrixIdentity();
			temp = XMMatrixMultiply(temp, XMMatrixRotationY(rot));
			XMStoreFloat4x4(&myMatrices.wMatrix, temp);

			//View Matrix
			temp = XMMatrixRotationX(XMConvertToRadians(-18));
			temp = XMMatrixMultiply(temp, XMMatrixTranslation(0, 0, -5));
			temp = XMMatrixInverse(nullptr, temp);
			//temp = XMMatrixLookAtLH({ 2,1,-3 }, { 0,0,0 }, { 0,1,0 });
			XMStoreFloat4x4(&myMatrices.vMatrix, temp);

			//Projection Matrix
			temp = XMMatrixPerspectiveFovLH(3.14f / 2.0f, aspectR, 0.1f, 1000.0f);
			XMStoreFloat4x4(&myMatrices.pMatrix, temp);


			D3D11_MAPPED_SUBRESOURCE gpuBuffer;
			myContext->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((WVP*)(gpuBuffer.pData)) = myMatrices;

			myContext->Unmap(cBuffer, 0);

			//connect constant buffer to pipeline
			ID3D11Buffer * constants[] = { cBuffer };
			myContext->VSSetConstantBuffers(0, 1, constants);

			// Present Backbuffer using Swapchain object
			// Framerate is currently unlocked, we suggest "MSI Afterburner" to track your current FPS and memory usage.
			mySwapChain->Present(1, 0); // set first argument to 1 to enable vertical refresh sync with display

			// Free any temp DX handles aquired this frame
			myRenderTargetView->Release();
		}
	}
}