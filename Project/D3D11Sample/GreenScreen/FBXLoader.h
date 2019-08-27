#pragma once
#include "MyMath.h"
#include <fbxsdk.h>

#include <vector>
#include <winnt.h>
#include <winerror.h>

struct Vertex
{
	XMFLOAT4 pos;
	XMFLOAT2 uv;
	XMFLOAT4 normal;
};


//FbxManager* fbxSdkManager = nullptr;
//
//void LoadUVInformation(FbxMesh* pMesh, std::vector<FbxVector2>& uvVerts)
//{
//	//get all UV set names
//	FbxStringList lUVSetNameList;
//	pMesh->GetUVSetNames(lUVSetNameList);
//
//	//iterating over all uv sets
//	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
//	{
//		//get lUVSetIndex-th uv set
//		const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
//		const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);
//
//		if (!lUVElement)
//			continue;
//
//		// only support mapping mode eByPolygonVertex and eByControlPoint
//		if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
//			lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
//			return;
//
//		//index array, where holds the index referenced to the uv data
//		const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
//		const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;
//
//		//iterating through the data by polygon
//		const int lPolyCount = pMesh->GetPolygonCount();
//
//		if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
//		{
//			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
//			{
//				// build the max index array that we need to pass into MakePoly
//				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
//				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
//				{
//					FbxVector2 lUVValue;
//
//					//get the index of the current vertex in control points array
//					int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex, lVertIndex);
//
//					//the UV index depends on the reference mode
//					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;
//
//					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);
//
//					uvVerts.push_back(lUVValue);
//				}
//			}
//		}
//		else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
//		{
//			int lPolyIndexCounter = 0;
//			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
//			{
//				// build the max index array that we need to pass into MakePoly
//				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
//				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
//				{
//					if (lPolyIndexCounter < lIndexCount)
//					{
//						FbxVector2 lUVValue;
//
//						//the UV index depends on the reference mode
//						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;
//
//						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);
//
//						uvVerts.push_back(lUVValue);
//
//						lPolyIndexCounter++;
//					}
//				}
//			}
//		}
//	}
//}
//
//HRESULT LoadFBX(std::vector<Vertex>* outVertexVector, int* outIndices, char* filePath)
//{
//	//init manager
//	if (fbxSdkManager == nullptr)
//	{
//		fbxSdkManager = FbxManager::Create();
//
//		//create settings for manager
//		FbxIOSettings* IOSettings = FbxIOSettings::Create(fbxSdkManager, IOSROOT);
//		fbxSdkManager->SetIOSettings(IOSettings);
//	}
//
//	FbxImporter* pImporter = FbxImporter::Create(fbxSdkManager, "");
//	FbxScene* pFbxScene = FbxScene::Create(fbxSdkManager, "");
//
//	bool success = pImporter->Initialize(filePath, -1, fbxSdkManager->GetIOSettings());
//
//	if (!success) return E_FAIL;
//	return S_OK;
//
//	success = pImporter->Import(pFbxScene);
//	if (!success) return E_FAIL;
//
//	pImporter->Destroy();
//
//	FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
//
//	if (pFbxRootNode)
//	{
//		for (int i = 0; i < pFbxRootNode->GetChildCount(); ++i)
//		{
//			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);
//
//			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();
//
//			if (AttributeType != FbxNodeAttribute::eMesh)
//				continue;
//
//			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();
//
//			int numVerts = pMesh->GetControlPointsCount();
//			int numIndices = pMesh->GetPolygonVertexCount();
//			outIndices = pMesh->GetPolygonVertices();
//
//			//Load position data
//
//			std::vector<Vertex> tempVerts;
//
//			for (int j = 0; j < numVerts; j++)
//			{
//				FbxVector4 pVertices = pMesh->GetControlPointAt(j);
//				Vertex myVert;
//				myVert.pos.x = (float)pVertices.mData[0];
//				myVert.pos.y = (float)pVertices.mData[1];
//				myVert.pos.z = (float)pVertices.mData[2];
//
//				myVert.uv = { 0,0 };
//
//				myVert.normal = { 0,0,0,0 };
//
//				tempVerts.push_back(myVert);
//			}
//
//
//			std::vector<FbxVector2> pUVs;
//			LoadUVInformation(pMesh, pUVs);
//			FbxArray<FbxVector4> pNorms;
//			pMesh->GetPolygonVertexNormals(pNorms);
//
//			//load remaining data
//			for (int j = 0; j < numIndices; ++j)
//			{
//				Vertex myVert;
//				myVert.pos = tempVerts[outIndices[j]].pos;
//				myVert.normal.x = pNorms.GetAt(j)[0];
//				myVert.normal.y = pNorms.GetAt(j)[1];
//				myVert.normal.z = pNorms.GetAt(j)[2];
//				myVert.uv = XMFLOAT2((float)pUVs[j].mData[0], (float)pUVs[j].mData[1]);
//				outVertexVector->push_back(myVert);
//			}
//		}
//	}
//}



