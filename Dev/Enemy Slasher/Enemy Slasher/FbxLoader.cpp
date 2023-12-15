#include "stdafx.h"
#include "FbxLoader.h"

#ifdef IOS_REF  // 모호합니다.
#undef  IOS_REF
#define IOS_REF (*(m_plSdkManager->GetIOSettings()))
#endif

CFBXLoader::CFBXLoader()
{
    InitializeSdkObjects();
}

CFBXLoader::~CFBXLoader()
{
    DestroySdkObjects(true);    // 필요한가?
}

void CFBXLoader::InitializeSdkObjects()
{
    m_plSdkManager = NULL;
    m_plScene = NULL;
    if (!m_plSdkManager)
        m_plSdkManager = FbxManager::Create();

    //The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
    if (!m_plSdkManager)
    {
        //FBXSDK_printf("Error: Unable to create FBX Manager!\n");
        exit(1);
    }
    else {
#ifdef _DEBUG
        FBXSDK_printf("Autodesk FBX SDK version %s\n", m_plSdkManager->GetVersion());
#endif // _DEBUG
    }

    //Create an IOSettings object. This object holds all import/export settings.
    FbxIOSettings* ios = FbxIOSettings::Create(m_plSdkManager, IOSROOT);
    m_plSdkManager->SetIOSettings(ios);

    //Load plugins from the executable directory (optional)
    FbxString lPath = FbxGetApplicationDirectory();
    m_plSdkManager->LoadPluginsDirectory(lPath.Buffer());

    //Create an FBX scene. This object holds most objects imported/exported from/to files.
    m_plScene = FbxScene::Create(m_plSdkManager, "My Scene");
    if (!m_plScene)
    {
        //FBXSDK_printf("Error: Unable to create FBX scene!\n");
        exit(1);
    }
}

void CFBXLoader::DestroySdkObjects(bool pExitStatus)
{
    if (m_plSdkManager)
        m_plSdkManager->Destroy();
    if (pExitStatus) {
#ifdef _DEBUG
        FBXSDK_printf("Program Success!\n");
#endif // _DEBUG
    }
}

LoadResult CFBXLoader::LoadScene(const char* pFilename)
{
    LoadResult result;

    if (CheckFileNameList(pFilename))
    {
        result = LoadResult::Overlapping;
#ifdef _DEBUG
        FBXSDK_printf("insertResult::Overlapping, File name : %s\n", pFilename);
#endif // _DEBUG
    }
    else {
        if (LoadSceneFromFile(pFilename))
        {
            result = LoadResult::First;
            // 없으면 리스트에 넣고 false 반환.
            FbxNode* lNode = GetScene()->GetRootNode();

            auto re = m_mfileNameList.insert({ pFilename, lNode });
#ifdef _DEBUG
            FBXSDK_printf("insertResult::First, File name : %s\n", pFilename);
#endif // _DEBUG
        }
        else
        {
            result = LoadResult::False;
#ifdef _DEBUG
            FBXSDK_printf("insertResult::False, File name : %s\n", pFilename);
#endif // _DEBUG
        }
    }

#ifdef _DEBUG
    //FBXSDK_printf("LoadResult::%d\n", static_cast<int>(result));
#endif // _DEBUG
    return result;
}

LoadResult CFBXLoader::LoadScene(FbxString lFilePath)
{
    LoadResult lResult;
    if (lFilePath.IsEmpty())
    {
        lResult = LoadResult::False;
#ifdef _DEBUG
        FBXSDK_printf("\n\nUsage: ImportScene <FBX file name>\n\n");
#endif // _DEBUG
    }
    else
    {
#ifdef _DEBUG
        //FBXSDK_printf("\n\nFile: %s\n\n", lFilePath.Buffer());
#endif // _DEBUG
        lResult = LoadScene(lFilePath.Buffer());
    }
    return lResult;
}


bool CFBXLoader::LoadSceneFromFile(const char* pFilename)
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor, lSDKMinor, lSDKRevision;
    //int lFileFormat = -1;
    int lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    FbxImporter* lImporter = FbxImporter::Create(m_plSdkManager, "");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, m_plSdkManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if (!lImportStatus)
    {
        FbxString error = lImporter->GetStatus().GetErrorString();
#ifdef _DEBUG
        FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
        FBXSDK_printf("Error returned: %s\n\n", error.Buffer());
#endif // _DEBUG

        if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
#ifdef _DEBUG
            FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
#endif // _DEBUG
        }

        return false;
    }
#ifdef _DEBUG
    //FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
#endif // _DEBUG


    if (lImporter->IsFBX())
    {
#ifdef _DEBUG
        //FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
#endif // _DEBUG


        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

        //FBXSDK_printf("Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        //FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
        //FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        //FBXSDK_printf("\n");

        for (int i = 0; i < lAnimStackCount; i++)
        {
            FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            //FBXSDK_printf("    Animation Stack %d\n", i);
            //FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
            //FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should be imported 
            // under a different name.
            //FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported. 
            //FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
            //FBXSDK_printf("\n");
        }

        // Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
        IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
        IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
        IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
        IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
        IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
        IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(m_plScene);
    if (lStatus == false && lImporter->GetStatus() == FbxStatus::ePasswordError)
    {
        //FBXSDK_printf("Please enter password: ");

        lPassword[0] = '\0';

        //FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
            //scanf("%s", lPassword);
        //FBXSDK_CRT_SECURE_NO_WARNING_END

        FbxString lString(lPassword);

        IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
        IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

        lStatus = lImporter->Import(m_plScene);

        if (lStatus == false && lImporter->GetStatus() == FbxStatus::ePasswordError)
        {
            //FBXSDK_printf("\nPassword is wrong, import aborted.\n");
        }
    }

    if (!lStatus || (lImporter->GetStatus() != FbxStatus::eSuccess))
    {
        //FBXSDK_printf("********************************************************************************\n");
        if (lStatus)
        {
            //FBXSDK_printf("WARNING:\n");
            //FBXSDK_printf("   The importer was able to read the file but with errors.\n");
            //FBXSDK_printf("   Loaded scene may be incomplete.\n\n");
        }
        else
        {
            //FBXSDK_printf("Importer failed to load the file!\n\n");
        }

        if (lImporter->GetStatus() != FbxStatus::eSuccess);
        //FBXSDK_printf("   Last error message: %s\n", lImporter->GetStatus().GetErrorString());

        FbxArray<FbxString*> history;
        lImporter->GetStatus().GetErrorStringHistory(history);
        if (history.GetCount() > 1)
        {
            //FBXSDK_printf("   Error history stack:\n");
            for (int i = 0; i < history.GetCount(); i++)
            {
                //FBXSDK_printf("      %s\n", history[i]->Buffer());
            }
        }
        FbxArrayDelete<FbxString*>(history);
        //FBXSDK_printf("********************************************************************************\n");
    }

    // Destroy the importer.
    lImporter->Destroy();

    return lStatus;
}

bool CFBXLoader::CheckFileNameList(const char* pFilename)
{
    // 리스트를 확인해 리스트에 있으면  true 반환. 
    if (m_mfileNameList.contains(pFilename))
    {
        return true;
    }
    
    return false;
}

FbxNode* CFBXLoader::GetRootNode(const char* filename)
{
    return m_mfileNameList.find(filename)->second;
}

FbxScene* CFBXLoader::GetScene()
{
    return m_plScene;
}

void CFBXLoader::LoadAnimaitionOnly(const char* fileName)
{
    LoadResult lResult;
    FbxString lFilePath(fileName);

    lResult = LoadScene(lFilePath);

    if (lResult == LoadResult::False)
    {
#ifdef _DEBUG
        FBXSDK_printf("\n\nAn error occurred while loading the scene...\n");
#endif // _DEBUG

    }
    else if (lResult == LoadResult::First)
    {
        LoadAnimation(lFilePath.Buffer());
    }
    else if (lResult == LoadResult::Overlapping)
    {
        // 애니메이션은 중복 로드 할 필요가 없음. 경우도 없을것으로 예상.
    }
}

void CFBXLoader::LoadAnimation(const char* fileName)
{
    int i;
    FbxScene* pScene = m_plScene;

    // 스택이 하나임을 가정한다.
    for (i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
    {
        FbxAnimStack* lAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);

        //FbxString lOutputString = "Animation Stack Name: ";
        //lOutputString += lAnimStack->GetName();
        //lOutputString += "\n";
        //FBXSDK_printf(lOutputString);


        // pFBXLoader의 m_mAnimationList에 새로운 애니메이션을 저장
        // 저장할때 파일 이름을 기준으로 저장하므로 주의할것.

        FbxTimeSpan timeSpan = lAnimStack->GetLocalTimeSpan();
        FbxTime start = timeSpan.GetStart();
        FbxTime end = timeSpan.GetStop();
        int totalFrames = (end - start).GetFrameCount(DEFAULT_TIME_MODE);

        // map은 같은 키(fileName)로 insert 할 경우 무시된다.
        CAnimationData* pNewAniData = new CAnimationData(totalFrames);
        m_mAnimationList.insert(std::make_pair(fileName, pNewAniData));

#ifdef _DEBUG
        std::cout << "LoadAnimation() 애니메이션 데이터 insert 성공 " << fileName << std::endl;
#endif // _DEBUG

        LoadAnimation(lAnimStack, pScene->GetRootNode(), pNewAniData);
    }
}

void CFBXLoader::LoadAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, CAnimationData* pNewAniData, bool isSwitcher)
{
    int l;
    int nbAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
    //int nbAudioLayers = pAnimStack->GetMemberCount<FbxAudioLayer>();
    //FbxString lOutputString;

    //lOutputString = "   contains ";
    //if (nbAnimLayers == 0 /*&& nbAudioLayers == 0*/)
    //	lOutputString += "no layers";

    //if (nbAnimLayers)
    //{
    //	lOutputString += nbAnimLayers;
    //	lOutputString += " Animation Layer";
    //	if (nbAnimLayers > 1)
    //		lOutputString += "s";
    //}

    //if (nbAudioLayers)
    //{
    //	if (nbAnimLayers)
    //		lOutputString += " and ";
    //
    //	lOutputString += nbAudioLayers;
    //	lOutputString += " Audio Layer";
    //	if (nbAudioLayers > 1)
    //		lOutputString += "s";
    //}
    //lOutputString += "\n\n";
    //FBXSDK_printf(lOutputString);

    // 레이어는 하나임을 가정한다.
    // 변경할 경우pNewAniData에도 반영해야함.
    for (l = 0; l < nbAnimLayers; l++)
    {
        FbxAnimLayer* lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(l);

        //lOutputString = "AnimLayer ";
        //lOutputString += l;
        //lOutputString += "\n";
        //FBXSDK_printf(lOutputString);

        LoadAnimation(lAnimLayer, pNode, pNewAniData, isSwitcher);
    }

    //for (l = 0; l < nbAudioLayers; l++)
    //{
    //	FbxAudioLayer* lAudioLayer = pAnimStack->GetMember<FbxAudioLayer>(l);
    //
    //	lOutputString = "AudioLayer ";
    //	lOutputString += l;
    //	lOutputString += "\n";
    //	FBXSDK_printf(lOutputString);
    //
    //	DisplayAnimation(lAudioLayer, isSwitcher);
    //	FBXSDK_printf("\n");
    //}
}

void CFBXLoader::LoadAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, CAnimationData* pNewAniData, bool isSwitcher)
{
    int lModelCount;
    //FbxString lOutputString;

    //lOutputString = "     Node Name: ";
    //lOutputString += pNode->GetName();
    //lOutputString += "\n\n";
    //FBXSDK_printf(lOutputString);

    //FBXSDK_printf("\n");
    // 아래 함수의 반환값과 노드의 이름을 갖는 pair를 pNewAniData->m_mAnimationData에 insert
    auto dataMap= DisplayChannels(pNode, pAnimLayer, isSwitcher);

    pNewAniData->insertData(std::make_pair(pNode->GetName(), dataMap));

    for (lModelCount = 0; lModelCount < pNode->GetChildCount(); lModelCount++)
    {
        LoadAnimation(pAnimLayer, pNode->GetChild(lModelCount), pNewAniData, isSwitcher);
    }
}

std::map<int, XMFLOAT4X4> CFBXLoader::DisplayChannels(FbxNode* pNode, FbxAnimLayer* pAnimLayer, bool isSwitcher)
{
    FbxAnimCurve* lAnimCurve = NULL;

    std::map<int, XMFLOAT4X4> dataMap;
    std::map<int, float> Tx, Ty, Tz, Rx, Ry, Rz, Sx, Sy, Sz;


    // Display general curves.
    if (!isSwitcher)
    {
        lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if (lAnimCurve)
            Tx = DisplayCurve(lAnimCurve);
        lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if (lAnimCurve)
            Ty = DisplayCurve(lAnimCurve);
        lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if (lAnimCurve)
            Tz = DisplayCurve(lAnimCurve);

        lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if (lAnimCurve)
            Rx = DisplayCurve(lAnimCurve);
        lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if (lAnimCurve)
            Ry = DisplayCurve(lAnimCurve);
        lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if (lAnimCurve)
            Rz = DisplayCurve(lAnimCurve);

        lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if (lAnimCurve)
            Sx = DisplayCurve(lAnimCurve);
        lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if (lAnimCurve)
            Sy = DisplayCurve(lAnimCurve);
        lAnimCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if (lAnimCurve)
            Sz = DisplayCurve(lAnimCurve);
    }

    for (const auto& entry : Tx) {
        int key = entry.first;

        // 해당 키에 대한 변환 정보 가져오기
        float tx = Tx[key];
        float ty = Ty[key];
        float tz = Tz[key];
        float rx = Rx[key];
        float ry = Ry[key];
        float rz = Rz[key];
        float sx = Sx[key];
        float sy = Sy[key];
        float sz = Sz[key];

        // 월드 변환 행렬 생성
        XMFLOAT4X4 worldMatrix;
        XMMATRIX translationMatrix = XMMatrixTranslation(tx, ty, tz);
        XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rx, ry, rz);
        XMMATRIX scaleMatrix = XMMatrixScaling(sx, sy, sz);

        // 월드 변환 행렬 조합
        XMMATRIX combinedMatrix = scaleMatrix * rotationMatrix * translationMatrix;

        XMStoreFloat4x4(&worldMatrix, combinedMatrix);

        // 결과를 dataMap에 저장
        dataMap.insert(std::make_pair(key, worldMatrix));

    }
    return dataMap;
}

std::map<int, float> CFBXLoader::DisplayCurve(FbxAnimCurve* pCurve)
{
    FbxTime   lKeyTime; // 프레임. 0부터 개수만큼 반복.
    float   lKeyValue;
    int     lCount;
    std::map<int, float> result;

    int lKeyCount = pCurve->KeyGetCount();

    for (lCount = 0; lCount < lKeyCount; lCount++)
    {
        lKeyValue = static_cast<float>(pCurve->KeyGetValue(lCount));
        //lKeyTime = pCurve->KeyGetTime(lCount);
        result.insert(std::make_pair(lCount, lKeyValue));
    }
    return result;
}

CAnimationData* CFBXLoader::GetAnimationData(string name)
{
    auto p = m_mAnimationList.find(name);
    if (p != m_mAnimationList.end())
    {
#ifdef _DEBUG
        std::cout << "GetAnimationData() 애니메이션 데이터 find 성공 " << p->first << std::endl;
#endif // _DEBUG
        return p->second;
    }
    else
    {
#ifdef _DEBUG
        std::cout << "GetAnimationData() 애니메이션 데이터 find 실패" << p->first << std::endl;
#endif // _DEBUG
        return nullptr;
    }
}
