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

	float left = (float)cameraSettings["left"];
	float right = (float)cameraSettings["right"];
	float bottom = (float)cameraSettings["bottom"];
	float nearP = (float)cameraSettings["nearPlane"];
	float farP = (float)cameraSettings["farPlane"];

	json position = cameraSettings["position"];
	Vector3 pos = Vector3((float)position[0], (float)position[1], (float)position[2]);

	json target = cameraSettings["target"];
	Vector3 tar = Vector3((float)target[0], (float)target[1], (float)target[2]);

	activeCamera = new Camera(pos, tar, Vector3::up);
	//camera->SetFrustrum(-1.778f, 1.778f, -1.0f, 1.0f, 1, 10, true);
	activeCamera->SetFrustrum(left, 1.778f, -1.0f, 1.0f, 1, 2000, true);
	
	//mainCamera = new Camera(tar, pos, fov, speed, sens, nearP, farP);
}

void Scene::LoadGameObjs(const json& configJson)
{
	//model setup
	std::string modelMesh, modelName;

	unsigned int modelCount = (unsigned int)configJson["gameobjs"].size();

	for (unsigned int i = 0; i < modelCount; ++i) {
		//get model mesh and material info
		json currentGameObj= configJson["gameobjs"][i];

		json position = currentGameObj["position"];
		json rotation = currentGameObj["rotation"];
		json scaling = currentGameObj["scaling"];

		GameObject* pObj = new GameObject();
		pObj->name = currentGameObj["name"];
		pObj->transform.rotation = Quaternion::Euler((float)rotation[0], (float)rotation[1], (float)rotation[2]);
		pObj->transform.position = Vector3((float)position[0], (float)position[1], (float)position[2]);
		pObj->transform.scale = Vector3((float)scaling[0], (float)scaling[1], (float)scaling[2]);
		AddGameObject(pObj);

		string modelPath = currentGameObj["model"];
		Model* centerModel = new Model(modelPath.c_str(), true);
		centerModel->CreateBufferData();

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
	}
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