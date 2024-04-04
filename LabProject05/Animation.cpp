#include "stdafx.h"
#include "Animation.h"
#include "Object.h"

CAnimationController::CAnimationController()
{
	m_fTime = 0.f;
	m_Tracks.reserve(0);
}

void CAnimationController::LoadAnimationFromFile(std::ifstream& InFile)
{
	std::string strToken;

	FBXLoad::ReadStringFromFile(InFile, strToken);

	int nAnimationSets = 0;
	if (strToken == "<AnimationSets>:")
		nAnimationSets = FBXLoad::ReadFromFile<int>(InFile);

	m_Tracks.reserve(nAnimationSets);

	for (int i = 0; i < nAnimationSets; ++i)
	{
		m_Tracks.emplace_back(InFile);
	}

	FBXLoad::ReadStringFromFile(InFile, strToken); // </AnimationSets>
}

void CAnimationController::Animate(const float& fElapsedTime, CGameObject* pRootObject)
{
	for (auto& track : m_Tracks)
	{
		if (track.IsEnable())
		{
			track.Animate(fElapsedTime); 
		}
	}

	for (auto& track : m_Tracks)
	{
		if (track.IsEnable())
		{
			track.UpdateMatrix();
		}
	}

	pRootObject->UpdateTransform(NULL);
}

void CAnimationController::SetFrameCaches(CGameObject* pRootObject)
{
	for (auto& track : m_Tracks)
		track.SetFrameCaches(pRootObject);
}

CAnimationTrack::CAnimationTrack()
{
	m_fPosition = 0.f;
	m_fSpeed = 1.f;
	m_bEnable = true;
	m_fWeight = 1.0f;

	m_pAnimationSet = NULL;

	m_LoopType = AnimationLoopType::Repeat;

}

CAnimationTrack::CAnimationTrack(std::ifstream& InFile) : CAnimationTrack()
{
	std::string strToken;
	FBXLoad::ReadStringFromFile(InFile, strToken);
	if (strToken == "<AnimationSet>:")
	{
		FBXLoad::ReadFromFile<float>(InFile); // index

		FBXLoad::ReadStringFromFile(InFile, strToken); // animation name

		float fStartTime, fEndTime;
		fStartTime = FBXLoad::ReadFromFile<float>(InFile);
		fEndTime = FBXLoad::ReadFromFile<float>(InFile);

		m_pAnimationSet = new CAnimationSet(fStartTime, fEndTime, strToken);

		m_pAnimationSet->LoadLayersFromFile(InFile);
	}

	FBXLoad::ReadStringFromFile(InFile, strToken); // </AnimationSet>
}

CAnimationTrack::~CAnimationTrack()
{
	if (m_pAnimationSet) delete m_pAnimationSet;
}

void CAnimationTrack::Animate(const float& fElapsedTime)
{
	UpdatePosition(fElapsedTime);

	if (m_pAnimationSet) m_pAnimationSet->Animate(m_fPosition, m_fWeight);
}

void CAnimationTrack::SetFrameCaches(CGameObject* pRootObject)
{
	m_pAnimationSet->SetFrameCaches(pRootObject);
}

void CAnimationTrack::UpdateMatrix()
{
	if (m_pAnimationSet) m_pAnimationSet->UpdateMatrix();
}

void CAnimationTrack::UpdatePosition(const float& fElapsedTime)
{
	m_fPosition += (fElapsedTime * m_fSpeed);

	if (m_pAnimationSet->GetEndTime() < m_fPosition)
	{
		m_fPosition = 0.f;

		if (m_LoopType == AnimationLoopType::Once) m_bEnable = false;
	}
}



CAnimationSet::CAnimationSet()
{
	m_fStartTime = 0.f;
	m_fEndTime = 0.f;

	m_Layers.reserve(0);

}

CAnimationSet::CAnimationSet(const float& fStartTime, const float& fEndTime, const std::string& strAnimationName)
	: m_fStartTime(fStartTime), m_fEndTime(fEndTime), m_strAnimationName(strAnimationName)
{
}

void CAnimationSet::LoadLayersFromFile(std::ifstream& InFile)
{
	std::string strToken;
	FBXLoad::ReadStringFromFile(InFile, strToken);
	if (strToken == "<AnimationLayers>:")
	{
		int nLayers = FBXLoad::ReadFromFile<int>(InFile);
		m_Layers.reserve(nLayers);
		for (int i = 0; i < nLayers; ++i)
		{
			m_Layers.emplace_back(InFile);
		}
	}

	FBXLoad::ReadStringFromFile(InFile, strToken); // </AnimationLayers>
}

void CAnimationSet::SetFrameCaches(CGameObject* pRootObject)
{
	for (auto& layer : m_Layers)
		layer.SetFrameCaches(pRootObject);
}

void CAnimationSet::Animate(const float& fPosition, const float& fTrackWeight)
{
	for (auto& Layer : m_Layers)
	{
		Layer.UpdateFrameCachesSRT(fPosition);
	}

	//Blending
	for (auto& Layer : m_Layers)
	{
		for (auto& Frame : Layer.GetFrameCaches())
		{
			Frame->m_xmf3BlendedScale = Vector3::Add(Frame->m_xmf3BlendedScale, Frame->m_xmf3Scale, fTrackWeight);
			Frame->m_xmf3BlendedRotation = Vector3::Add(Frame->m_xmf3BlendedRotation, Frame->m_xmf3Rotation, fTrackWeight);
			Frame->m_xmf3BlendedTranslation = Vector3::Add(Frame->m_xmf3BlendedTranslation, Frame->m_xmf3Translation, fTrackWeight);
		}
	}

}

void CAnimationSet::UpdateMatrix()
{
	//Make Matrix By SRT
	for (auto& Layer : m_Layers)
	{
		for (auto& Frame : Layer.GetFrameCaches())
		{
			XMMATRIX S = XMMatrixScaling(Frame->m_xmf3BlendedScale.x, Frame->m_xmf3BlendedScale.y, Frame->m_xmf3BlendedScale.z);
			XMMATRIX R = XMMatrixMultiply(XMMatrixMultiply(XMMatrixRotationX(Frame->m_xmf3BlendedRotation.x), XMMatrixRotationY(Frame->m_xmf3BlendedRotation.y)), XMMatrixRotationZ(Frame->m_xmf3BlendedRotation.z));
			XMMATRIX T = XMMatrixTranslation(Frame->m_xmf3BlendedTranslation.x, Frame->m_xmf3BlendedTranslation.y, Frame->m_xmf3BlendedTranslation.z);
			XMStoreFloat4x4(&Frame->m_xmf4x4ToParent, XMMatrixMultiply(XMMatrixMultiply(S, R), T));
		}
	}
	
}

CAnimationLayer::CAnimationLayer()
{
	m_fWeight = 1.f;

	m_Curves.reserve(0);
}

CAnimationLayer::CAnimationLayer(std::ifstream& InFile) : CAnimationLayer()
{
	std::string strToken;
	FBXLoad::ReadStringFromFile(InFile, strToken);

	if (strToken == "<AnimationLayer>:")
	{
		FBXLoad::ReadFromFile<int>(InFile); // index

		int nCurves = FBXLoad::ReadFromFile<int>(InFile);

		m_fWeight = FBXLoad::ReadFromFile<float>(InFile);

		m_Curves.reserve(nCurves);
		m_FrameCaches.resize(nCurves);

		for (int i = 0; i < nCurves; ++i)
		{
			m_Curves.emplace_back(InFile);
		}

	}
	FBXLoad::ReadStringFromFile(InFile, strToken); // </AnimationLayer>

}

void CAnimationLayer::SetFrameCaches(CGameObject* pRootObject)
{
	for (int i =0; i < m_Curves.size(); ++i)
	{
		m_FrameCaches[i] = pRootObject->FindFrame(m_Curves[i].m_strFrameName);
	}
}

void CAnimationLayer::UpdateFrameCachesSRT(const float& fPosition)
{
	for (int i = 0 ; i < m_Curves.size(); ++i)
	{
		CGameObject* pFrameCache = m_FrameCaches[i];
		m_Curves[i].GetAnimatedSRT(fPosition, m_fWeight,
			&pFrameCache->m_xmf3Scale, &pFrameCache->m_xmf3Rotation, &pFrameCache->m_xmf3Translation);
	}
}

CAnimationCurve::CAnimationCurve()
{
	m_strFrameName = "-";
	m_KeyTimes.reserve(0);
	m_Scales.reserve(0);
	m_Rotates.reserve(0);
	m_Translations.reserve(0);

}

CAnimationCurve::CAnimationCurve(std::ifstream& InFile): CAnimationCurve()
{
	std::string strToken;
	FBXLoad::ReadStringFromFile(InFile, strToken);

	if (strToken == "<AnimationCurve>:")
	{
		FBXLoad::ReadFromFile<int>(InFile); //index

		FBXLoad::ReadStringFromFile(InFile, strToken);

		m_strFrameName = strToken;

		FBXLoad::ReadStringFromFile(InFile, strToken); 

		while (strToken != "</AnimationCurve>")
		{
			if (strToken == "<TX>:") LoadKeys(InFile, m_Translations);
			if (strToken == "<RX>:") LoadKeys(InFile, m_Rotates);
			if (strToken == "<SX>:") LoadKeys(InFile, m_Scales);

			FBXLoad::ReadStringFromFile(InFile, strToken);
		}
	}
}

void CAnimationCurve::GetAnimatedSRT(const float& fPosition, const float& fLayerWeight, XMFLOAT3* pScale, XMFLOAT3* pRotate, XMFLOAT3* pTranslation)
{
	for (int i = 0; i < m_KeyTimes.size() - 1; ++i)
	{
		if (m_KeyTimes[i] <= fPosition && m_KeyTimes[i + 1] > fPosition)
		{
			float t = (fPosition - m_KeyTimes[i]) / (m_KeyTimes[i + 1] - m_KeyTimes[i]);

			// (S1 * (1.0 - t) + S2 * t) * LayerWeight
			if (m_KeyTimes.size() == m_Scales.size()) 
				*pScale = Vector3::ScalarProduct(Vector3::Add(Vector3::ScalarProduct(m_Scales[i], 1.0f - t, false), Vector3::ScalarProduct(m_Scales[i + 1], t, false)), fLayerWeight, false);
			if (m_KeyTimes.size() == m_Rotates.size()) 
				*pRotate = Vector3::ScalarProduct(Vector3::Add(Vector3::ScalarProduct(m_Rotates[i], 1.0f - t, false), Vector3::ScalarProduct(m_Rotates[i + 1], t, false)), fLayerWeight, false);
			if (m_KeyTimes.size() == m_Translations.size()) 
				*pTranslation = Vector3::ScalarProduct(Vector3::Add(Vector3::ScalarProduct(m_Translations[i], 1.0f - t, false), Vector3::ScalarProduct(m_Translations[i + 1], t, false)), fLayerWeight, false);
			
			return;
		}
	}

	// S * LayerWeight
	if (m_KeyTimes.size() == m_Scales.size()) *pScale = Vector3::ScalarProduct(m_Scales[m_Scales.size() - 1], fLayerWeight, false);
	if (m_KeyTimes.size() == m_Rotates.size()) *pRotate = Vector3::ScalarProduct(m_Rotates[m_Rotates.size() - 1], fLayerWeight, false);
	if (m_KeyTimes.size() == m_Translations.size()) *pTranslation = Vector3::ScalarProduct(m_Translations[m_Translations.size() - 1], fLayerWeight, false);

}

void CAnimationCurve::LoadKeys(std::ifstream& InFile, std::vector<XMFLOAT3>& vecKeyValues)
{
	int nKeys = FBXLoad::ReadFromFile<int>(InFile); 

	vecKeyValues.reserve(nKeys);

	float* pBuffer = new float[nKeys * 2]; // fkeytimes, fkeyvalues

	//X
	FBXLoad::ReadFromFile(InFile, pBuffer, nKeys * 2);
	if (nKeys != m_KeyTimes.size())
	{
		m_KeyTimes.reserve(nKeys);
		for (int i = 0; i < nKeys; ++i)
			m_KeyTimes.emplace_back(pBuffer[i]);
	}

	for (int i = nKeys; i < nKeys * 2; ++i) vecKeyValues.emplace_back(pBuffer[i], 0.f, 0.f);


	//Y
	std::string strToken;
	FBXLoad::ReadStringFromFile(InFile, strToken); //<TY>
	FBXLoad::ReadFromFile<int>(InFile);

	FBXLoad::ReadFromFile(InFile, pBuffer, nKeys * 2);

	for (int i = nKeys; i < nKeys * 2; ++i) vecKeyValues[i - nKeys].y = pBuffer[i];
	


	//Z
	FBXLoad::ReadStringFromFile(InFile, strToken); //<TZ>
	FBXLoad::ReadFromFile<int>(InFile);

	FBXLoad::ReadFromFile(InFile, pBuffer, nKeys * 2);

	for (int i = nKeys; i < nKeys * 2; ++i) vecKeyValues[i - nKeys].z = pBuffer[i];

}

