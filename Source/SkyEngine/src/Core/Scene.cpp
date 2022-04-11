#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Core/GameObject.h"
#include "Render/Model.h"
#include "Render/ShaderProgram.h"
#include "Components/MeshRenderer.h"
#include "Render/Mesh.h"


Scene::Scene() : activeCamera(0)
{
	rootObject = new GameObject();
}



void Scene::Init(const std::string& file)
{
	std::ifstream fileStream(file.c_str());
	json configJson;
	fileStream >> configJson;

	mSceneId = configJson["sceneID"];

	LoadCamera(configJson);

	LoadGameObjs(configJson);
}

void Scene::LoadCamera(const json& configJson)
{
	json cameraSettings = configJson["camera"];

	//float left = (float)cameraSettings["left"];
	//float right = (float)cameraSettings["right"];
	//float bottom = (float)cameraSettings["bottom"];
	//float top = (float)cameraSettings["top"];
	float nearP = (float)cameraSettings["nearPlane"];
	float farP = (float)cameraSettings["farPlane"];
	float fov = (float)cameraSettings["fov"];
	float aspect = (float)cameraSettings["aspect"];

	json position = cameraSettings["position"];
	Vector3 pos = Vector3((float)position["x"], (float)position["y"], (float)position["z"]);

	json target = cameraSettings["target"];
	Vector3 tar = Vector3((float)target["x"], (float)target["y"], (float)target["z"]);

	//activeCamera = new Camera(pos, tar, Vector3::up);
	//camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 10, true);
	//activeCamera->SetFrustrum(left, right, bottom, top, nearP, farP, true);
	
	activeCamera = new Camera(tar, pos, fov, aspect, nearP, farP);
}

vector<float> StrSplitToFloats(const string& str, const string& pattern)
{
	vector<float> res;
	if (str == "")
		return res;
	//在字符串末尾也加入分隔符，方便截取最后一段
	string strs = str + pattern;
	size_t pos = strs.find(pattern);

	while (pos != strs.npos)
	{
		string temp = strs.substr(0, pos);
		res.push_back(std::stof(temp));
		//去掉已分割的字符串,在剩下的字符串中进行分割
		strs = strs.substr(pos + 1, strs.size());
		pos = strs.find(pattern);
	}

	return res;
}


void ParseMaterialDescription(const json& matJson, MaterialDescription* description)
{
	description->name = matJson["name"];
	description->shader = matJson["shader"];
	description->albedoMap = matJson["albedoMap"];
	description->emissiveMap = matJson["emissiveMap"];
	description->metallicRoughnessMap = matJson["metallicRoughnessMap"];
	description->normalMap = matJson["normalMap"];
	description->opacityMap = matJson["opacityMap"];
	description->ambientOcclusionMap = matJson["ambientOcclusionMap"];


	vector<float> color = StrSplitToFloats(matJson["emissiveColor"], ",");
	description->emissiveColor = Vector4(color[0], color[1], color[2], 1);

	color = StrSplitToFloats(matJson["albedoColor"], ",");
	description->albedoColor = Vector4(color[0], color[1], color[2], 1);

	color = StrSplitToFloats(matJson["roughness"], ",");
	description->roughness = Vector4(color[0], color[1], color[2], 1);


	description->metallicFactor = color[1];

	std::string::size_type sz;
	if (description->shader.find("alphatest") != std::string::npos) {
		description->alphaCutoff = std::stof(string(matJson["alphaCutoff"]), &sz); ;
	}
	description->flags = std::stoi(string(matJson["flags"]), &sz);
	description->cullMode = std::stoi(string(matJson["cullMode"]), &sz);
	description->fillMode = std::stoi(string(matJson["fillMode"]), &sz);
	description->zTest = std::stoi(string(matJson["zTest"]), &sz);
	description->zWrite = std::stoi(string(matJson["zWrite"]), &sz);
}

string Scene::GetScenePath()
{
	return "../../Library/" + mSceneId + "/";
}

GameObject* Scene::ParseGameObjInfo(const json& currentGameObj)
{
	json position = currentGameObj["position"];
	json rotation = currentGameObj["rotation"];
	json scaling = currentGameObj["scaling"];

	GameObject* pObj = new GameObject();
	pObj->name = currentGameObj["name"];
	pObj->transform.rotation = Quaternion::Euler((float)rotation["x"], (float)rotation["y"], (float)rotation["z"]);
	pObj->transform.position = Vector3((float)position["x"], (float)position["y"], (float)position["z"]);
	pObj->transform.scale = Vector3((float)scaling["x"], (float)scaling["y"], (float)scaling["z"]);

	json components = currentGameObj["components"];
	for (int i = 0; i < components.size(); i++)
	{
		//if (components.find("meshRenderer") != components.end())
		if (components[i]["name"] == "MeshRenderer")
		{
			json meshRendererProps = components[i]["properties"];
			string meshPath = GetScenePath() + string(meshRendererProps["mesh"]);
			string materialPath = GetScenePath() + string(meshRendererProps["material"]);

			std::ifstream fileStream(materialPath.c_str());
			json materialJson;
			fileStream >> materialJson;

			MaterialDescription* description = new MaterialDescription();
			ParseMaterialDescription(materialJson, description);
			Material* pmat = new Material(*description, mSceneId);

			Mesh* pmesh = new Mesh(meshPath);

			MeshRenderer* mr = new MeshRenderer(pmesh, pmat);
			pObj->AddComponent(mr);
		}
	}

	json childs = currentGameObj["childs"];
	for (int i = 0; i < childs.size(); i++)
	{
		GameObject* child = Scene::ParseGameObjInfo(childs[i]);
		pObj->AddChild(child);
	}

	return pObj;
}

void Scene::LoadGameObjs(const json& configJson)
{
	unsigned int objCount = (unsigned int)configJson["gameobjs"].size();

   	for (unsigned int i = 0; i < objCount; ++i) 
	{
		//get model mesh and material info
		json currentGameObj = configJson["gameobjs"][i];
		GameObject* pObj = ParseGameObjInfo(currentGameObj);
		AddGameObject(pObj);
	}
		

		/*
		string modelPath = currentGameObj["model"];
		Model* centerModel = new Model(modelPath.c_str(), true); 
		center Model->CreateBufferData();

		string matName = currentGameObj["material"];

		Material* pMat = ResourceManager::GetInstance()->FindMaterial(matName);
		
		if (centerModel->ModelType == EModelType::GLTF)
		{
			vector<TfNode>* nodes = centerModel->GetNodes();
			//vector<Mesh>* meshes = centerModel->GetMeshes();
			vector<TfMesh>* meshes = centerModel->GetTfMeshes();
			vector<TfMaterial>* mats = centerModel->GetTfMats();

			for (int j = 0; j < nodes->size(); j++)
			{
				if ((*nodes)[j].MeshIndex != -1)
				{
					TfMesh& mesh = (*meshes)[(*nodes)[j].MeshIndex];
					if (mesh.Primitives.size() > 0)
					{
						for (int k = 0; k < mesh.Primitives.size(); k++)
						{
							Mesh* meshData = mesh.Primitives[k].MeshData;
							GameObject* childObj = new GameObject();
							string objName = pObj->name +mesh.Name + "_primetive_" + to_string(k);
							childObj->name = objName;

							childObj->transform.rotation = (*nodes)[j].LocalTrans.rotation * pObj->transform.rotation;
							childObj->transform.position = (*nodes)[j].LocalTrans.position + pObj->transform.position;
							childObj->transform.scale = (*nodes)[j].LocalTrans.scale * pObj->transform.scale;
							
							TfMaterial mat = (*mats)[mesh.Primitives[k].MaterialIndex];

							ShaderProgram* shader = ResourceManager::GetInstance()->FindShader("PbrOpaque");

							Material* tmpMat = new Material(shader);
							tmpMat->Name = mat.name;
							tmpMat->SetCullMode(CM_Back);

							tmpMat->SetColor("baseColorFactor", mat.baseColorFactor);
							tmpMat->SetFloat("metallicFactor", mat.metallicFactor);
							tmpMat->SetFloat("roughnessFactor", mat.roughnessFactor);
							tmpMat->SetColor("emissiveFactor", mat.emissiveFactor);

							tmpMat->AddTextureVariable(mat.baseColorTexture);
							tmpMat->AddTextureVariable(mat.normalTexture);
							tmpMat->AddTextureVariable(mat.metallicRoughnessTexture);
							tmpMat->AddTextureVariable(mat.occlusionTexture);
							tmpMat->AddTextureVariable(mat.emissiveTexture);

							ResourceManager::GetInstance()->AddMaterial(tmpMat);

							MeshRenderer* mr = new MeshRenderer(meshData, tmpMat);
							childObj->AddComponent(mr);

							pObj->AddChild(childObj);
						}
					}
				}
			}
		}
		else
		{
			vector<Mesh>* meshes = centerModel->GetMeshes();
			for (int j = 0; j < meshes->size(); j++)
			{
				GameObject* childObj = new GameObject();
				childObj->name = pObj->name + "_child_" + to_string(j);

				//Need to Add Transform hirachey support.
				//childObj->transform.rotation = Quaternion::Euler(0, 0, 0);
				//childObj->transform.position = Vector3(0, 0, 0);
				//childObj->transform.scale = Vector3(1, 1, 1);

				childObj->transform.rotation = pObj->transform.rotation;
				childObj->transform.position = pObj->transform.position;
				childObj->transform.scale = pObj->transform.scale;

				MeshRenderer* mr = new MeshRenderer(&(*meshes)[j], pMat);
				childObj->AddComponent(mr);

				pObj->AddChild(childObj);
			}
		}
	}*/
}


void Scene::Start()
{
	this->GetRoot()->Start();
}

void Scene::Update()
{
	this->GetRoot()->Update();
}

GameObject* Scene::GetRoot()
{
	return rootObject;
}



GameObject* Scene::FindByName(const string& name, GameObject* root)
{
	if (root == NULL)
	{
		root = rootObject;
	}

	if (root->GetNumChildren() > 0)
	{
		for (int i = 0; i < root->GetNumChildren(); i++)
		{
			GameObject* obj = FindByName(name, root->GetChild(i));
			if (obj != NULL)
			{
				return obj;
			}
		}
	}

	if (root->name.compare(name) == 0)
	{
		return root;
	}
	else
	{
		return NULL;
	}

}


void Scene::AddGameObject(GameObject* gameObject)
{
	rootObject->AddChild(gameObject);
}

Camera* Scene::GetActiveCamera()
{
	return activeCamera;
}


void Scene::SetActiveCamera(Camera* camera)
{
	activeCamera = camera;
}

void Scene::Shutdown()
{
	delete rootObject;
}

Scene::~Scene()
{
	this->Shutdown();
}