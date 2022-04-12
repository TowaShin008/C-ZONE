#include "FbxLoader.h"
#include<cassert>

using namespace DirectX;

const std::string FbxLoader::baseDirectory = "Resources/";
const std::string FbxLoader::defaultTextureFileName = "lightblue1x1.png";

FbxLoader* FbxLoader::GetInstance()
{
    static FbxLoader instance;
    return &instance;
}

void FbxLoader::Create(ID3D12Device* device)
{
    assert(fbxManager == nullptr);

    this->device = device;

    fbxManager = FbxManager::Create();

    FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
    fbxManager->SetIOSettings(ios);

    fbximporter = FbxImporter::Create(fbxManager, "");
}

void FbxLoader::Finalize()
{
    fbximporter->Destroy();
    fbxManager->Destroy();
}

Model* FbxLoader::LoadModelFromFile(const string& modelName)
{
    const string directoryPath = baseDirectory + modelName + "/";

    const string fileName = modelName + ".fbx";

    const string fullpath = directoryPath + fileName;

    if (!fbximporter->Initialize(fullpath.c_str(), -1, fbxManager->GetIOSettings()))
    {
        assert(0);
    }

    FbxScene* fbxScene = FbxScene::Create(fbxManager, "fbxScene");

    fbximporter->Import(fbxScene);

    Model* model = new Model();
    model->name = modelName;

    int nodeCount = fbxScene->GetNodeCount();

    model->nodes.reserve(nodeCount);

    ParseNodeRecursive(model, fbxScene->GetRootNode());

    //fbxScene->Destroy();
    model->fbxScene = fbxScene;

    model->CreateBuffers(device);

    return model;
}

void FbxLoader::ParseNodeRecursive(Model* model, FbxNode* fbxNode,Node* parent)
{
    string name = fbxNode->GetName();


    model->nodes.emplace_back();
    Node& node = model->nodes.back();

    node.name = fbxNode->GetName();

    FbxDouble3 rotation = fbxNode->LclRotation.Get();
    FbxDouble3 scaling = fbxNode->LclScaling.Get();
    FbxDouble3 translation = fbxNode->LclTranslation.Get();
    //各情報の格納
    node.rotation    = {    (float)rotation[0],   (float)rotation[1],   (float)rotation[2],0.0f };
    node.scaleing    = {     (float)scaling[0],    (float)scaling[1],    (float)scaling[2],0.0f };
    node.translation = { (float)translation[0],(float)translation[1],(float)translation[2],1.0f };

    XMMATRIX matScaling, matRotation, matTranslation;
    matScaling = XMMatrixScalingFromVector(node.scaleing);
    matRotation = XMMatrixRotationRollPitchYawFromVector(node.rotation);
    matTranslation = XMMatrixTranslationFromVector(node.translation);

    node.transform = XMMatrixIdentity();
    node.transform *= matScaling;
    node.transform *= matRotation;
    node.transform *= matTranslation;

    node.globalTransform = node.transform;
    if (parent)
    {
        node.parent = parent;

        node.globalTransform *= parent->globalTransform;
    }

    FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

    if (fbxNodeAttribute) {
        if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
            model->meshNode = &node;
            ParseMesh(model, fbxNode);
        }
    }

    for (int i = 0; i < fbxNode->GetChildCount(); i++)
    {
        ParseNodeRecursive(model, fbxNode->GetChild(i), &node);
    }
}

void FbxLoader::ParseMesh(Model* model, FbxNode* fbxNode)
{
    //ノードのメッシュを取得
    FbxMesh* fbxMesh = fbxNode->GetMesh();
    //頂点座標読み取り
    ParseMeshVertices(model, fbxMesh);
    //面を構成するデータの読み取り
    ParseMeshFaces(model, fbxMesh);
    //マテリアルの読み取り
    ParseMaterial(model, fbxNode);
    //スキニング情報の読み取り
    ParseSkin(model, fbxMesh);
}

void FbxLoader::ParseMeshVertices(Model* model, FbxMesh* fbxMesh)
{
    auto& vertices = model->vertices;

    //頂点座標データの数
    const int controlPointsCount = fbxMesh->GetControlPointsCount();
    //必要数だけ頂点データ配列を確保
    Model::VertexPosNormalUvSkin vert{};
    model->vertices.resize(controlPointsCount, vert);

    //FBXメッシュの頂点座標配列を取得
    FbxVector4* pCoord = fbxMesh->GetControlPoints();

    for (int i = 0; i < controlPointsCount; i++)
    {
        Model::VertexPosNormalUvSkin & vertex = vertices[i];

        vertex.pos.x = (float)pCoord[i][0];
        vertex.pos.y = (float)pCoord[i][1];
        vertex.pos.z = (float)pCoord[i][2];
    }
}

void FbxLoader::ParseMeshFaces(Model* model, FbxMesh* fbxMesh)
{
    auto& vertices = model->vertices;
    auto& indices = model->indices;

    //1ファイルに複数メッシュのモデルは非対応
    assert(indices.size() == 0);
    //面の数
    const int polygonCount = fbxMesh->GetPolygonCount();
    //UVデータの数
    const int textureUVCount = fbxMesh->GetTextureUVCount();
    //UV名リスト
    FbxStringList uvNames;
    fbxMesh->GetUVSetNames(uvNames);
    //面ごとの情報読み取り
    for (int i = 0; i < polygonCount; i++)
    {//面を構成する頂点の数を取得(3なら三角形ポリゴン)
        const int polyGonSize = fbxMesh->GetPolygonSize(i);
        assert(polyGonSize <= 4);

        //1頂点ずつ処理
        for (int j = 0; j < polyGonSize; j++)
        {//FBX頂点配列のインデックス
            int index = fbxMesh->GetPolygonVertex(i, j);
            assert(index >= 0);
            //頂点法線読み込み
            Model::VertexPosNormalUvSkin& vertex = vertices[index];
            FbxVector4 normal;
            if (fbxMesh->GetPolygonVertexNormal(i, j, normal))
            {
                vertex.normal.x = (float)normal[0];
                vertex.normal.y = (float)normal[1];
                vertex.normal.z = (float)normal[2];
            }
            //テクスチャUV読み込み
            if (textureUVCount > 0)
            {
                FbxVector2 uvs;
                bool lUnmappedUV;

                //0番決め打ちで読み込み
                if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV))
                {
                    vertex.uv.x = (float)uvs[0];
                    vertex.uv.y = (float)uvs[1];
                }
            }

            //インデックス配列に頂点インデックス追加
            //3頂点目までなら
            if (j < 3)
            {
                //1点追加し他の2テント三角形を構築する
                indices.push_back(index);
            }//4頂点目
            else
            {
                //3頂点追加し
                //四角形の0,1,2,3,の内2,3,0で三角形を構築する
                int index2 = indices[indices.size() - 1];
                int index3 = index;
                int index0 = indices[indices.size() - 3];
                indices.push_back(index2);
                indices.push_back(index3);
                indices.push_back(index0);
            }
        }
    }
}

void FbxLoader::ParseSkin(Model* model, FbxMesh* fbxMesh)
{
    //スキニング情報
    FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
    //スキニング情報がなければ終了
    if (fbxSkin == nullptr)
    {
        for (int i = 0; i < model->vertices.size(); i++)
        {
            model->vertices[i].boneIndex[0] = 0;
            model->vertices[i].boneWeight[0] = 1.0f;
        }
        return;
    }

    //ボーン配列の参照
    std::vector<Model::Bone>& bones = model->bones;

    //ボーンの数
    int clusterCount = fbxSkin->GetClusterCount();
    bones.reserve(clusterCount);

    //全てのボーンについて
    for (int i = 0; i < clusterCount; i++)
    {
        //FBXボーン情報
        FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

        //ボーン自体のノードの名前を取得
        const char* boneName = fbxCluster->GetLink()->GetName();

        //新しくボーンを追加し、追加したボーンの参照を得る
        bones.emplace_back(Model::Bone(boneName));
        Model::Bone& bone = bones.back();
        //自作ボーンとFBXのボーンを紐付ける
        bone.fbxCluster = fbxCluster;
        //FBXから初期姿勢行列を取得する
        FbxAMatrix fbxMat;
        fbxCluster->GetTransformLinkMatrix(fbxMat);
        //XMMatrix型に変換する
        XMMATRIX initialPose;
        ConvertMatrixFromFbx(&initialPose, fbxMat);
        //初期姿勢行列の逆票列を得る
        bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);
    }

    //ボーン番号とスキンウェイト
    struct WeightSet
    {
        UINT index;
        float weight;
    };

    //二次元配列（ジャグ配列）
    //list:頂点が影響を受けるボーンの全リスト
    //vector:それを全頂点文
    std::vector<std::list<WeightSet>>weightLists(model->vertices.size());

    //全てのボーンについて
    for (int i = 0; i < clusterCount; i++)
    {
        //FBXボーン情報
        FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
        //個のボーンに影響を受ける頂点の数
        int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
        //個のボーンに影響を受ける頂点の配列
        int* controlPointIndices = fbxCluster->GetControlPointIndices();
        double* controlPointWeights = fbxCluster->GetControlPointWeights();

        //影響を受ける全頂点について
        for (int j = 0; j < controlPointIndicesCount; j++)
        {
            //頂点番号
            int vertIndex = controlPointIndices[j];
            //スキンウェイト
            float weight = (float)controlPointWeights[j];
            //その頂点の影響を受けるボーンリストに、ボーンウェイトのペアを追加
            weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
        }
    }
    //頂点配列書き換え用の参照
    auto& vertices = model->vertices;
    //拡張店のウェイトからもっとも大きければtrue　それでなければfalseを返す
    for (int i = 0; i < vertices.size(); i++)
    {
        //頂点のウェイトからもっとも大きい4つを選択
        auto& weightList = weightLists[i];
        //大小比較用のラムダ式を指定して降順にソート
        weightList.sort(
            [](auto const& lhs, auto const& rhs) {
            //左の要素の方が大きければtrue それでなければfalseを返す
            return lhs.weight > rhs.weight;
        });

        int weightArrayIndex = 0;
        //降順ソート済みのウェイトリスト
        for (auto& weightSet : weightList)
        {
            //頂点データ書き込み
            vertices[i].boneIndex[weightArrayIndex] = weightSet.index;
            vertices[i].boneWeight[weightArrayIndex] = weightSet.weight;
            //4つに達したら終了
            if (++weightArrayIndex >= Model::MAX_BONE_INDICES)
            {
                float weight = 0.0f;
                //2番目以降のウェイトを合計
                for (int j = 1; j < Model::MAX_BONE_INDICES; j++)
                {
                    weight += vertices[i].boneWeight[j];
                }
                //合計で1.0f(100%)になるように調整
                vertices[i].boneWeight[0] = 1.0f - weight;
                break;
            }
        }
    }
}

std::string FbxLoader::ExtractFileName(const std::string& path)
{
    size_t pos1;
    //区切り文字'\\'が出てくる一番最後の部分を検索
    pos1 = path.rfind('\\');
    if (pos1 != string::npos)
    {
        return path.substr(pos1 + 1, path.size() - pos1 - 1);
    }
    //区切り文字'/'が出てくる一番最後の部分を検索
    pos1 = path.rfind('/');
    if (pos1 != string::npos)
    {
        return path.substr(pos1 + 1, path.size() - pos1 - 1);
    }

    return path;
}

void FbxLoader::ParseMaterial(Model* model, FbxNode* fbxNode)
{
    const int materialCount = fbxNode->GetMaterialCount();
    if (materialCount > 0)
    {//先頭のマテリアルを取得
        FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
        //テクスチャを読み込んだかどうかを表すフラグ
        bool textureLoaded = false;

        if (material)
        {
            if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
            {
                FbxSurfaceLambert* lambert = static_cast<FbxSurfaceLambert*>(material);
                //環境光係数
                FbxPropertyT<FbxDouble3>ambient = lambert->Ambient;
                model->ambient.x = (float)ambient.Get()[0];
                model->ambient.y = (float)ambient.Get()[1];
                model->ambient.z = (float)ambient.Get()[2];
                //反射光係数
                FbxPropertyT<FbxDouble3>diffuse = lambert->Diffuse;
                model->diffuse.x = (float)diffuse.Get()[0];
                model->diffuse.y = (float)diffuse.Get()[1];
                model->diffuse.z = (float)diffuse.Get()[2];
            }
            const FbxProperty diffuseProperty = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
            if (diffuseProperty.IsValid())
            {
                const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
                if (texture)
                {
                    const char* filepath = texture->GetFileName();
                    //ファイルパスからファイル名抽出
                    string path_str(filepath);
                    string name = ExtractFileName(path_str);
                    //テクスチャ読み込み
                    LoadTexture(model, baseDirectory + model->name + "/" + name);
                    textureLoaded = true;
                }
            }
        }
        //テクスチャがない場合は白テクスチャを貼る
        if (!textureLoaded)
        {
            LoadTexture(model, baseDirectory + defaultTextureFileName);
        }
    }
}

void FbxLoader::LoadTexture(Model* model, const std::string& fullpath)
{
    HRESULT result = S_FALSE;
    //WICテクスチャのロード
    TexMetadata& metadata = model->metadata;
    ScratchImage& scrachImg = model->scratchImg;
    //ユニコード文字列に変換
    wchar_t wfilepath[128];
    MultiByteToWideChar(CP_ACP, 0, fullpath.c_str(), -1, wfilepath,
        _countof(wfilepath));
    result = LoadFromWICFile(
        wfilepath, WIC_FLAGS_NONE,
        &metadata, scrachImg);
    if (FAILED(result))
    {
        assert(0);
    }
}

void FbxLoader::ConvertMatrixFromFbx(DirectX::XMMATRIX* dst, const FbxAMatrix& src)
{
    //行
    for (int i = 0; i < 4; i++)
    {
        //列
        for (int j = 0; j < 4; j++)
        {
            //1要素コピー
            dst->r[i].m128_f32[j] = (float)src.Get(i, j);
        }
    }
}

