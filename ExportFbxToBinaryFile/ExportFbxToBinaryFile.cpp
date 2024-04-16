// ExportFbxToBinaryFile.cpp :이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>

#include "Common/Common.h"
#include "DisplayCommon.h"
#include "DisplayHierarchy.h"
#include "DisplayAnimation.h"

FILE *gpBinaryFile = NULL;
char pszBuffer[256];

void WriteStringToFile(char *pszString) { ::fwrite(pszString, sizeof(char), strlen(pszString), gpBinaryFile); }

void WriteGlobalTimeSettings(FbxGlobalSettings *pfbxGlobalSettings)
{
	char pstrTimeString[256];

	WriteString("<TimeMode>:", FbxGetTimeModeName(pfbxGlobalSettings->GetTimeMode()));

	FbxTimeSpan fbxTimeSpan;
	pfbxGlobalSettings->GetTimelineDefaultTimeSpan(fbxTimeSpan);
	FbxTime fbxTimeStart = fbxTimeSpan.GetStart();
	FbxTime fbxTimeEnd = fbxTimeSpan.GetStop();
	WriteString("<TimelineDefaultTimeSpan>:");
	WriteString(fbxTimeStart.GetTimeString(pstrTimeString, FbxUShort(256)));
	WriteString(fbxTimeEnd.GetTimeString(pstrTimeString, FbxUShort(256)));
}

int main(int argc, char** argv)
{
	FbxManager *pfbxSdkManager = NULL;
    FbxScene *pfbxScene = NULL;

    InitializeSdkObjects(pfbxSdkManager);

//	FbxString pfbxstrModelFilePath("Eagle.fbx");
//	char *pszWriteFileName = "Eagle.bin";
//	FbxString pfbxstrModelFilePath("Lion.fbx");
//	char *pszWriteFileName = "Lion.bin";
	FbxString pfbxstrModelFilePath("../Resource/Model/Mawang_Zup.fbx");
	char *pszWriteFileName = "../Resource/Model/Mawang_Zup.bin";
//	FbxString pfbxstrModelFilePath("Angrybot.fbx");
//	char *pszWriteFileName = "Angrybot.bin";
//	FbxString pfbxstrModelFilePath("Elven_Witch.fbx");
//	char *pszWriteFileName = "Elven_Witch.bin";

    FbxScene *pfbxModelScene = FbxScene::Create(pfbxSdkManager, pfbxstrModelFilePath);
	bool bResult = LoadScene(pfbxSdkManager, pfbxModelScene, pfbxstrModelFilePath.Buffer());

//#define _WITH_SEPARATED_ANIMATIONS

#ifdef _WITH_SEPARATED_ANIMATIONS
	const int nSeparatedAnimations = 3;
	FbxString pfbxstrAnimationFilePaths[nSeparatedAnimations];
	FbxScene *ppfbxAnimationScenes[nSeparatedAnimations];
	pfbxstrAnimationFilePaths[0] = "Lion@IdleBreathe.fbx";
	pfbxstrAnimationFilePaths[1] = "Lion@Walk.fbx";
	pfbxstrAnimationFilePaths[2] = "Lion@Run.fbx";

	//const int nSeparatedAnimations = 3;
	//FbxString pfbxstrAnimationFilePaths[nSeparatedAnimations];
	//FbxScene *ppfbxAnimationScenes[nSeparatedAnimations];
	//pfbxstrAnimationFilePaths[0] = "Eagle@TakeOffGrounded.fbx";
	//pfbxstrAnimationFilePaths[1] = "Eagle@Fly.fbx";
	//pfbxstrAnimationFilePaths[2] = "Eagle@Glide.fbx";

	for(int i = 0; i < nSeparatedAnimations; i++)
	{
		ppfbxAnimationScenes[i] = FbxScene::Create(pfbxSdkManager, pfbxstrAnimationFilePaths[i]);
		LoadScene(pfbxSdkManager, ppfbxAnimationScenes[i], pfbxstrAnimationFilePaths[i].Buffer());
	}
#endif
	FbxGeometryConverter fbxGeomConverter(pfbxSdkManager);

	//FbxAxisSystem fbxSceneAxisSystem = pfbxModelScene->GetGlobalSettings().GetAxisSystem();
	//FbxAxisSystem fbxDirectXAxisSystem(FbxAxisSystem::eDirectX);
	//if (fbxSceneAxisSystem != fbxDirectXAxisSystem) fbxDirectXAxisSystem.ConvertScene(pfbxModelScene);

	//FbxSystemUnit fbxSceneSystemUnit = pfbxModelScene->GetGlobalSettings().GetSystemUnit();
	//if (fbxSceneSystemUnit.GetScaleFactor() != 1.0) FbxSystemUnit::cm.ConvertScene(pfbxModelScene);

	fbxGeomConverter.Triangulate(pfbxModelScene, true);
	fbxGeomConverter.RemoveBadPolygonsFromMeshes(pfbxModelScene, NULL);
		//fbxGeomConverter.SplitMeshesPerMaterial(pfbxModelScene, true);


#ifdef _WITH_SEPARATED_ANIMATIONS
	//for(int i = 0; i < nSeparatedAnimations; i++)
	//{
	//	if (fbxSceneSystemUnit.GetScaleFactor() != 1.0) FbxSystemUnit::cm.ConvertScene(ppfbxAnimationScenes[i]);
	//}
#endif

	::fopen_s(&gpBinaryFile, pszWriteFileName, "wb");

	//WriteString("<GlobalTimeSettings>:");
  	//WriteGlobalTimeSettings(&pfbxScene->GetGlobalSettings());
	//WriteString("</GlobalTimeSettings>");

	WriteString("<Hierarchy>");
	WriteHierarchy(pfbxModelScene);
	WriteString("</Hierarchy>");

	WriteString("<Animation>");
#ifdef _WITH_SEPARATED_ANIMATIONS
	WriteAnimation(ppfbxAnimationScenes, nSeparatedAnimations);
#else
	WriteAnimation(pfbxModelScene);
#endif
	WriteString("</Animation>");

	::fclose(gpBinaryFile);
    DestroySdkObjects(pfbxSdkManager, bResult);

	return(0);
}

