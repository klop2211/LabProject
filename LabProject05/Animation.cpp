#include "stdafx.h"
#include "Animation.h"

CAnimationController::CAnimationController()
{
	m_fTime = 0.f;
	m_Tracks.reserve(0);
}

void CAnimationController::LoadAnimationFromFile(const std::string& strFileName)
{
	std::ifstream InFile{ strFileName };

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
}



CAnimationTrack::CAnimationTrack()
{
	m_fPosition = 0.f;
	m_fSpeed = 0.f;
	m_bEnable = false;
	m_fWeight = 1.0f;

	m_pAnimationSet = NULL;

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

}

CAnimationTrack::~CAnimationTrack()
{
	if (m_pAnimationSet) delete m_pAnimationSet;
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

}

CAnimationLayer::CAnimationLayer()
{
	m_fWeight = 0.f;

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

		for (int i = 0; i < nCurves; ++i)
		{
			m_Curves.emplace_back(InFile);
		}

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
			if (strToken == "<SX>:") LoadKeys(InFile, m_Scales);
			if (strToken == "<RX>:") LoadKeys(InFile, m_Rotates);

			FBXLoad::ReadStringFromFile(InFile, strToken);
		}
	}
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

	for (int i = 0; i < nKeys; ++i) vecKeyValues.emplace_back(pBuffer[i], 0.f, 0.f);


	//Y
	std::string strToken;
	FBXLoad::ReadStringFromFile(InFile, strToken); //<TY>
	FBXLoad::ReadFromFile<int>(InFile);

	FBXLoad::ReadFromFile(InFile, pBuffer, nKeys * 2);

	for (int i = 0; i < nKeys; ++i) vecKeyValues[i].y = pBuffer[i];


	//Z
	FBXLoad::ReadStringFromFile(InFile, strToken); //<TZ>
	FBXLoad::ReadFromFile<int>(InFile);

	FBXLoad::ReadFromFile(InFile, pBuffer, nKeys * 2);

	for (int i = 0; i < nKeys; ++i) vecKeyValues[i].z = pBuffer[i];

}

