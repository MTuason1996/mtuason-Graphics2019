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

#include "FBXLoader.h"
#include "XTime.h"

// DDSTextureLoading
#include "DDSTextureLoader.h"

// models and textures

#include "VertexShader.csh"
#include "PixelShader.csh"
#include "PS_BWshader.csh"
#include "VS_sinShader.csh"
#include "PS_Cloudshader.csh"
#include "PS_skyboxshader.csh"
#include "VS_skyboxShader.csh"
#include "PS_ReflectionShader.csh"



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
	};

	ID3D11Buffer*				worldVBuffer = nullptr;
	ID3D11Buffer*				worldIBuffer = nullptr;
	ID3D11Buffer*				dragonVBuffer = nullptr;
	ID3D11Buffer*				dragonIBuffer = nullptr;
	ID3D11Buffer*				wizardVBuffer = nullptr;
	ID3D11Buffer*				wizardIBuffer = nullptr;
	ID3D11Buffer*				dwarfVBuffer = nullptr;
	ID3D11Buffer*				dwarfIBuffer = nullptr;
	ID3D11Buffer*				statueVBuffer = nullptr;
	ID3D11Buffer*				statueIBuffer = nullptr;
	ID3D11Buffer*				gridVBuffer = nullptr;
	ID3D11Buffer*				gridIBuffer = nullptr;
	ID3D11Buffer*				cBuffer = nullptr;
	ID3D11Buffer*				lightBuffer = nullptr;
	ID3D11Buffer*				timeBuffer = nullptr;
	ID3D11InputLayout*			vLayout = nullptr;

	//Vertex Shaders
	ID3D11VertexShader*			vShader = nullptr;
	ID3D11VertexShader*			sinVShader = nullptr;
	ID3D11VertexShader*			skybox_VShader = nullptr;

	//Pixel Shaders
	ID3D11PixelShader*			pShader = nullptr;
	ID3D11PixelShader*			bw_PShader = nullptr;
	ID3D11PixelShader*			cloud_PShader = nullptr;
	ID3D11PixelShader*			skybox_PShader = nullptr;
	ID3D11PixelShader*			reflect_PShader = nullptr;

	// skyBox
	ID3D11Buffer*				skyVBuffer = nullptr;
	ID3D11Buffer*				skyIBuffer = nullptr;
	ID3D11ShaderResourceView*	skyTexture = nullptr;

	//Textures
	ID3D11ShaderResourceView*	worldTex = nullptr;
	ID3D11ShaderResourceView*	dragonTex = nullptr;
	ID3D11ShaderResourceView*	wizardTex = nullptr;
	ID3D11ShaderResourceView*	dwarfTex = nullptr;
	ID3D11ShaderResourceView*	saberTex = nullptr;
	ID3D11SamplerState*			samplerLin = nullptr;

	// Math
	struct WVP
	{
	XMFLOAT4X4 wMatrix[6];
	XMFLOAT4X4 vMatrix;
	XMFLOAT4X4 pMatrix;
	XMFLOAT4X4 worldView;
	}myMatrices;


	// Lights
	struct Lights
	{
		XMFLOAT4 dLight[3];			//directional light
		XMFLOAT4 pLight[4];			//point light
		XMFLOAT4 sLight[4];			//spot light
		XMFLOAT4 specular[2];
	}myLights;

	struct Mesh
	{
		std::vector<Vertex>* verts = new std::vector<Vertex>();
		int numVertices = 0;
		int* indices = nullptr;
		int numIndices = 0;
		
	};

	int numMesh = 6;
	Mesh* meshes = new Mesh[numMesh];
	XMMATRIX worlds[5];
	XMMATRIX instancedWorlds[6];

	XTime timer;

	// Enum triggers
	enum Pixel
	{
		norm = 0,
		bw,
		cloudy
	};
	int numPS = 3;

	float aspectR = 1.0f;

	HRESULT hr;

	FbxManager* fbxSdkManager = nullptr;

	void LoadUVInformation(FbxMesh* pMesh, std::vector<FbxVector2>& uvVerts);

public:
	// Init
	LetsDrawSomeStuff(GW::SYSTEM::GWindow* attatchPoint);
	// Shutdown
	~LetsDrawSomeStuff();

	HRESULT LoadFBX(char* filePath, Mesh *outMesh, bool rotateModel, float scale);

	void Compactify(Mesh *outMesh);

	HRESULT GenerateGrid(float width, float height, int vertsWidth, int vertsHeight, Mesh *outMesh);

	// Draw
	void Render();
};

void LetsDrawSomeStuff::LoadUVInformation(FbxMesh* pMesh, std::vector<FbxVector2>& uvVerts)
{
	//get all UV set names
	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames(lUVSetNameList);

	//iterating over all uv sets
	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
	{
		//get lUVSetIndex-th uv set
		const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
		const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

		if (!lUVElement)
			continue;

		// only support mapping mode eByPolygonVertex and eByControlPoint
		if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
			return;

		//index array, where holds the index referenced to the uv data
		const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

		//iterating through the data by polygon
		const int lPolyCount = pMesh->GetPolygonCount();

		if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					FbxVector2 lUVValue;

					//get the index of the current vertex in control points array
					int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					uvVerts.push_back(lUVValue);
				}
			}
		}
		else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int lPolyIndexCounter = 0;
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					if (lPolyIndexCounter < lIndexCount)
					{
						FbxVector2 lUVValue;

						//the UV index depends on the reference mode
						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

						uvVerts.push_back(lUVValue);

						lPolyIndexCounter++;
					}
				}
			}
		}
	}
}

HRESULT LetsDrawSomeStuff::LoadFBX(char* filePath, Mesh *outMesh, bool rotateMesh, float scale)
{
	//init manager
	if (fbxSdkManager == nullptr)
	{
		fbxSdkManager = FbxManager::Create();

		//create settings for manager
		FbxIOSettings* IOSettings = FbxIOSettings::Create(fbxSdkManager, IOSROOT);
		fbxSdkManager->SetIOSettings(IOSettings);
	}

	FbxImporter* pImporter = FbxImporter::Create(fbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(fbxSdkManager, "");

	bool success = pImporter->Initialize(filePath, -1, fbxSdkManager->GetIOSettings());

	if (!success) return E_FAIL;

	success = pImporter->Import(pFbxScene);
	if (!success) return E_FAIL;

	pImporter->Destroy();

	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();

	int childCount = pFbxRootNode->GetChildCount();

	if (pFbxRootNode)
	{
		for (int i = 0; i < childCount; ++i)
		{
			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);

			if (pFbxChildNode->GetNodeAttribute() == nullptr)
				continue;

			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

			int numVerts = pMesh->GetControlPointsCount();
			int tempIndexCount = pMesh->GetPolygonVertexCount();
			outMesh->numIndices += tempIndexCount;
			outMesh->indices = pMesh->GetPolygonVertices();

			//Load position data
			std::vector<Vertex> tempVerts;

			for (int j = 0; j < numVerts; j++)
			{
				FbxVector4 pVertices = pMesh->GetControlPointAt(j);
				Vertex myVert;
				myVert.pos.x = (float)pVertices.mData[0] * scale;
				myVert.pos.y = (float)pVertices.mData[1] * scale;
				myVert.pos.z = (float)pVertices.mData[2] * scale;
				myVert.pos.w = 1.0f;

				myVert.uv = { 0,0 };

				myVert.normal = { 0,0,0,0 };

				tempVerts.push_back(myVert);
			}


			std::vector<FbxVector2> pUVs;
			LoadUVInformation(pMesh, pUVs);
			FbxArray<FbxVector4> pNorms;
			pMesh->GetPolygonVertexNormals(pNorms);

			Vertex * verts2 = new Vertex[outMesh->numIndices];

			//load remaining data
			for (int j = 0; j < tempIndexCount; ++j)
			{
				Vertex myVert;
				myVert.pos = tempVerts[outMesh->indices[j]].pos;
				myVert.normal.x = pNorms.GetAt(j)[0];
				myVert.normal.y = pNorms.GetAt(j)[1];
				myVert.normal.z = pNorms.GetAt(j)[2];

				XMVECTOR normal = { myVert.normal.x, myVert.normal.y, myVert.normal.z, 0.0f };
				normal = XMVector4Normalize(normal);

				if (rotateMesh)
				{
					XMVECTOR position = { myVert.pos.x, myVert.pos.y, myVert.pos.z, myVert.pos.w };
					XMMATRIX rotate = XMMatrixRotationX(XMConvertToRadians(-90));

					position = XMVector4Transform(position, rotate);
					normal = XMVector4Normalize(XMVector4Transform(normal, rotate));
					XMStoreFloat4(&myVert.pos, position);
					XMStoreFloat4(&myVert.normal, normal);
				}
				XMStoreFloat4(&myVert.normal, normal);

				myVert.uv = XMFLOAT2((float)pUVs[j].mData[0], 1.0f - (float)pUVs[j].mData[1]);

				outMesh->verts->push_back(myVert);
			}

			outMesh->numVertices = outMesh->numIndices;

			delete outMesh->indices;
			outMesh->indices = new int[outMesh->numIndices];
			for (int j = 0; j < outMesh->numIndices; j++)
			{
				outMesh->indices[j] = j;
			}
		}
	}
	//Compactify(outMesh);
	return S_OK;
}

void LetsDrawSomeStuff::Compactify(Mesh *outMesh)
{
	std::vector<Vertex>& vecRef = *outMesh->verts;
	std::vector<int> indicesList;
	std::vector<Vertex> vertexList;

	vertexList.push_back(vecRef[0]);
	vertexList.push_back(vecRef[1]);
	vertexList.push_back(vecRef[2]);

	indicesList.push_back(0);
	indicesList.push_back(1);
	indicesList.push_back(2);

	int add = 2;

	float epsilon = 0.0001f;

	bool copy = false;

	for (int i = add + 1; i < outMesh->numVertices; ++i)
	{
		copy = false;

		for (unsigned int j = 0; j < vertexList.size(); j++)
		{
			if (fabs(vertexList[j].pos.x - vecRef[i].pos.x) < epsilon &&
				fabs(vertexList[j].pos.y - vecRef[i].pos.y) < epsilon &&
				fabs(vertexList[j].pos.z - vecRef[i].pos.z) < epsilon &&
				fabs(vertexList[j].normal.x - vecRef[i].normal.x) < epsilon &&
				fabs(vertexList[j].normal.y - vecRef[i].normal.y) < epsilon &&
				fabs(vertexList[j].normal.z - vecRef[i].normal.z) < epsilon &&
				fabs(vertexList[j].uv.x - vecRef[i].uv.x) < epsilon &&
				fabs(vertexList[j].uv.y - vecRef[i].uv.y) < epsilon)
			{
				indicesList.push_back(j);
				copy = true;
				break;
			}
		}
		if (!copy)
		{
			indicesList.push_back(++add);
			vertexList.push_back(vecRef[i]);
		}
	}

	outMesh->numIndices = indicesList.size();
	outMesh->numVertices = vertexList.size();
	delete outMesh->verts;
	outMesh->verts = &vertexList;

	delete outMesh->indices;
	outMesh->indices = new int[outMesh->numIndices];

	for (int i = 0; i < outMesh->numIndices; ++i)
		outMesh->indices[i] = indicesList[i];
}

HRESULT LetsDrawSomeStuff::GenerateGrid(float width, float height, int vertsWidth, int vertsHeight, Mesh *outMesh)
{
	float widthDist = width / (float)vertsWidth;
	float widthStart = -(width / 2.0f);
	float texUDist = 1.0f / (float)vertsWidth;

	float heightDist = height / (float)vertsHeight;
	float heightStart = (height / 2.0f);
	float texVDist = 1.0f / (float)vertsHeight;

	// create vertices
	for (int i = 0; i < vertsHeight; ++i)
	{
		float zPos = heightStart - (heightDist * i);
		float texV = i * texVDist;
		for (int j = 0; j < vertsWidth; ++j)
		{
			float xPos = widthStart + (widthDist * j);
			float texU = j * texUDist;

			Vertex myVert;
			myVert.pos = { xPos, 0.0f, zPos, 1.0f };
			myVert.normal = { 0,1,0,0 };	//flat Plane
			myVert.uv = { texU, texV };

			outMesh->verts->push_back(myVert);
		}
	}
	
	std::vector<int> indexList;
	// create indices
	for (int i = 0; i < vertsHeight - 1; ++i)
	{
		for (int j = 0; j < vertsWidth - 1; ++j)
		{
			indexList.push_back(j + (vertsWidth * i));
			indexList.push_back(j + (vertsWidth * i) + 1);
			indexList.push_back(j + (vertsWidth * i) + 1 + vertsWidth);

			indexList.push_back(j + (vertsWidth * i) + 1 + vertsWidth);
			indexList.push_back(j + (vertsWidth * i) + vertsWidth);
			indexList.push_back(j + (vertsWidth * i));
		}
	}

	outMesh->numIndices = (int)indexList.size();
	outMesh->numVertices = vertsWidth * vertsHeight;

	outMesh->indices = new int[outMesh->numIndices];

	for (int i = 0; i < outMesh->numIndices; ++i)
		outMesh->indices[i] = indexList[i];

	return S_OK;
}

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

			//load onto card
			D3D11_BUFFER_DESC bDesc = {};
			D3D11_SUBRESOURCE_DATA subData = {};
			ZeroMemory(&bDesc, sizeof(bDesc));
			ZeroMemory(&subData, sizeof(subData));

			///////////////////////////////////////
			// Loading with FBX loader
			///////////////////////////////////////
			// Book
			hr = LoadFBX("Assets/Fantasy/Book.fbx", &meshes[0], true, 1.0f);

			//VertexBuffer
			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.ByteWidth = sizeof(Vertex) * meshes[0].numVertices;
			bDesc.CPUAccessFlags = 0;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = &((*meshes[0].verts)[0]);

			hr = myDevice->CreateBuffer(&bDesc, &subData, &worldVBuffer);

			//IndexBuffer
			bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bDesc.ByteWidth = sizeof(int) * meshes[0].numIndices;
			bDesc.CPUAccessFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = meshes[0].indices;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &worldIBuffer);

			// Dragon
			hr = LoadFBX("Assets/Fantasy/Dragon.fbx", &meshes[1], true, 0.1f);

			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.ByteWidth = sizeof(Vertex) * meshes[1].numVertices;
			bDesc.CPUAccessFlags = 0;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = &((*meshes[1].verts)[0]);

			hr = myDevice->CreateBuffer(&bDesc, &subData, &dragonVBuffer);

			bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bDesc.ByteWidth = sizeof(int) * meshes[1].numIndices;
			bDesc.CPUAccessFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = meshes[1].indices;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &dragonIBuffer);

			// Wizard
			hr = LoadFBX("Assets/Fantasy/Wizard.fbx", &meshes[2], true, 0.1f);

			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.ByteWidth = sizeof(Vertex) * meshes[2].numVertices;
			bDesc.CPUAccessFlags = 0;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = &((*meshes[2].verts)[0]);

			hr = myDevice->CreateBuffer(&bDesc, &subData, &wizardVBuffer);

			bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bDesc.ByteWidth = sizeof(int) * meshes[2].numIndices;
			bDesc.CPUAccessFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = meshes[2].indices;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &wizardIBuffer);

			// Dwarf
			hr = LoadFBX("Assets/Fantasy/Dwarf.fbx", &meshes[3], false, 0.1f);

			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.ByteWidth = sizeof(Vertex) * meshes[3].numVertices;
			bDesc.CPUAccessFlags = 0;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = &((*meshes[3].verts)[0]);

			hr = myDevice->CreateBuffer(&bDesc, &subData, &dwarfVBuffer);

			bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bDesc.ByteWidth = sizeof(int) * meshes[3].numIndices;
			bDesc.CPUAccessFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = meshes[3].indices;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &dwarfIBuffer);

			// Statue
			hr = LoadFBX("Assets/Fantasy/statue.fbx", &meshes[4], false, 0.01f);

			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.ByteWidth = sizeof(Vertex) * meshes[4].numVertices;
			bDesc.CPUAccessFlags = 0;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = &((*meshes[4].verts)[0]);

			hr = myDevice->CreateBuffer(&bDesc, &subData, &statueVBuffer);

			bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bDesc.ByteWidth = sizeof(int) * meshes[4].numIndices;
			bDesc.CPUAccessFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = meshes[4].indices;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &statueIBuffer);

			// Grid
			hr = GenerateGrid(100.0f, 100.0f, 15, 15, &meshes[5]);

			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.ByteWidth = sizeof(Vertex) * meshes[5].numVertices;
			bDesc.CPUAccessFlags = 0;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = &((*meshes[5].verts)[0]);

			hr = myDevice->CreateBuffer(&bDesc, &subData, &gridVBuffer);

			bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bDesc.ByteWidth = sizeof(int) * meshes[5].numIndices;
			bDesc.CPUAccessFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = meshes[5].indices;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &gridIBuffer);

			// SkyBox
			float skySize = 1.0f;
			Vertex skyBox[24] = 
			{
				//front
				{ {-skySize,	skySize,	-skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {-skySize,	-skySize,	-skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {skySize,		skySize,	-skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {skySize,		-skySize,	-skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				//back
				{ {skySize,		skySize,	skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {skySize,		-skySize,	skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {-skySize,	skySize,	skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {-skySize,	-skySize,	skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				//right
				{ {skySize,		skySize,	-skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {skySize,		-skySize,	-skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {skySize,		skySize,	skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {skySize,		-skySize,	skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				//left
				{ {-skySize,	skySize,	skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {-skySize,	-skySize,	skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {-skySize,	skySize,	-skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {-skySize,	-skySize,	-skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				// top
				{ {-skySize,	skySize,	skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {skySize,		skySize,	skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {-skySize,	skySize,	-skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {skySize,		skySize,	-skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				//bottom
				{ {-skySize,	-skySize,	-skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {skySize,		-skySize,	-skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {-skySize,	-skySize,	skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} },
				{ {skySize,		-skySize,	skySize,	1.0f}, {1.0f, 0.0f}, {0,0,0,0} }
			};

			//skybox vertex buffer
			bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(skyBox);
			bDesc.CPUAccessFlags = 0;
			bDesc.MiscFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = skyBox;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &skyVBuffer);

			int skyBoxIndex[36] = 
			{
				2,0,1,
				3,2,1,

				6,4,7,
				7,4,5,

				8,9,10,
				10,9,11,

				12,13,14,
				14,13,15,

				19,17,16,
				16,18,19,

				20,22,23,
				23,21,20
			};

			//skybox index buffer
			bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bDesc.ByteWidth = sizeof(int) * ARRAYSIZE(skyBoxIndex);
			bDesc.CPUAccessFlags = 0;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DEFAULT;

			subData.pSysMem = skyBoxIndex;

			hr = myDevice->CreateBuffer(&bDesc, &subData, &skyIBuffer);

			//write, compile and load shaders
			 hr = myDevice->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &vShader);
			 hr = myDevice->CreateVertexShader(VS_sinShader, sizeof(VS_sinShader), nullptr, &sinVShader);
			 hr = myDevice->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &pShader);
			 hr = myDevice->CreatePixelShader(PS_BWshader, sizeof(PS_BWshader), nullptr, &bw_PShader);
			 hr = myDevice->CreatePixelShader(PS_Cloudshader, sizeof(PS_Cloudshader), nullptr, &cloud_PShader);
			 hr = myDevice->CreatePixelShader(PS_ReflectionShader, sizeof(PS_ReflectionShader), nullptr, &reflect_PShader);
			 
			 //skybox
			 hr = myDevice->CreateVertexShader(VS_skyboxShader, sizeof(VS_skyboxShader), nullptr, &skybox_VShader);
			 hr = myDevice->CreatePixelShader(PS_skyboxShader, sizeof(PS_skyboxShader), nullptr, &skybox_PShader);

			//describe to d3d11
			 D3D11_INPUT_ELEMENT_DESC ieDesc[] = 
			 {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			 };
			hr = myDevice->CreateInputLayout(ieDesc, 3, VertexShader, sizeof(VertexShader), &vLayout);

			//Matrix constant buffer
			bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bDesc.ByteWidth = sizeof(WVP);
			bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DYNAMIC;

			hr = myDevice->CreateBuffer(&bDesc, nullptr, &cBuffer);

			// Light constant buffer
			bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bDesc.ByteWidth = sizeof(Lights);
			bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DYNAMIC;

			hr = myDevice->CreateBuffer(&bDesc, nullptr, &lightBuffer);

			// Time constant buffer
			bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bDesc.ByteWidth = sizeof(XMFLOAT4);
			bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bDesc.StructureByteStride = 0;
			bDesc.Usage = D3D11_USAGE_DYNAMIC;

			hr = myDevice->CreateBuffer(&bDesc, nullptr, &timeBuffer);



			hr = CreateDDSTextureFromFile(myDevice, L"Assets/fantasy/bookTex.dds", nullptr, &worldTex);
			hr = CreateDDSTextureFromFile(myDevice, L"Assets/fantasy/DragonTex.dds", nullptr, &dragonTex);
			hr = CreateDDSTextureFromFile(myDevice, L"Assets/fantasy/WizardTex.dds", nullptr, &wizardTex);
			hr = CreateDDSTextureFromFile(myDevice, L"Assets/fantasy/DwarfTex.dds", nullptr, &dwarfTex);
			hr = CreateDDSTextureFromFile(myDevice, L"Assets/misc/Saber.dds", nullptr, &saberTex);
			hr = CreateDDSTextureFromFile(myDevice, L"Assets/SkyBox/greenmountains_skyBox.dds", nullptr, &skyTexture);

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

			//-----------------------------------------------------------------------
			// World Matrix
			//-----------------------------------------------------------------------
			worlds[0] = XMMatrixIdentity();

			worlds[1] = XMMatrixTranslation(12, -4.5f, -0.3f);
			worlds[1] = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(90)), worlds[1]);

			worlds[2] = XMMatrixTranslation(17, -4.5f, -0.3f);
			worlds[2] = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(-90)), worlds[2]);

			worlds[3] = XMMatrixTranslation(0, -4.5f, 25.0f);

			worlds[4] = XMMatrixTranslation(20.0f, -50.0f, 20.0f);

			instancedWorlds[0] = XMMatrixTranslation(17.5, -4.5f, -1.0f);
			instancedWorlds[0] = XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(-90)), instancedWorlds[0]);

			instancedWorlds[1] = XMMatrixMultiply(instancedWorlds[0], XMMatrixTranslation(0.6f, 0, 0) );
			instancedWorlds[2] = XMMatrixMultiply(instancedWorlds[1], XMMatrixTranslation(0.6f, 0, 0) );
			instancedWorlds[3] = XMMatrixMultiply(instancedWorlds[0], XMMatrixTranslation(0, 0, 1.2f) );
			instancedWorlds[4] = XMMatrixMultiply(instancedWorlds[3], XMMatrixTranslation(0.6f, 0, 0) );
			instancedWorlds[5] = XMMatrixMultiply(instancedWorlds[4], XMMatrixTranslation(0.6f, 0, 0) );
			//-----------------------------------------------------------------------
			// View Matrix
			//-----------------------------------------------------------------------
			XMMATRIX temp = XMMatrixIdentity();
			temp = XMMatrixRotationX(XMConvertToRadians(25));
			temp = XMMatrixMultiply(temp, XMMatrixTranslation(0, 10, -20));
			temp = XMMatrixInverse(nullptr, temp);
			XMStoreFloat4x4(&myMatrices.vMatrix, temp);
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
	if (worldVBuffer) worldVBuffer->Release();
	if (worldIBuffer) worldIBuffer->Release();
	if (dragonVBuffer) dragonVBuffer->Release();
	if (dragonIBuffer) dragonIBuffer->Release();
	if (wizardVBuffer) wizardVBuffer->Release();
	if (wizardIBuffer) wizardIBuffer->Release();
	if (dwarfVBuffer) dwarfVBuffer->Release();
	if (dwarfIBuffer) dwarfIBuffer->Release();
	if (statueVBuffer) statueVBuffer->Release();
	if (statueIBuffer) statueIBuffer->Release();
	if (gridVBuffer) gridVBuffer->Release();
	if (gridIBuffer) gridIBuffer->Release();

	if (vLayout) vLayout->Release();
	//skybox
	if (skyVBuffer) skyVBuffer->Release();
	if (skyIBuffer) skyIBuffer->Release();
	if (skyTexture) skyTexture->Release();
	if (skybox_VShader) skybox_VShader->Release();
	if (skybox_PShader) skybox_PShader->Release();
	//Vertex shaders
	if (vShader) vShader->Release();
	if (sinVShader) sinVShader->Release();
	//Pixel shaders
	if (pShader) pShader->Release();
	if (bw_PShader) bw_PShader->Release();
	if (cloud_PShader) cloud_PShader->Release();
	if (reflect_PShader) reflect_PShader->Release();
	if (cBuffer) cBuffer->Release();
	if (lightBuffer) lightBuffer->Release();
	if (timeBuffer) timeBuffer->Release();
	if (samplerLin) samplerLin->Release();

	// release shader resource
	if (worldTex) worldTex->Release();
	if (dragonTex) dragonTex->Release();
	if (wizardTex) wizardTex->Release();
	if (dwarfTex) dwarfTex->Release();
	if (saberTex) saberTex->Release();

	for (int i = 0; i < numMesh; ++i)
	{
		delete meshes[i].verts;
		delete meshes[i].indices;
	}

	delete meshes;


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
			//-----------------------------------------------------------------------
			// Camera Controls
			//-----------------------------------------------------------------------
			XMMATRIX temp = XMLoadFloat4x4(&myMatrices.vMatrix);
			temp = XMMatrixInverse(nullptr, temp);

			float xAxisT = 0;
			float xAxisR = 0;
			float yAxisR = 0;
			float yAxisT = 0;
			
			float zAxisT = 0;

			//Translate
			float tSpeed = 5 * (float)timer.SmoothDelta();
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
			float rSpeed = 120 * (float)timer.SmoothDelta();
			if (GetAsyncKeyState('K'))
				xAxisR += rSpeed;
			if (GetAsyncKeyState('I'))
				xAxisR -= rSpeed;
			if (GetAsyncKeyState('J'))
				yAxisR -= rSpeed;
			if (GetAsyncKeyState('L'))
				yAxisR += rSpeed;

			//translation
			temp = XMMatrixMultiply(XMMatrixTranslation(xAxisT, 0, zAxisT), temp);
			temp = XMMatrixMultiply(temp, XMMatrixTranslation(0, yAxisT, 0));
			//rotation
			temp = XMMatrixMultiply(XMMatrixRotationX(XMConvertToRadians(xAxisR)), temp);

			XMVECTOR posVec = temp.r[3];
			temp.r[3] = { 0,0,0,1 };
			temp = XMMatrixMultiply(temp, XMMatrixRotationY(XMConvertToRadians(yAxisR)));
			temp.r[3] = posVec;

			//Grab cam position for specular
			XMStoreFloat4(&myLights.specular[1], posVec);

			XMMATRIX worldViewTemp = XMMatrixTranslation(XMVectorGetX(temp.r[3]), XMVectorGetY(temp.r[3]), XMVectorGetZ(temp.r[3]));
			XMStoreFloat4x4(&myMatrices.worldView, worldViewTemp);

			temp = XMMatrixInverse(nullptr, temp);
			XMStoreFloat4x4(&myMatrices.vMatrix, temp);

			//-----------------------------------------------------------------------
			// Projection Matrix
			//-----------------------------------------------------------------------
			//edit fov
			static float fov = 90;
			if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_LSHIFT))
			{
				if (fov < 90.0f)
					fov += 0.5f;
			}
			else if (GetAsyncKeyState(VK_CONTROL))
			{
				if (fov > 30.0f)
					fov -= 0.5f;
			}

			// edit near- and far-plane
			static float nPlane = 0.1f;
			static float fPlane = 500.0f;
			if (GetAsyncKeyState(VK_RIGHT))
			{
				if (nPlane < fPlane - 0.1f)
					nPlane += 0.1f;
			}
			if (GetAsyncKeyState(VK_LEFT))
			{
				if (nPlane > 0.1f)
					nPlane -= 0.1f;
				if (nPlane < 0.1f)
					nPlane = 0.1f;
			}
			if (GetAsyncKeyState(VK_UP))
			{
				if (fPlane < 1000.0f)
					fPlane += 0.1f;
			}
			if (GetAsyncKeyState(VK_DOWN))
			{
				if (fPlane > nPlane + 0.1f)
					fPlane -= 0.1f;
			}

			// Get aspect ratio
			mySurface->GetAspectRatio(aspectR);
			temp = XMMatrixPerspectiveFovLH(XMConvertToRadians(fov), aspectR, nPlane, fPlane);
			XMStoreFloat4x4(&myMatrices.pMatrix, temp);


			//-----------------------------------------------------------------------
			// Directional Light
			//-----------------------------------------------------------------------
			XMFLOAT4 dColor = { 1.0f, 1.0f, 0.95f, 1 };
			static bool dToggle = true;
			myLights.dLight[0] = { 0, 0, 0, 1.0f };
			myLights.dLight[1] = { 0.577f, 0.577f, -0.577f, 0.0f };
			//toggle dLight
			if (dToggle)
				myLights.dLight[2] = dColor;
			else
				myLights.dLight[2] = { 0,0,0,0 };

			if (GetAsyncKeyState('1') & 0x1)
				dToggle = !dToggle;

			XMVECTOR temp2 = { myLights.dLight[1].x, myLights.dLight[1].y, myLights.dLight[1].z, myLights.dLight[1].w, };
			//Rotate directional light
			//static float rotD = 0.0f; rotD += 0.5f;
			//temp2 = XMVector4Transform(temp2, XMMatrixRotationZ(XMConvertToRadians(rotD)));
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

			if (GetAsyncKeyState('2') & 0x1)
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
			// Spotlight
			//-----------------------------------------------------------------------
			XMFLOAT4 sColor = { 0.5f, 0.5f, 0, 1 };
			static bool sToggle = false;
			// position
			myLights.sLight[0] = { 1.0f, 2.0f, -4.0f, 1.0f };
			// direction
			myLights.sLight[1] = {-0.577f, -0.577f, 0.577f, 0.0f};
			// toggle sLight
			if (sToggle)
				myLights.sLight[2] = sColor;
			else
				myLights.sLight[2] = { 0.0f, 0.0f, 0.0f, 0.0f };

			if (GetAsyncKeyState('3') & 0x1)
				sToggle = !sToggle;

			// innerConeRatio, outerConeRatio, minDistance, maxDistance
			myLights.sLight[3] = { 0.97f, 0.82f, 10.0f, 0.0f };

			temp2 = { myLights.sLight[1].x, myLights.sLight[1].y, myLights.sLight[1].z, myLights.sLight[1].w };
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
			// Specular values
			//-----------------------------------------------------------------------

			//-----------------------------------------------------------------------
			// Time buffer
			//-----------------------------------------------------------------------
			XMFLOAT4 time = { (float)timer.TotalTime(), 0, 0, 0 };

			//-----------------------------------------------------------------------
			// Constant Buffer connection
			//-----------------------------------------------------------------------

			// Attach matrices to cBuffer
			D3D11_MAPPED_SUBRESOURCE gpuBuffer;


			// Attach time to timeBuffer
			myContext->Map(timeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((XMFLOAT4*)(gpuBuffer.pData)) = time;

			myContext->Unmap(timeBuffer, 0);


			//connect constant buffers to pipeline
			ID3D11Buffer * vConstants[] = { cBuffer, timeBuffer };
			myContext->VSSetConstantBuffers(0, 2, vConstants);

			ID3D11Buffer * pConstants[] = { lightBuffer, timeBuffer };
			myContext->PSSetConstantBuffers(0, 2, pConstants);

			//dds
			myContext->PSSetSamplers(0, 1, &samplerLin);

			////////////////////////////////////////////
			// Draw
			////////////////////////////////////////////
#if 1
			//Input Assembler
			myContext->IASetInputLayout(vLayout);

			UINT strides[] = { sizeof(Vertex) };
			UINT offsets[] = { 0 };

			// skyBox
			myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			myContext->VSSetShader(skybox_VShader, 0, 0);
			myContext->PSSetShader(skybox_PShader, 0, 0);

			myContext->IASetVertexBuffers(0, 1, &skyVBuffer, strides, offsets);
			myContext->IASetIndexBuffer(skyIBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &skyTexture);
			myContext->DrawIndexed(36, 0, 0);

			myContext->ClearDepthStencilView(myDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);

			//Vertex Shader Stage
			static bool vsSin = false;
			if (!vsSin)
				myContext->VSSetShader(vShader, 0, 0);
			else
				myContext->VSSetShader(sinVShader, 0, 0);
			//Pixel Shader Stage
			//toggle pixel shader
			static int choosePS = 0;

			switch (choosePS)
			{
			case norm:
				myContext->PSSetShader(pShader, 0, 0);
				break;
			case bw:
				myContext->PSSetShader(bw_PShader, 0, 0);
				break;
			case cloudy:
				myContext->PSSetShader(cloud_PShader, 0, 0);
				break;
			}

			if (GetAsyncKeyState('4') & 0x1)
			{
				if (choosePS < numPS - 1)
					choosePS += 1;
				else
					choosePS = norm;
			}
			if (GetAsyncKeyState('5') & 0x1)
				vsSin = !vsSin;
			///////////////////////////////////////
			// Draw all  meshes
			//////////////////////////////////////
			// Book/World
			// set world matrix
			XMStoreFloat4x4(&myMatrices.wMatrix[0], worlds[0]);
			myContext->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((WVP*)(gpuBuffer.pData)) = myMatrices;
			myContext->Unmap(cBuffer, 0);
			myContext->VSSetConstantBuffers(0, 2, vConstants);

			// Set specular values
			myLights.specular[0].x = 0.0f;	// Specular Intensity
			myLights.specular[0].y = 0.0f; // Specular Power
			myContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((Lights*)(gpuBuffer.pData)) = myLights;
			myContext->Unmap(lightBuffer, 0);

			// Draw
			myContext->IASetVertexBuffers(0, 1, &worldVBuffer, strides, offsets);
			myContext->IASetIndexBuffer(worldIBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &worldTex);

			myContext->DrawIndexed(meshes[0].numIndices, 0, 0);

			// Dragon
			XMStoreFloat4x4(&myMatrices.wMatrix[0], worlds[1]);
			myContext->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((WVP*)(gpuBuffer.pData)) = myMatrices;
			myContext->Unmap(cBuffer, 0);
			myContext->VSSetConstantBuffers(0, 2, vConstants);

			myLights.specular[0].x = 1.0f;
			myLights.specular[0].y = 64.0f;
			myContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((Lights*)(gpuBuffer.pData)) = myLights;
			myContext->Unmap(lightBuffer, 0);

			myContext->IASetVertexBuffers(0, 1, &dragonVBuffer, strides, offsets);
			myContext->IASetIndexBuffer(dragonIBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &dragonTex);

			myContext->DrawIndexed(meshes[1].numIndices, 0, 0);

			// Wizard
			XMStoreFloat4x4(&myMatrices.wMatrix[0], worlds[2]);
			myContext->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((WVP*)(gpuBuffer.pData)) = myMatrices;
			myContext->Unmap(cBuffer, 0);
			myContext->VSSetConstantBuffers(0, 2, vConstants);

			myLights.specular[0].x = 0.0f;
			myLights.specular[0].y = 0.0f;
			myContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((Lights*)(gpuBuffer.pData)) = myLights;
			myContext->Unmap(lightBuffer, 0);

			myContext->IASetVertexBuffers(0, 1, &wizardVBuffer, strides, offsets);
			myContext->IASetIndexBuffer(wizardIBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &wizardTex);

			myContext->DrawIndexed(meshes[2].numIndices, 0, 0);

			// Dwarf
			for (int i = 0; i < 6; ++i)
				XMStoreFloat4x4(&myMatrices.wMatrix[i], instancedWorlds[i]);
			myContext->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((WVP*)(gpuBuffer.pData)) = myMatrices;
			myContext->Unmap(cBuffer, 0);
			myContext->VSSetConstantBuffers(0, 2, vConstants);

			myLights.specular[0].x = 0.0f;
			myLights.specular[0].y = 0.0f;
			myContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((Lights*)(gpuBuffer.pData)) = myLights;
			myContext->Unmap(lightBuffer, 0);

			myContext->IASetVertexBuffers(0, 1, &dwarfVBuffer, strides, offsets);
			myContext->IASetIndexBuffer(dwarfIBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &dwarfTex);

			myContext->DrawIndexedInstanced(meshes[3].numIndices, 6, 0, 0, 0);

			// Grid
			XMStoreFloat4x4(&myMatrices.wMatrix[0], worlds[4]);
			myContext->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((WVP*)(gpuBuffer.pData)) = myMatrices;
			myContext->Unmap(cBuffer, 0);
			myContext->VSSetConstantBuffers(0, 2, vConstants);

			myLights.specular[0].x = 0.0f;
			myLights.specular[0].y = 0.0f;
			myContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((Lights*)(gpuBuffer.pData)) = myLights;
			myContext->Unmap(lightBuffer, 0);

			myContext->IASetVertexBuffers(0, 1, &gridVBuffer, strides, offsets);
			myContext->IASetIndexBuffer(gridIBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &saberTex);

			myContext->DrawIndexed(meshes[5].numIndices, 0, 0);

			// Statue
			XMStoreFloat4x4(&myMatrices.wMatrix[0], worlds[3]);
			myContext->Map(cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((WVP*)(gpuBuffer.pData)) = myMatrices;
			myContext->Unmap(cBuffer, 0);
			myContext->VSSetConstantBuffers(0, 2, vConstants);

			myLights.specular[0].x = 3.0f;
			myLights.specular[0].y = 64.0f;
			myContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
			*((Lights*)(gpuBuffer.pData)) = myLights;
			myContext->Unmap(lightBuffer, 0);

			myContext->IASetVertexBuffers(0, 1, &statueVBuffer, strides, offsets);
			myContext->IASetIndexBuffer(statueIBuffer, DXGI_FORMAT_R32_UINT, 0);
			myContext->PSSetShaderResources(0, 1, &skyTexture);
			myContext->PSSetShader(reflect_PShader, 0, 0);

			myContext->DrawIndexed(meshes[4].numIndices, 0, 0);



#endif
			// Present Backbuffer using Swapchain object
			// Framerate is currently unlocked, we suggest "MSI Afterburner" to track your current FPS and memory usage.
			mySwapChain->Present(1, 0); // set first argument to 1 to enable vertical refresh sync with display

			// Free any temp DX handles aquired this frame
			myRenderTargetView->Release();
		}
	}
}