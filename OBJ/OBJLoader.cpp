#include "OBJLoader.h"
using namespace std;
OBJLoader* OBJLoader::objLoader = nullptr;

//ローダー生成処理
void OBJLoader::Create()
{//ヌルである場合のみ生成をする
	if (objLoader == nullptr)
	{
		objLoader = new OBJLoader;
	}
}
//ローダー削除処理
void OBJLoader::Destroy()
{
	if (objLoader != nullptr)
	{
		delete objLoader;
	}

	objLoader = nullptr;
}
//読み込んだモデルデータをもとにモデルの実体を作りOBJModel型で返す
OBJModel* OBJLoader::LoadModel(const std::string& modelName)
{
	OBJModel* objModel=new OBJModel();

	CreateCharacter(modelName, objModel);

	objModel->CreateThreeBuffer(device);
	return objModel;
}

void OBJLoader::LoadMaterial(const std::string& directoryPath, const std::string& filename, OBJModel* objModel)
{
	std::ifstream file;
	file.open(directoryPath + filename);//ファイルの名称がここに入る

	string line;

	while (getline(file, line))
	{
		std::istringstream line_stream(line);

		string key;
		getline(line_stream, key, ' ');


		if (key == "newmtl") {
			line_stream >> objModel->material.name;
		}
		if (key == "Ka")
		{
			line_stream >> objModel->material.ambient.x;
			line_stream >> objModel->material.ambient.y;
			line_stream >> objModel->material.ambient.z;
		}
		if (key == "Kd")
		{
			line_stream >> objModel->material.diffuse.x;
			line_stream >> objModel->material.diffuse.y;
			line_stream >> objModel->material.diffuse.z;
		}
		if (key == "Ks")
		{
			line_stream >> objModel->material.specular.x;
			line_stream >> objModel->material.specular.y;
			line_stream >> objModel->material.specular.z;
		}
		if (key == "map_Kd") {
			line_stream >> objModel->material.textureFilename;

		}
	}
	file.close();
}

void OBJLoader::CreateCharacter(const std::string& modelName, OBJModel* objModel)
{
	std::ifstream file;
	const string modelname = modelName;
	const string filename = modelname + ".obj";//"triangle_mat.obj"
	const string directoryPath = "Resources/" + modelname + "/";//"Resources/triangle_mat/"

	//OBJModelクラス内にあるメンバ変数にも代入する
	objModel->modelname = modelname;
	objModel->filename = filename;
	objModel->directoryPath = directoryPath;
	file.open(directoryPath + filename);

	if (file.fail()) {
		assert(0);
		return;
	}

	vector<Vector3>positions;  //頂点座標
	vector<Vector3>normals;    //法線ベクトル
	vector<Vector2>texcoords;  //テクスチャUV
	string line;//読み込んだ一行分の文字列がここに入る
	while (getline(file, line)) {
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		string key;
		getline(line_stream, key, ' ');

		if (key == "v")
		{
			Vector3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			positions.emplace_back(position);
		}
		if (key == "vt") {
			Vector2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.emplace_back(texcoord);
		}
		if (key == "vn") {
			Vector3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			normals.emplace_back(normal);
		}
		if (key == "f")
		{
			int indexCount = 0;
			VertexPosNormalUv vertex_1, vertex_3;
			string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				indexCount++;
				if (indexCount < 4)
				{
					std::istringstream index_stream(index_string);
					unsigned short indexPosition, indexNormal, indexTexcoord;
					index_stream >> indexPosition;

					index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
					index_stream >> indexNormal;


					VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];

					if (indexCount == 1)
					{
						vertex_1 = vertex;
					}
					else if (indexCount == 3)
					{
						vertex_3 = vertex;
					}
					objModel->vertices.emplace_back(vertex);
					objModel->indices.emplace_back(objModel->indices.size());
				}
				else if (indexCount == 4)
				{
					std::istringstream index_stream(index_string);
					unsigned short indexPosition, indexNormal, indexTexcoord;
					index_stream >> indexPosition;

					index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
					index_stream >> indexNormal;


					VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];

					objModel->vertices.emplace_back(vertex);
					objModel->indices.emplace_back(objModel->indices.size());


					objModel->vertices.emplace_back(vertex_1);
					objModel->indices.emplace_back(objModel->indices.size());


					objModel->vertices.emplace_back(vertex_3);
					objModel->indices.emplace_back(objModel->indices.size());
				}
			}
		}
		if (key == "mtllib")
		{
			string filename;
			line_stream >> filename;
			LoadMaterial(directoryPath, filename,objModel);
		}
	}
	file.close();
}