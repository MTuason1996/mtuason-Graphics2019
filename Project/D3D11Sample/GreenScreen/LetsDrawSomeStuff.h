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
#include "XTime.h"

// DDSTextureLoading
#include "DDSTextureLoader.h"

// models and textures
#include "Assets/Spyro/ArtisansHub.h"

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

	ID3D11Buffer*				vBuffer = nullptr;
	ID3D11Buffer*				iBuffer = nullptr;
	ID3D11Buffer*				cBuffer = nullptr;
	ID3D11Buffer*				lightBuffer = nullptr;
	ID3D11InputLayout*			vLayout = nullptr;
	ID3D11VertexShader*			vShader = nullptr;
	ID3D11PixelShader*			pShader = nullptr; //hlsl

	//Textures
	ID3D11ShaderResourceView*	textureBox = nullptr;
	ID3D11SamplerState*			samplerLin = nullptr;

	// Math
	struct WVP
	{
	XMFLOAT4X4 wMatrix;
	XMFLOAT4X4 vMatrix;
	XMFLOAT4X4 pMatrix;
	}myMatrices;

	// Lights
	struct Lights
	{
		XMFLOAT4 dLight[3];			//directional light
		XMFLOAT4 pLight[4];			//point light
		XMFLOAT4 sLight[4];			//spot light
	}myLights;

	Vertex* artisansHub = new Vertex[ARRAYSIZE(ArtisansHub_data)];
	XTime timer;



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
			timer.Restart();
			mySurface->GetAspectRatio(aspectR);

			//load onto card
			D3D11_BUFFER_DESC bDesc = {};
			D3D11_SUBRESOURCE_DATA subData = {};
			ZeroMemory(&bDesc, sizeof(bDesc));
			ZeroMemory(&subData, sizeof(subData));

			for (int i = 0; i < ARRAYSIZE(ArtisansHub_data); ++i)
			{
				artisansHub[i].pos.x = ArtisansHub_data[i].pos[0] * 0.1f;
				artisansHub[i].pos.y = ArtisansHub_data[i].pos[1] * 0.1f;
				artisansHub[i].pos.z = ArtisansHub_data[i].pos[2] * 0.1f;
				artisansHub[i].pos.w = 1.0f;

				artisansHub[i].uv.x = ArtisansHub_data[i].uvw[0];
				artisansHub[i].uv.y = ArtisansHub_data[i].uvw[1];

				artisansHub[i].normal.x = ArtisansHub_data[i].nrm[0];
				artisansHub[i].normal.y = ArtisansHub_data[i].nrm[1];
				artisansHub[i].normal.z = ArtisansHub_data[i].nrm[2];
				artisansHub[i].normal.w = 0.0f;

				artisansHub[i].color = { 0,0,0,1 };

			}


			//VertexBuffer
			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(ArtisansHub_data);
			bDesc.CPUAccessFlags = 0;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = artisansHub;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &vBuffer);

			numVertices = ARRAYSIZE(ArtisansHub_data);

			//IndexBuffer
			 bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			 bDesc.ByteWidth = sizeof(ArtisansHub_indicies);
			 bDesc.CPUAccessFlags = 0;
			 bDesc.StructureByteStride = 0;
			 bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = ArtisansHub_indicies;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &iBuffer);
			numIndices = ARRAYSIZE(ArtisansHub_indicies);

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

			bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bDesc.ByteWidth = sizeof(Lights);
			bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DYNAMIC;

			hr = myDevice->CreateBuffer(&bDesc, nullptr, &lightBuffer);


			hr = CreateDDSTextureFromFile(myDevice, L"Assets/Spyro/HubTextures.dds", nullptr, &textureBox);

			// Create sample state
			D3D11_SAMPLER_DESC sampDesc = {};
			sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
			hr = myDevice->CreateSamplerState(&sampDesc, &samplerLin);


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
	if (lightBuffer) lightBuffer->Release();
	if (samplerLin) samplerLin->Release();

	// release shader resource
	if (textureBox) textureBox->Release();

	delete artisansHub;



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
			timer.Signal();
			// Grab the Z Buffer if one was requested
			if (G_SUCCESS(mySurface->GetDepthStencilView((void**)&myDepthStencilView)))
			{
				myContext->ClearDepthStencilView(myDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0); // clear it to Z exponential Far.
				myDepthStencilView->Release();
			}

			// Set active target for drawing, all array based D3D11 functions should use a syntax similar to below
			ID3D11RenderTargetView* const targets[] = { myRenderTargetView };
			myContext->OMSetRenderTargets(1, targets, myDepthStencilView);

			// Clear the screen
			const float color[] = { 0, 0, 0.2f, 1 };
			myContext->ClearRenderTargetView(myRenderTargetView, color);

			// TODO: Set your shaders, Update & Set your constant buffers, Attach your vertex & index buffers, Set your InputLayout & Topology & Draw!
			//Input Assembler
			myContext->IASetInputLayout(vLayout);

			UINT strides[] = { sizeof(Vertex) };
			UINT offsets[] = { 0 };
			ID3D11Buffer* tempVB[] = { vBuffer };
			myContext->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
			myContext->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			//Vertex Shader Stage
			myContext->VSSetShader(vShader, 0, 0);
			//Pixel Shader Stage
			myContext->PSSetShader(pShader, 0, 0);

			// Draw
			myContext->DrawIndexed(numIndices, 0, 0);

			//-----------------------------------------------------------------------
			// World Matrix
			//-----------------------------------------------------------------------
			XMMATRIX temp = XMMatrixIdentity();
			XMStoreFloat4x4(&myMatrices.wMatrix, temp);

			//-----------------------------------------------------------------------
			// View Matrix + camera controls
			//-----------------------------------------------------------------------
			temp = XMMatrixRotationX(XMConvertToRadians(25));
			temp = XMMatrixMultiply(temp, XMMatrixTranslation(0, 10, -20));
			XMMATRIX worldView = temp;

			//Camera controls
			static float xAxisT = 0;
			static float xAxisR = 0;
			static float yAxisR = 0;
			static float yAxisT = 0;
			
			static float zAxisT = 0;

			//Translate
			float tSpeed = 5 * timer.SmoothDelta();
			if (GetAsyncKeyState('W'))
				zAxisT += tSpeed;
			if (GetAsyncKeyState('S'))
				zAxisT -= tSpeed;
			if (GetAsyncKeyState('A'))
				xAxisT -= tSpeed;
			if (GetAsyncKeyState('D'))
				xAxisT += tSpeed;
			if (GetAsyncKeyState(VK_SPACE) && GetAsyncKeyState(VK_LSHIFT))
				yAxisT -= tSpeed;
			else if (GetAsyncKeyState(VK_SPACE))
				yAxisT += tSpeed;

			//Rotate
			float rSpeed = 30 * timer.SmoothDelta();
			if (GetAsyncKeyState('K'))
				xAxisR += rSpeed;
			if (GetAsyncKeyState('I'))
				xAxisR -= rSpeed;
			if (GetAsyncKeyState('J'))
				yAxisR -= rSpeed;
			if (GetAsyncKeyState('L'))
				yAxisR += rSpeed;

			//reset position and rotation
			if (GetAsyncKeyState('R'))
			{
				xAxisT = 0;
				yAxisT = 0;
				zAxisT = 0;
				xAxisR = 0;
				yAxisR = 0;
			}
			//rotation
			temp = XMMatrixMultiply(XMMatrixRotationX(XMConvertToRadians(xAxisR)), temp);
			temp = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(yAxisR)), temp);

			// Call on rotation
			//XMVECTOR newX = { 0 };
			//XMVECTOR newY = { 0 };
			//XMVector4Cross({ 0,1,0,0 }, temp.r[2], newX);
			//XMVector4Cross(temp.r[2], newX, newY);

			//temp.r[0] = XMVector4Normalize(newX);
			//temp.r[1] = XMVector4Normalize(newY);
			//temp.r[2] = XMVector4Normalize(temp.r[2]);


			//translation
			temp = XMMatrixMultiply(XMMatrixTranslation(xAxisT, 0, zAxisT), temp);
			temp = XMMatrixMultiply(temp, XMMatrixTranslation(0, yAxisT, 0));

			temp = XMMatrixInverse(nullptr, temp);
			XMStoreFloat4x4(&myMatrices.vMatrix, temp);

			//-----------------------------------------------------------------------
			// Projection Matrix
			//-----------------------------------------------------------------------
			static float fov = 90;
			if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_LSHIFT))
			{
				if (fov < 90.0f)
					fov += 0.1f;
			}
			else if (GetAsyncKeyState(VK_CONTROL))
			{
				if (fov > 30.0f)
					fov -= 0.1f;
			}
			temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), aspectR, 0.1f, 1000.0f);
			XMStoreFloat4x4(&myMatrices.pMatrix, temp);

			// Attach matrices to cBuffer
			D3D11_MAPPED_SUBRESOURCE gpuBuffer;
			myContext->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((WVP*)(gpuBuffer.pData)) = myMatrices;

			myContext->Unmap(cBuffer, 0);

			//-----------------------------------------------------------------------
			// Directional Light
			//-----------------------------------------------------------------------
			XMFLOAT4 dColor = { 0.32f, 0.942f, 0.762f,1 };
			static bool dToggle = false;
			myLights.dLight[0] = { 0, 0, 0, 1.0f };
			myLights.dLight[1] = { 0.577f, 0.577f, -0.577f, 0.0f };
			//toggle dLight
			if (dToggle)
				myLights.dLight[2] = dColor;
			else
				myLights.dLight[2] = { 0,0,0,0 };

			if (GetAsyncKeyState('1') & 1)
				dToggle = !dToggle;

			XMVECTOR temp2 = { myLights.dLight[1].x, myLights.dLight[1].y, myLights.dLight[1].z, myLights.dLight[1].w, };
			//Rotate directional light
			static float rotD = 0.0f; rotD += 0.5f;
			temp2 = XMVector4Transform(temp2, XMMatrixRotationZ(XMConvertToRadians(rotD)));
			// normalize
			temp2 = XMVector4Normalize(temp2);
			XMStoreFloat4(&myLights.dLight[1], temp2);

			//-----------------------------------------------------------------------
			// Point Light
			//-----------------------------------------------------------------------
			XMFLOAT4 pColor = { 1.0f, 0, 0, 1 };
			static bool pToggle = false;
			myLights.pLight[0] = { 0.0f, 0.5f, 0.0f, 1.0f };
			myLights.pLight[1] = { 0.0f, 0.0f, 0.0f, 0.0f };
			//toggle pLight
			if (pToggle)
				myLights.pLight[2] = pColor;
			else
				myLights.pLight[2] = { 0,0,0,0 };

			if (GetAsyncKeyState('2') & 1)
				pToggle = !pToggle;
		
			// point light radius
			myLights.pLight[3].x = 5.0f;
			//translate point light
			static float transP = 0; 
			static bool moveUp = true;
			if (moveUp)
			{
				if (transP < 12.0f)
					transP += 0.1f;
				else
					moveUp = false;
			}
			else
			{
				if (transP > -12.0f)
					transP -= 0.1f;
				else
					moveUp = true;
			}
			temp2 = { myLights.pLight[0].x, myLights.pLight[0].y, myLights.pLight[0].z, myLights.pLight[0].w, };
			temp2 = XMVector4Transform(temp2, XMMatrixTranslation(0, 0, transP));
			XMStoreFloat4(&myLights.pLight[0], temp2);

			//-----------------------------------------------------------------------
			// Spot Light
			//-----------------------------------------------------------------------
			XMFLOAT4 sColor = { 0.5f, 0.5f, 0, 1 };
			static bool sToggle = true;
			// position
			myLights.sLight[0] = { 1.0f, 2.0f, -4.0f, 1.0f };
			// direction
			myLights.sLight[1] = {-0.577f, -0.577f, 0.577f, 0.0f};
			// toggle sLight
			if (sToggle)
				myLights.sLight[2] = sColor;
			else
				myLights.sLight[2] = { 0.0f, 0.0f, 0.0f, 0.0f };

			if (GetAsyncKeyState('3') & 1)
				sToggle = !sToggle;

			// innerConeRatio, outerConeRatio
			myLights.sLight[3] = { 0.97f, 0.82f, 0, 0 };

			temp2 = { myLights.sLight[1].x, myLights.sLight[1].y, myLights.sLight[1].z, myLights.sLight[1].w, };
			// rotate direction of spotlight
			static float rotS = 0.0f; rotS += 0.5f;
			temp2 = XMVector4Transform(temp2, XMMatrixRotationY(XMConvertToRadians(rotS)));
			// normalize direction
			temp2 = XMVector4Normalize(temp2);
			XMStoreFloat4(&myLights.sLight[1], temp2);
			static float transS = 0;
			static bool moveRight = true;
			if (moveRight)
			{
				if (transS < 12.0f)
					transS += 0.1f;
				else
					moveRight = false;
			}
			else
			{
				if (transS > -12.0f)
					transS -= 0.1f;
				else
					moveRight = true;
			}
			temp2 = { myLights.sLight[0].x, myLights.sLight[0].y, myLights.sLight[0].z, myLights.sLight[0].w, };
			temp2 = XMVector4Transform(temp2, XMMatrixTranslation(transS, 0, 0));
			XMStoreFloat4(&myLights.sLight[0], temp2);


			//-----------------------------------------------------------------------
			// Constant Buffer connections
			//-----------------------------------------------------------------------

			// Attach light to lightBuffer
			myContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((Lights*)(gpuBuffer.pData)) = myLights;

			myContext->Unmap(lightBuffer, 0);

			//connect constant buffers to pipeline
			ID3D11Buffer * constants[] = { cBuffer, lightBuffer };
			myContext->VSSetConstantBuffers(0, 2, constants);

			//dds
			myContext->PSSetShaderResources(0, 1, &textureBox);
			myContext->PSSetSamplers(0, 1, &samplerLin);

			// Present Backbuffer using Swapchain object
			// Framerate is currently unlocked, we suggest "MSI Afterburner" to track your current FPS and memory usage.
			mySwapChain->Present(1, 0); // set first argument to 1 to enable vertical refresh sync with display

			// Free any temp DX handles aquired this frame
			myRenderTargetView->Release();
		}
	}
}