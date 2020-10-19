#include "skinned_mesh.h"
#include "misc.h"
#include "shader.h"
#include "texture.h"

#include <fbxsdk.h>
using namespace fbxsdk;
#include <vector>
#include <functional>

void FbxamatrixToXmfloat4x4(const FbxAMatrix& fbxamatrix, DirectX::XMFLOAT4X4& xmfloat4x4)
{
    for (int row = 0; row < 4; row++)
    {
        for (int column = 0; column < 4; column++)
        {
            xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
        }
    }
}
struct boneInfluence
{
    int index;      // index of bone
    float weight;   // weight of bone
};
typedef std::vector<boneInfluence> boneInfluencesPerControlPoint;
// boneInfluence構造隊の1つのインスタンスは１つ頂点が影響を受けるボーン番号とその重みを表現する
// 1つ頂点は複数のボーンから影響を受ける場合があるので可変長配列で表現している

//================================================
// ボーンの影響を取得する関数
//================================================
void fetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<boneInfluencesPerControlPoint>& influences)
{
    const int numberOfControlPoints = fbxMesh->GetControlPointsCount();
    influences.resize(numberOfControlPoints);

    const int numberOfDeformers = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
    for (int indexOfDeformer = 0; indexOfDeformer < numberOfDeformers; ++indexOfDeformer)
    {
        FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(indexOfDeformer, FbxDeformer::eSkin));

        const int numberOfClusters = skin->GetClusterCount();
        for (int indexOfCluster = 0; indexOfCluster < numberOfClusters; ++indexOfCluster)
        {
            FbxCluster* cluster = skin->GetCluster(indexOfCluster);

            const int numberOfControlPointIndices = cluster->GetControlPointIndicesCount();
            const int* arrayOfCountrolPointIndices = cluster->GetControlPointIndices();
            const double* arrayOfControlPointWeights = cluster->GetControlPointWeights();

            for (int i = 0; i < numberOfControlPointIndices; ++i)
            {
                boneInfluencesPerControlPoint& influecncesPerControlPoint = influences.at(arrayOfCountrolPointIndices[i]);
                boneInfluence influence;
                influence.index = indexOfCluster;
                influence.weight = static_cast<float>(arrayOfControlPointWeights[i]);
                influecncesPerControlPoint.push_back(influence);

            }
        }
    }
}

//==============================================
// ボーン行列の取得する関数
//==============================================
void FetchBoneMatrices(FbxMesh* fbxMesh, std::vector<Skinned_Mesh::bone>& skeletal, FbxTime time)
{
    const int numberOfDeformers = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
    for (int indexOfDeformer = 0; indexOfDeformer < numberOfDeformers; ++indexOfDeformer)
    {
        FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(indexOfDeformer, FbxDeformer::eSkin));

        FbxSkin::EType skinningType = skin->GetSkinningType();
        _ASSERT_EXPR(skinningType == FbxSkin::eLinear || skinningType == FbxSkin::eRigid, L"only support FbxSkin::eLinear or FbxSkin;;eRigid");

        const int numberOfClusters = skin->GetClusterCount();
        skeletal.resize(numberOfClusters);
        for (int indexOfCluster = 0; indexOfCluster < numberOfClusters; ++indexOfCluster)
        {
            Skinned_Mesh::bone& bone = skeletal.at(indexOfCluster);
            FbxCluster* cluster = skin->GetCluster(indexOfCluster);

            // this matrix transforms coordinates of the initial pose from mesh space to global space
            FbxAMatrix referenceGlobalInitPosition;//reference・・・参照
            cluster->GetTransformMatrix(referenceGlobalInitPosition);

            // this matrix transforms coordinates of the initial pose from bone space to global space
            FbxAMatrix clusterGlobalInitPosition;
            cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

            // this matrix transforms coordinates of the current pose from bone space to global space
            FbxAMatrix clusterGlobalCurrentPosition;
            clusterGlobalCurrentPosition = cluster->GetLink()->EvaluateGlobalTransform(time);

            // this matrix transforms coordinates of the current pose from mesh space to global space
            FbxAMatrix referenceGlobalCurrentPosition;
            referenceGlobalCurrentPosition = fbxMesh->GetNode()->EvaluateGlobalTransform(time);

            // Matrices are defined using the Colum major scheme. When a FbxAMatrix represents a transformation
            // (translation, rotation and scale), the last row of the matrix represents the translation part of the transformation.
            FbxAMatrix transform = referenceGlobalCurrentPosition.Inverse() * clusterGlobalCurrentPosition * clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition;

            // convert FbxAMatrix(transform) to XMDLOAR4X4(bone,transform)
            FbxamatrixToXmfloat4x4(transform, bone.transform);
        }
    }
}

//============================================
// アニメーション取得する関数
//============================================
void FetchAnimations(FbxMesh* fbxMesh, std::vector<Skinned_Mesh::skeletalAnimation>& skeletalAnimations, u_int samplingRate = 0)
{
    // Get the list of all the animetion stack. // すべてのアニメのスタックのリストを取得します。
    FbxArray<FbxString*>arrayOfAnimationStackNames;
    fbxMesh->GetScene()->FillAnimStackNameArray(arrayOfAnimationStackNames);

    // Get the number of animetions.
    int numberOfAnimations = arrayOfAnimationStackNames.Size();

    skeletalAnimations.resize(numberOfAnimations);

    //if (numberOfAnimations > 0)
    for (int indexOfAnimation = 0; indexOfAnimation < numberOfAnimations; indexOfAnimation++)
    {
        Skinned_Mesh::skeletalAnimation& skeletalAnimation = skeletalAnimations.at(indexOfAnimation);

        // Get the FbxTime per animation's frame.
        FbxTime::EMode timeMode = fbxMesh->GetScene()->GetGlobalSettings().GetTimeMode();
        FbxTime frameTime;
        frameTime.SetTime(0, 0, 0, 1, 0, timeMode);

        samplingRate = samplingRate > 0 ? samplingRate : static_cast<u_int>(frameTime.GetFrameRate(timeMode));
        float samplingTime = 1.0f / samplingRate;
        skeletalAnimation.samplingTime = samplingTime;
        skeletalAnimation.animationTick = 0.0f;

        FbxString* animationStackName = arrayOfAnimationStackNames.GetAt(indexOfAnimation);
        FbxAnimStack* currentAnimationStack = fbxMesh->GetScene()->FindMember<FbxAnimStack>(animationStackName->Buffer());
        fbxMesh->GetScene()->SetCurrentAnimationStack(currentAnimationStack);

        FbxTakeInfo* takeInfo = fbxMesh->GetScene()->GetTakeInfo(animationStackName->Buffer());
        _ASSERT_EXPR(takeInfo, L"must need FbxTakeInfo");
        FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
        FbxTime endTime = takeInfo->mLocalTimeSpan.GetStop();

        FbxTime samplingStep;
        samplingStep.SetTime(0, 0, 1, 0, 0, timeMode);
        samplingStep = static_cast<FbxLongLong>(samplingStep.Get() * samplingTime);

        for (FbxTime currentTime = startTime; currentTime < endTime; currentTime += samplingStep)
        {
            Skinned_Mesh::skeletal skeletal;
            FetchBoneMatrices(fbxMesh, skeletal, currentTime);
            skeletalAnimation.push_back(skeletal);
        }
    }
    for (int i = 0; i < numberOfAnimations; i++)
    {
        delete arrayOfAnimationStackNames[i];
    }
    // sampling_rate は 1 秒あたりにサンプリングする回数、０の場合は FBX データのアニメーションフレームレートを使う
    // 完成後 sampling_rate を変えてアニメーションの変化を確認する

}

Skinned_Mesh::Skinned_Mesh(ID3D11Device* device, const char* fbxFilename,bool clockFlg)
{

    //FbxManagerとFbxSceneオブジェクトを作成
    // Create the FBX SDK manager
    // FBXSDKを使うにあたって最初にmanagerオブジェクトを作成します
    // FbxIOSettingsオブジェクトを作成し、managerにセットする
    // これはimport/export処理に関わる部分である。

    FbxManager* manager = FbxManager::Create();

    // Create anIOSetting object. IOSROOT is defined in Fbxiosettingspath h
    manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

    // Create a new scene so it can be populated by the imported file.
    FbxScene* scene = FbxScene::Create(manager, "");

    // データインポート
    // Create an importer.
    // ファイル名などを格納し初期化しsceneにインポートする
    // そのあとは削除する
    FbxImporter* importer = FbxImporter::Create(manager, "");

    // Initialize the importer.
    bool importStatus = false;
    importStatus = importer->Initialize(fbxFilename, -1, manager->GetIOSettings());
    if (!importStatus)
    {
        //_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());
        assert(importer->GetStatus().GetErrorString());
        return;
    }
    // Import the contents of the file into the scene
    importStatus = importer->Import(scene);
    //_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

    //三角ポリゴン化
    // Convert mesh, NURBS and patch into triangle mesh
    // 読み込んだメッシュデータには三角形と四角形が混在している場合が多い
    // そのため扱いやすいようにすべてを三角ポリゴンに変換する
    fbxsdk::FbxGeometryConverter geometryConverter(manager);
    geometryConverter.Triangulate(scene, /*replace*/true); // 置き換え

    // nodeを再帰的に巡っていく
    // nodeとは、Meshが中に入っておりモデルパーツごとにnodeが生成される
    // 例えば、node1には顔データ、node2には腕、node3には服・・・
    // と情報が分けられて入れらている
    // Fetch node attributes and materials under this node recursively. Currently only mesh.
    std::vector<FbxNode*> fetchedMeshes;
    std::function<void(FbxNode*)> traverse = [&](FbxNode* node)// 関数を変数に保持する。 =の右側が第一引数ありの関数？
    {
        if (node)
        {
            FbxNodeAttribute* fbxNodeAttribute = node->GetNodeAttribute();
            if (fbxNodeAttribute)
            {
                switch (fbxNodeAttribute->GetAttributeType())
                {
                case FbxNodeAttribute::eMesh:
                    fetchedMeshes.push_back(node);
                    break;
                }
            }
            for (int i = 0; i < node->GetChildCount(); i++)
            {
                // 保持している関数を呼び出す
                traverse(node->GetChild(i));// traverse変数が関数の役割を持っている
            }
        }
    };
    traverse(scene->GetRootNode());

    meshes.resize(fetchedMeshes.size());

    //FbxMesh* fbxMesh = fetchedMeshes.at(0)->GetMesh(); // Currently only one mesh.
    for (size_t i = 0; i < fetchedMeshes.size(); i++)
    {
        // このfor文の中で使う変数宣言
        FbxMesh* fbxMesh = fetchedMeshes.at(i)->GetMesh();
        mesh& mesh = meshes.at(i); // 各メッシュの要素への参照を格納

        FbxAMatrix globalTransform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
        FbxamatrixToXmfloat4x4(globalTransform, mesh.globalTransform);

        //========================================================
        // Fetch material properties.
        // マテリアル情報を取得
        //========================================================
        const int numberOfMaterials = fbxMesh->GetNode()->GetMaterialCount();
        //subsets.resize(numberOfMaterials);
        //subsets.resize(numberOfMaterials > 0 ? numberOfMaterials : 1);
        mesh.subsets.resize(numberOfMaterials > 0 ? numberOfMaterials : 1);

        for (int indexOfMaterial = 0; indexOfMaterial < numberOfMaterials; ++indexOfMaterial)
        {
            //subset& subset = subsets.at(indexOfMaterial);
            subset& subset = mesh.subsets.at(indexOfMaterial);


            const FbxSurfaceMaterial* surfaceMaterial = fbxMesh->GetNode()->GetMaterial(indexOfMaterial);// マテリアルの面(表面)を格納

            const FbxProperty property = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);  // マテリアルの特性を格納
            const FbxProperty factor = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sDiffuseFactor); // マテリアルの因子を格納
            if (property.IsValid() && factor.IsValid())
            {
                FbxDouble3 color = property.Get<FbxDouble3>();
                double f = factor.Get<FbxDouble>();
                subset.material.color.x = static_cast<float>(color[0] * f);
                subset.material.color.y = static_cast<float>(color[1] * f);
                subset.material.color.z = static_cast<float>(color[2] * f);
                subset.material.color.w = 1.0f;
            }
            if (property.IsValid())
            {
                const int numberOfTexture = property.GetSrcObjectCount<FbxFileTexture>();
                if (numberOfTexture)
                {
                    const FbxFileTexture* fileTexture = property.GetSrcObject<FbxFileTexture>();
                    if (fileTexture)
                    {
                        const char* filename = fileTexture->GetRelativeFileName();
                        // Create"diffuse.shaderResourceView" from"filename".
                        //wchar_t textureUnicode[256];
                        //MultiByteToWideChar(CP_ACP, 0, filename, strlen(filename) + 1, textureUnicode, 1024);
                        //D3D11_TEXTURE2D_DESC texture2dDesc;
                        //LoadTextureFromFile(device,textureUnicode,material.shaderResourceView.GetAddressOf(),&texture2dDesc);

                        wchar_t fbxUnicode[256];
                        MultiByteToWideChar(CP_ACP, 0, fbxFilename, strlen(fbxFilename) + 1, fbxUnicode, 1024);
                        wchar_t textureUnicode[256];
                        MultiByteToWideChar(CP_ACP, 0, fileTexture->GetFileName(), strlen(fileTexture->GetFileName()) + 1, textureUnicode, 1024);
                        SetDirectoryPath(textureUnicode, fbxUnicode, textureUnicode);

                        //material.texture_filename = textureUnicode;
                        D3D11_TEXTURE2D_DESC texture2d_desc;
                        LoadTextureFromFile(device, textureUnicode, subset.material.shaderResourceView.GetAddressOf(), &texture2d_desc);
                    }
                }
            }
        }
        for (subset& subset : mesh.subsets)
        {
            if (!subset.material.shaderResourceView)
            {
                MakeDummyTexture(device, subset.material.shaderResourceView.GetAddressOf());
            }
        }
        // Count the polygon count of each material
        // 各マテリアルのポリゴン数を数えていく
        if (numberOfMaterials > 0)
        {
            // Count the polygon count of each material
            const int numberOfPolygon = fbxMesh->GetPolygonCount();
            for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygon; ++indexOfPolygon)
            {
                const u_int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);
                mesh.subsets.at(materialIndex).indexCount += 3;
            }

            // Record the offset (how many vertex)
            int offset = 0;
            for (subset& subset : mesh.subsets)
            {
                subset.indexStart = offset;
                offset += subset.indexCount;
                // This will be used as counter in the following procedures. reset to zero
                subset.indexCount = 0;
            }
        }

        //===========================================================
        // 頂点インデックスと頂点座標,UV座標を取得する
        // Fetch mesh data 
        // mesh情報を取得
        //===========================================================
        std::vector<vertex> vertices;   // VertexBuffer
        std::vector<u_int>  indices;    // IndexBuffer
        u_int vertexCount = 0;

        FbxStringList uvNames;
        fbxMesh->GetUVSetNames(uvNames);

        const FbxVector4* arrayOfControlPoints = fbxMesh->GetControlPoints();
        const int numberOfPolygons = fbxMesh->GetPolygonCount();

        indices.resize(numberOfPolygons * 3);

        // boneInfluenceを取得
        std::vector<boneInfluencesPerControlPoint> boneInfluences;
        fetchBoneInfluences(fbxMesh, boneInfluences);

        bool allByControlPoint = true;
        bool hasNormal = fbxMesh->GetElementNormalCount() > 0;
        bool hasUv = fbxMesh->GetElementUVCount() > 0;
        if (hasNormal && fbxMesh->GetElementNormal(0)->GetMappingMode() != FbxGeometryElement::EMappingMode::eByControlPoint)
        {
            allByControlPoint = false;
        }
        if (hasUv && fbxMesh->GetElementUV(0)->GetMappingMode() != FbxGeometryElement::EMappingMode::eByControlPoint)
        {
            allByControlPoint = false;
        }
        if (allByControlPoint)
        {
            const FbxGeometryElementNormal* geometryElementNormal = fbxMesh->GetElementNormal(0);
            const FbxGeometryElementUV* geometryElementUv = fbxMesh->GetElementUV(0);
            const int numberOfControlPoints = fbxMesh->GetControlPointsCount();
            for (int indexOfControlPoint = 0; indexOfControlPoint < numberOfControlPoints; ++indexOfControlPoint)
            {
                vertex vertex;
                vertex.pos.x = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][0]);
                vertex.pos.y = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][1]);
                vertex.pos.z = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][2]);

                boneInfluencesPerControlPoint influencesPerControlPoint = boneInfluences.at(indexOfControlPoint);
                for (size_t indexOfInfluence = 0; indexOfInfluence < influencesPerControlPoint.size(); indexOfInfluence++)
                {
                    if (indexOfInfluence < MAX_BONE_INFLUNCES)
                    {
                        vertex.boneWeights[indexOfInfluence] = influencesPerControlPoint.at(indexOfInfluence).weight;
                        vertex.boneIndices[indexOfInfluence] = influencesPerControlPoint.at(indexOfInfluence).index;
                    }
                    else
                    {
                        for (int i = 0; i < MAX_BONE_INFLUNCES; i++)
                        {
                            if (vertex.boneWeights[i], influencesPerControlPoint.at(indexOfInfluence).weight)
                            {
                                vertex.boneWeights[i] += influencesPerControlPoint.at(indexOfInfluence).weight;
                                vertex.boneIndices[i] = influencesPerControlPoint.at(indexOfInfluence).index;
                                break;
                            }
                        }
                    }

                }

                if (hasNormal)
                {
                    int i = indexOfControlPoint;
                    if (geometryElementNormal->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
                    {
                        i = geometryElementNormal->GetIndexArray().GetAt(indexOfControlPoint);
                    }
                    vertex.normal.x = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(i)[0]);
                    vertex.normal.y = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(i)[1]);
                    vertex.normal.z = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(i)[2]);
                }

                if (hasUv)
                {
                    int i = indexOfControlPoint;
                    if (geometryElementUv->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
                    {
                        i = geometryElementUv->GetIndexArray().GetAt(indexOfControlPoint);
                    }
                    vertex.texcoord.x = static_cast<float>(geometryElementUv->GetDirectArray().GetAt(i)[0]);
                    vertex.texcoord.y = static_cast<float>(geometryElementUv->GetDirectArray().GetAt(i)[1]);
                }
                vertices.push_back(vertex);
            }
        }

        ////////////////////////
        // ポリゴン情報格納
        ////////////////////////
        for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; indexOfPolygon++)
        {
            // The material for current face.
            int indexOfMaterial = 0;
            if (numberOfMaterials > 0)
            {
                indexOfMaterial = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);
            }
            // Where should I save the vertex attribute index, according to the material
            subset& subset = mesh.subsets.at(indexOfMaterial);
            const int indexOffset = subset.indexStart + subset.indexCount;

            for (int indexOfVertex = 0; indexOfVertex < 3; indexOfVertex++)
            {
                const int indexOfControlPoint = fbxMesh->GetPolygonVertex(indexOfPolygon, indexOfVertex);
                if (allByControlPoint)
                {
                    indices.at(indexOffset + indexOfVertex) = static_cast<unsigned int>(indexOfControlPoint);
                }
                else
                {
                    vertex vertex;
                    const int indexOfControlPoint = fbxMesh->GetPolygonVertex(indexOfPolygon, indexOfVertex);
                    vertex.pos.x = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][0]);
                    vertex.pos.y = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][1]);
                    vertex.pos.z = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][2]);

                    boneInfluencesPerControlPoint influencesPerControlPoint = boneInfluences.at(indexOfControlPoint);
                    for (size_t indexOfInfluence = 0; indexOfInfluence < influencesPerControlPoint.size(); indexOfInfluence++)
                    {
                        if (indexOfInfluence < MAX_BONE_INFLUNCES)
                        {
                            vertex.boneWeights[indexOfInfluence] = influencesPerControlPoint.at(indexOfInfluence).weight;
                            vertex.boneIndices[indexOfInfluence] = influencesPerControlPoint.at(indexOfInfluence).index;
                        }
                        else
                        {
                            for (int i = 0; i < MAX_BONE_INFLUNCES; i++)
                            {
                                if (vertex.boneWeights[i] < influencesPerControlPoint.at(indexOfInfluence).weight)
                                {
                                    //vertex.boneWeights[i] = influencesPerControlPoint.at(indexOfInfluence).weight;
                                    vertex.boneWeights[i] += influencesPerControlPoint.at(indexOfInfluence).weight;
                                    vertex.boneIndices[i] = influencesPerControlPoint.at(indexOfInfluence).index;
                                }
                            }
                        }
                    }


                    // 法線・テクスチャ座標を持っているかのチェック
                    //if (fbxMesh->GetElementNormalCount() > 0)
                    if (hasNormal)
                    {
                        FbxVector4 normal;
                        fbxMesh->GetPolygonVertexNormal(indexOfPolygon, indexOfVertex, normal);
                        vertex.normal.x = static_cast<float>(normal[0]);
                        vertex.normal.y = static_cast<float>(normal[1]);
                        vertex.normal.z = static_cast<float>(normal[2]);
                    }
                    //if (fbxMesh->GetElementUVCount() > 0)
                    if (hasUv)
                    {
                        FbxVector2 uv;
                        bool unmappedUv;
                        fbxMesh->GetPolygonVertexUV(indexOfPolygon, indexOfVertex, uvNames[0], uv, unmappedUv);
                        vertex.texcoord.x = static_cast<float>(uv[0]);
                        vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
                    }

                    vertices.push_back(vertex);


                    //indices.push_back(vertexCount);
                    indices.at(indexOffset + indexOfVertex) = static_cast<u_int>(vertexCount);
                }
                vertexCount += 1;
            }
            subset.indexCount += 3;
        }
        //FbxTime::EMode timeMode = fbxMesh->GetScene()->GetGlobalSettings().GetTimeMode();
        //FbxTime frameTime;
        //frameTime.SetTime(0, 0, 0, 1, 0, timeMode);
        //FetchBoneMatrices(fbxMesh, mesh.skeletal, frameTime * 20); // pose at frame 20
        FetchAnimations(fbxMesh, mesh.skeletalAnimation);


        mesh.CreateBuffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
    }
    manager->Destroy();

    // Initialize Direct3D COM objects.

    HRESULT hr = S_OK;

    // 入力レイアウトオブジェックト生成
    D3D11_INPUT_ELEMENT_DESC input_element_desc[] = {
        { "POSITION",0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL" , 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BONES" ,  0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    createVsFromCso(device, "skinned_not_light_vs.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
    createPsFromCso(device, "skinned_not_light_ps.cso", pixelShader.GetAddressOf());

    // create rasterizer state : solid mode
    {
        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT,D3D11_CULL_BACKがある。用途によって使い分けよ
        rasterizerDesc.FrontCounterClockwise = clockFlg;
        rasterizerDesc.DepthBias = 0;
        rasterizerDesc.DepthBiasClamp = 0;
        rasterizerDesc.SlopeScaledDepthBias = 0;
        rasterizerDesc.DepthClipEnable = TRUE;
        rasterizerDesc.ScissorEnable = FALSE;
        rasterizerDesc.MultisampleEnable = FALSE;
        rasterizerDesc.AntialiasedLineEnable = FALSE;
        hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState[0].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    // create rasterizer state : wireframe mode
    {
        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
        rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
        rasterizerDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT,D3D11_CULL_BACKがある。用途によって使い分けよ
        rasterizerDesc.FrontCounterClockwise = clockFlg;
        rasterizerDesc.DepthBias = 0;
        rasterizerDesc.DepthBiasClamp = 0;
        rasterizerDesc.SlopeScaledDepthBias = 0;
        rasterizerDesc.DepthClipEnable = TRUE;
        rasterizerDesc.ScissorEnable = FALSE;
        rasterizerDesc.MultisampleEnable = FALSE;
        rasterizerDesc.AntialiasedLineEnable = FALSE;
        hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerState[1].GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    // create depth stencil state
    {
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        depthStencilDesc.DepthEnable = TRUE;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilDesc.StencilEnable = FALSE;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = 0xFF;
        depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    // create constant buffer
    {
        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.ByteWidth = sizeof(cbuffer);
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;
        hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // create sampler state
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0;
    samplerDesc.MaxAnisotropy = 16;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    //memcpy(samplerDesc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}
void Skinned_Mesh::mesh::CreateBuffers(ID3D11Device* device, vertex* vertices, int numVertices, u_int* indices, int numIndices)
{
    HRESULT hr;
    {
        D3D11_BUFFER_DESC bufferDesc;
        D3D11_SUBRESOURCE_DATA subResourceData;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        ZeroMemory(&subResourceData, sizeof(subResourceData));

        bufferDesc.ByteWidth = sizeof(vertex) * numVertices;
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        subResourceData.pSysMem = vertices;
        subResourceData.SysMemPitch = 0; // Not use for vertex buffers.
        subResourceData.SysMemSlicePitch = 0; // Not use for vertex buffers.

        hr = device->CreateBuffer(&bufferDesc, &subResourceData, vertexBuffer.ReleaseAndGetAddressOf());

        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }

    // create indexbuffer
    {
        D3D11_BUFFER_DESC bufferDesc;
        D3D11_SUBRESOURCE_DATA subResourceData;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        ZeroMemory(&subResourceData, sizeof(subResourceData));

        bufferDesc.ByteWidth = sizeof(u_int) * numIndices;
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;
        subResourceData.pSysMem = indices;
        subResourceData.SysMemPitch = 0; //Not use for index buffers.
        subResourceData.SysMemSlicePitch = 0; //Not use for index buffers.

        //num_indices = numIndices;
        hr = device->CreateBuffer(&bufferDesc, &subResourceData, indexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
}
#ifndef DEBUG
void Skinned_Mesh::CreateBuffers(ID3D11Device* device, vertex* vertices, int numVertices, u_int* indices, int numIndices)
{
    HRESULT hr;
    {
        D3D11_BUFFER_DESC bufferDesc;
        D3D11_SUBRESOURCE_DATA subResourceData;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        ZeroMemory(&subResourceData, sizeof(subResourceData));

        bufferDesc.ByteWidth = sizeof(vertex) * numVertices;
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;

        subResourceData.pSysMem = vertices;
        subResourceData.SysMemPitch = 0; // Not use for vertex buffers.
        subResourceData.SysMemSlicePitch = 0; // Not use for vertex buffers.

        hr = device->CreateBuffer(&bufferDesc, &subResourceData, vertexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
    // create indexbuffer
    {
        D3D11_BUFFER_DESC bufferDesc;
        D3D11_SUBRESOURCE_DATA subResourceData;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        ZeroMemory(&subResourceData, sizeof(subResourceData));

        bufferDesc.ByteWidth = sizeof(u_int) * numIndices;
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0;
        bufferDesc.MiscFlags = 0;
        bufferDesc.StructureByteStride = 0;
        subResourceData.pSysMem = indices;
        subResourceData.SysMemPitch = 0; //Not use for index buffers.
        subResourceData.SysMemSlicePitch = 0; //Not use for index buffers.

        num_indices = numIndices;
        hr = device->CreateBuffer(&bufferDesc, &subResourceData, indexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
    }
}
#endif // !debug




void Skinned_Mesh::render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4 lightDirection, const DirectX::XMFLOAT4 materialColor, bool wireframe, float elapsedTime, size_t animation)
{
    for (mesh& mesh : meshes)
    {
        UINT stride = sizeof(vertex);   // 格納する側の初期化
        UINT offset = 0;                // 読込みの位置

        // 頂点バッファのセット
        //context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
        // インデックスバッファのセット
        //context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        // プリミティブトポロジのセット
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 入力レイアウトのセット
        context->IASetInputLayout(inputLayout.Get());

        // シェーダーのセット
        context->VSSetShader(vertexShader.Get(), nullptr, 0);
        context->PSSetShader(pixelShader.Get(), nullptr, 0);

        context->OMSetDepthStencilState(depthStencilState.Get(), 1);

        // ラスタライザーのセット
        if (wireframe)
        {
            context->RSSetState(rasterizerState[1].Get());
        }
        else context->RSSetState(rasterizerState[0].Get());

        // 定数バッファのセット
        cbuffer cBuffer;

        DirectX::XMStoreFloat4x4(&cBuffer.worldViewProjection, DirectX::XMLoadFloat4x4(&mesh.globalTransform) /** DirectX::XMLoadFloat4x4(&coordinateConversion) */ * DirectX::XMLoadFloat4x4(&wvp));
        DirectX::XMStoreFloat4x4(&cBuffer.world, DirectX::XMLoadFloat4x4(&mesh.globalTransform) /** DirectX::XMLoadFloat4x4(&coordinateConversion)*/ * DirectX::XMLoadFloat4x4(&world));
        // これによってカリングが逆になったためラスタライザーの設定を変更する

        cBuffer.lightDirection = lightDirection;


        size_t numberOfAnimations = mesh.skeletalAnimation.size();
        //if (mesh.skeletalAnimation.size() > 0)
        if (numberOfAnimations > 0)
        {
            _ASSERT_EXPR(numberOfAnimations > animation, L"animation' is invalid number.");
            skeletalAnimation& skeletalAnimation = mesh.skeletalAnimation.at(animation);

            size_t frame = static_cast<size_t>(skeletalAnimation.animationTick / skeletalAnimation.samplingTime);
            if (frame > skeletalAnimation.size() - 1)
            {
                frame = 0;
                skeletalAnimation.animationTick = 0;
            }
            std::vector<bone>& skeletal = skeletalAnimation.at(frame);
            size_t numberOfBones = skeletal.size();
            _ASSERT_EXPR(numberOfBones < MAX_BONES, L"the numberOfBones' exceeds MAX_BONES.");
            for (size_t i = 0; i < numberOfBones; i++)
            {
                DirectX::XMStoreFloat4x4(&cBuffer.bone_transforms[i], DirectX::XMLoadFloat4x4(&skeletal.at(i).transform));
            }
            skeletalAnimation.animationTick += elapsedTime;

        }

        // マテリアルカラーとシェーダーとサンプラーと描画のセット
        for (subset& subset : mesh.subsets)
        {
            cBuffer.materialColor.x = subset.material.color.x * materialColor.x;
            cBuffer.materialColor.y = subset.material.color.y * materialColor.y;
            cBuffer.materialColor.z = subset.material.color.z * materialColor.z;
            cBuffer.materialColor.w = subset.material.color.w;
            context->UpdateSubresource(constantBuffer.Get(), 0, 0, &cBuffer, 0, 0);
            context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

            context->PSSetShaderResources(0, 1, subset.material.shaderResourceView.GetAddressOf());
            context->PSSetSamplers(0, 1, samplerState.GetAddressOf());
            context->DrawIndexed(subset.indexCount, subset.indexStart, 0);
        }
    }
}

