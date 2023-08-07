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
    else
#ifdef _DEBUG
        FBXSDK_printf("Autodesk FBX SDK version %s\n", m_plSdkManager->GetVersion());
#endif // _DEBUG


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
    if (pExitStatus)
#ifdef _DEBUG
        FBXSDK_printf("Program Success!\n");
#endif // _DEBUG
}

LoadResult CFBXLoader::LoadScene(const char* pFilename)
{
    LoadResult result;

    if (CheckFileNameList(pFilename))
    {
        result = LoadResult::Overlapping;
    }
    else {
        if (LoadSceneFromFile(pFilename))
            result = LoadResult::First;
        else
            result = LoadResult::False;
    }

    return result;
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
    if (m_vfileNameList.end() != find(m_vfileNameList.begin(), m_vfileNameList.end(), pFilename))
        return true;
    // 없으면 리스트에 넣고 false 반환.
    m_vfileNameList.push_back(pFilename);
    return false;
}

FbxScene* CFBXLoader::GetScene()
{
    return m_plScene;
}
