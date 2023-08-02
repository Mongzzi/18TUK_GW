#pragma once
#include <fbxsdk.h>

#ifdef IOS_REF  // 모호합니다.
    #undef  IOS_REF
    #define IOS_REF (*(pManager->GetIOSettings()))
#endif

// 모호합니다 뜨면 ::FbxManager
// 모호하다는 class 앞에 :: 붙여볼것. global namespace
void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);
//void DisplayContent(FbxScene* pScene);
//void DisplayContent(FbxNode* pNode);
//void DisplayMesh(FbxNode* pNode);
bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);



void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
    // 이 두줄이 없으면 오류나옴. 왜? 것보다 어떻게 해결했음? 이게 해결한건가?
    // 아마 FbxManager의 생성자가 static이여서 생기는 문제인듯
    if(pManager) pManager->Destroy();
    else pManager = FbxManager::Create();

    //The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
    if (!pManager)
    {
        //FBXSDK_printf("Error: Unable to create FBX Manager!\n");
        exit(1);
    }
    else;//FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

    //Create an IOSettings object. This object holds all import/export settings.
    FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
    pManager->SetIOSettings(ios);

    //Load plugins from the executable directory (optional)
    FbxString lPath = FbxGetApplicationDirectory();
    pManager->LoadPluginsDirectory(lPath.Buffer());

    //Create an FBX scene. This object holds most objects imported/exported from/to files.
    pScene = FbxScene::Create(pManager, "My Scene");
    if (!pScene)
    {
        //FBXSDK_printf("Error: Unable to create FBX scene!\n");
        exit(1);
    }
}

void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
    if (pManager) pManager->Destroy();
    //if (pExitStatus) FBXSDK_printf("Program Success!\n");
}

//-------------------------------------------------------------------------------------------
//                  Mesh로 옮겨야함.
//void DisplayContent(FbxScene* pScene)
//{
//    int i;
//    FbxNode* lNode = pScene->GetRootNode();
//    // 루트 노드와 그 차일드.
//
//    if (lNode)
//    {
//        for (i = 0; i < lNode->GetChildCount(); i++)
//        {
//            DisplayContent(lNode->GetChild(i));
//        }
//    }
//}
//
//void DisplayContent(FbxNode* pNode)
//{
//    FbxNodeAttribute::EType lAttributeType;
//    int i;
//
//    if (pNode->GetNodeAttribute() == NULL)
//    {
//        //FBXSDK_printf("NULL Node Attribute\n\n");
//    }
//    else
//    {
//        lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());
//
//        switch (lAttributeType)
//        {
//        default:
//            break;
//        case FbxNodeAttribute::eMarker:
//            //DisplayMarker(pNode);
//            break;
//
//        case FbxNodeAttribute::eSkeleton:
//            //DisplaySkeleton(pNode);
//            break;
//
//        case FbxNodeAttribute::eMesh:
//            DisplayMesh(pNode);
//            break;
//
//        case FbxNodeAttribute::eNurbs:
//            //DisplayNurb(pNode);
//            break;
//
//        case FbxNodeAttribute::ePatch:
//            //DisplayPatch(pNode);
//            break;
//
//        case FbxNodeAttribute::eCamera:
//            //DisplayCamera(pNode);
//            break;
//
//        case FbxNodeAttribute::eLight:
//            //DisplayLight(pNode);
//            break;
//
//        case FbxNodeAttribute::eLODGroup:
//            //DisplayLodGroup(pNode);
//            break;
//        }
//    }
//
//    //DisplayUserProperties(pNode);
//    //DisplayTarget(pNode);
//    //DisplayPivotsAndLimits(pNode);
//    //DisplayTransformPropagation(pNode);
//    //DisplayGeometricTransform(pNode);
//
//    for (i = 0; i < pNode->GetChildCount(); i++)
//    {
//        DisplayContent(pNode->GetChild(i));
//    }
//}
//
//void DisplayMesh(FbxNode* pNode)
//{
//    FbxMesh* lMesh = (FbxMesh*)pNode->GetNodeAttribute();
//
//    std::string rudfh = "C:\\Users\\yyyyw\\Desktop\\asdf\\";	// 파일 생성 위치.
//    std::string MeshName = (char*)pNode->GetName();
//    std::string type = ".txt";
//
//    rudfh += MeshName;
//    rudfh += type;
//
//    std::ofstream out{ rudfh };
//
//    //DisplayMetaDataConnections(lMesh);
//
//    //-----------------------------------------------------------------------------------
//    //DisplayControlsPoints(lMesh);
//    int i, lControlPointsCount = lMesh->GetControlPointsCount();
//    FbxVector4* lControlPoints = lMesh->GetControlPoints();
//
//    //DisplayString("    Control Points");
//    //out.write((const char*)&lControlPointsCount, sizeof(int));
//    out << lControlPointsCount << " ";
//    for (i = 0; i < lControlPointsCount; i++)
//    {
//        out << lControlPoints[i][0] << " " << lControlPoints[i][1] << " " << lControlPoints[i][2] << " ";
//
//        /*for (int j = 0; j < lMesh->GetElementNormalCount(); j++)
//        {
//            FbxGeometryElementNormal* leNormals = lMesh->GetElementNormal(j);
//            if (leNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint)
//            {
//                char header[100];
//                FBXSDK_sprintf(header, 100, "            Normal Vector: ");
//                if (leNormals->GetReferenceMode() == FbxGeometryElement::eDirect)
//                    Display3DVector(header, leNormals->GetDirectArray().GetAt(i));
//            }
//        }*/
//    }
//    out << std::endl;
//    //DisplayString("");
//    //-----------------------------------------------------------------------------------
//
//    //DisplayPolygons(lMesh);
//    int j, lPolygonCount = lMesh->GetPolygonCount();
//    out << lPolygonCount << " ";
//    for (i = 0; i < lPolygonCount; i++)
//    {
//        int lPolygonSize = lMesh->GetPolygonSize(i);
//        for (j = 0; j < lPolygonSize; j++)
//        {
//            int lControlPointIndex = lMesh->GetPolygonVertex(i, j);
//            out << lControlPointIndex << " ";
//        }
//
//    }
//
//    //DisplayMaterialMapping(lMesh);
//    //DisplayMaterial(lMesh);
//    //DisplayTexture(lMesh);
//    //DisplayMaterialConnections(lMesh);
//    
//    //DisplayLink(lMesh);
//    
//    //DisplayShape(lMesh);
//
//    //DisplayCache(lMesh);
//}
// 
//-------------------------------------------------------------------------------------------
bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
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
    FbxImporter* lImporter = FbxImporter::Create(pManager, "");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if (!lImportStatus)
    {
        FbxString error = lImporter->GetStatus().GetErrorString();
        /*FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
        FBXSDK_printf("Error returned: %s\n\n", error.Buffer());*/

        if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
        {
            /*FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);*/
        }

        return false;
    }

    //FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        //FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

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
    lStatus = lImporter->Import(pScene);
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

        lStatus = lImporter->Import(pScene);

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