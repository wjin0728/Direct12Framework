#include"stdafx.h"
#include "GameObject.h"
#include"MonoBehaviour.h"
#include"Transform.h"
#include"MeshRenderer.h"
#include"Camera.h"
#include"ResourceManager.h"
#include"Terrain.h"
#include"Collider.h"
#include"UploadBuffer.h"
#include"SceneManager.h"
#include"Scene.h"


CGameObject::CGameObject(bool makeTransform)
{
	if (makeTransform) {
		mTransform = std::make_shared<CTransform>();
		mComponents.push_back(mTransform);
		mTransform->SetOwner(this);
	}
}

CGameObject::~CGameObject()
{
}

void CGameObject::Awake()
{
	for (auto& component : mComponents) {
		component->Awake();
	}

	for (auto& child : mChildren) {
		child->Awake();
	}

	if(!mMeshRenderer)
		mMeshRenderer = GetComponent<CMeshRenderer>();
	if (!mCollider) {
		mCollider = GetComponent<CCollider>();
	}
}

void CGameObject::Start()
{
	for (auto& component : mComponents) {
		component->Start();
	}

	for (auto& child : mChildren) {
		child->Start();
	}
}

void CGameObject::Update()
{
	if (!mActive) {
		return;
	}

	for (auto& component : mComponents) {
		component->Update();
	}

	for (auto& child : mChildren) {
		child->Update();
	}
}

void CGameObject::LateUpdate()
{
	if (!mActive) {
		return;
	}

	for (auto& component : mComponents) {
		component->LateUpdate();
	}

	for (auto& child : mChildren) {
		child->LateUpdate();
	}

	if (!mIsStatic) mTransform->UpdateWorldMatrix();
}

void CGameObject::Render(std::shared_ptr<CCamera> camera, int pass)
{
	if (mName == "SM_Env_Skydome_01" && pass == FORWARD) {
		int i = 0;
	}
	if (!mActive) return;
	if (mMeshRenderer) mMeshRenderer->Render(camera, pass);
	for (auto& child : mChildren) {
		child->Render(camera, pass);
	}
}

void CGameObject::SetStatic(bool isStatic)
{
	mIsStatic = isStatic;

	for (auto& child : mChildren) {
		child->SetStatic(isStatic);
	}
}

void CGameObject::SetInstancing(bool isInstancing)
{
	if (isInstancing) {
		mMeshRenderer->ReturnCBVIndex();
	}
	else {
		mMeshRenderer->SetCBVIndex();
	}

	mIsInstancing = isInstancing;

	for (auto& child : mChildren) {
		child->SetStatic(isInstancing);
	}
}

void CGameObject::SetParent(const std::shared_ptr<CGameObject>& parent)
{
	mTransform->SetParent(parent->mTransform);
}

void CGameObject::ReturnCBVIndex()
{
	if (!mMeshRenderer) return;

	mMeshRenderer->ReturnCBVIndex();
	for (auto& child : mChildren) {
		child->ReturnCBVIndex();
	}
}

std::shared_ptr<CGameObject> CGameObject::Instantiate(const std::shared_ptr<CGameObject>& original, 
	const std::shared_ptr<CTransform>& parentTransform)
{
	std::shared_ptr<CGameObject> instance = std::make_shared<CGameObject>(false);

	for (const auto& component : original->mComponents) {
		instance->mComponents.push_back(component->Clone());
		instance->mComponents.back()->SetOwner(instance.get());
	}

	instance->mTransform = instance->GetComponent<CTransform>();
	instance->mTag = original->mTag;
	instance->mLayerType = original->mLayerType;
	instance->mRootLocalBS = original->mRootLocalBS;
	instance->mName = original->mName;

	instance->mMeshRenderer = instance->GetComponent<CMeshRenderer>();
	instance->mCollider = instance->GetComponent<CCollider>();

	if (parentTransform) {
		instance->mTransform->SetParent(parentTransform);
	}

	for (const auto& originChild : original->mChildren) {
		auto childInstance = Instantiate(originChild, instance->mTransform);
	}

	return instance;
}



std::shared_ptr<CGameObject> CGameObject::Instantiate(const std::unique_ptr<CGameObject>& original,
	const std::shared_ptr<CTransform>& parentTransform)
{
	std::shared_ptr<CGameObject> instance = std::make_shared<CGameObject>(false);

	for (const auto& component : original->mComponents) {
		instance->mComponents.push_back(component->Clone());
		instance->mComponents.back()->SetOwner(instance.get());
	}

	instance->mTransform = instance->GetComponent<CTransform>();
	instance->mTag = original->mTag;
	instance->mLayerType = original->mLayerType;
	instance->mRootLocalBS = original->mRootLocalBS;
	instance->mName = original->mName;

	instance->mMeshRenderer = instance->GetComponent<CMeshRenderer>();
	instance->mCollider = instance->GetComponent<CCollider>();

	if (parentTransform) {
		instance->mTransform->SetParent(parentTransform);
	}

	for (const auto& originChild : original->mChildren) {
		auto childInstance = Instantiate(originChild, instance->mTransform);
	}

	return instance;
}


std::shared_ptr<CGameObject> CGameObject::CreateCameraObject(const std::string& tag, Vec2 rtSize, 
	float nearPlane, float farPlane, float fovAngle)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->SetTag(tag);

	auto camera = std::make_shared<CCamera>();
	object->AddComponent(camera);
	camera->SetViewport(0, 0, rtSize.x, rtSize.y);
	camera->SetScissorRect(0, 0, rtSize.x, rtSize.y);
	camera->GeneratePerspectiveProjectionMatrix(nearPlane, farPlane, fovAngle);

	object->SetActive(true);
	
	INSTANCE(CSceneManager).GetCurScene()->AddCamera(camera);

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateRenderObject(const std::string& tag, 
	const std::string& meshName, const std::string& materialName)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->mTag = tag;

	object->mMeshRenderer = std::make_shared<CMeshRenderer>();
	object->AddComponent(object->mMeshRenderer);
	object->mMeshRenderer->SetMesh(RESOURCE.Get<CMesh>(meshName));
	object->mMeshRenderer->AddMaterial(RESOURCE.Get<CMaterial>(materialName));

	object->SetActive(true);

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateUIObject(const std::string& tag, const std::string& materialName, Vec2 pos)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->mTag = tag;

	object->mMeshRenderer = std::make_shared<CMeshRenderer>();
	object->AddComponent(object->mMeshRenderer);
	object->mMeshRenderer->SetMesh(RESOURCE.Get<CMesh>("Rectangle"));
	object->mMeshRenderer->AddMaterial(RESOURCE.Get<CMaterial>(materialName));

	object->GetTransform()->SetLocalPosition({ pos.x, pos.y, 0.f });
	object->SetActive(true);
	object->SetRenderLayer("UI");

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateTerrainObject(std::ifstream& ifs)
{
	using namespace BinaryReader;
	std::string name{};
	ReadDateFromFile(ifs, name);
		
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();

	UINT resolution{};
	Vec3 size{};
	ReadDateFromFile(ifs, resolution);

	auto material = std::static_pointer_cast<CTerrainMaterial>(RESOURCE.Get<CMaterial>(name + "Material"));
	if (!material) {
		material = std::make_shared<CTerrainMaterial>();
		material->LoadTerrainData(ifs);
		material->SetName(name + "Material");
		RESOURCE.Add(material);
	}
	size = material->GetSize();

	auto mesh = std::static_pointer_cast<CHeightMapGridMesh>(RESOURCE.Get<CMesh>(name + "Mesh"));
	if (!mesh) { 
		mesh = std::make_shared<CHeightMapGridMesh>();
		mesh->Initialize(name + "Heightmap", resolution, size);
		mesh->SetName(name + "Mesh");
		RESOURCE.Add(mesh);
	}
	auto terrain = object->AddComponent<CTerrain>();
	terrain->SetHeightMapGridMesh(mesh);
	terrain->SetMaterial(material);

	object->CreateTransformFromFile(ifs);


	object->SetActive(true);

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateObjectFromFile(std::ifstream& ifs, std::unordered_map<std::string, std::shared_ptr<CGameObject>>& prefabs)
{
	std::shared_ptr<CGameObject> root = InitFromFile(ifs, prefabs);
	if (root) {
		Vec3 boundingCenter;
		float radius;

		BinaryReader::ReadDateFromFile(ifs, boundingCenter);
		BinaryReader::ReadDateFromFile(ifs, radius);

		root->mRootLocalBS = BoundingSphere(boundingCenter, radius);
		root->SetActive(true);
		root->SetStatic(false);
	}
	return root;
}

std::shared_ptr<CGameObject> CGameObject::GetSptrFromThis()
{
	return shared_from_this();
}

std::shared_ptr<CGameObject> CGameObject::InitFromFile(std::ifstream& inFile, std::unordered_map<std::string, std::shared_ptr<CGameObject>>& prefabs)
{
	using namespace BinaryReader;

	std::shared_ptr<CGameObject> obj = std::make_shared<CGameObject>();

	std::string token{};

	while (true) {
		ReadDateFromFile(inFile, token);
		if (token == "<Prefab>:") {
			std::string prefabName{};
			ReadDateFromFile(inFile, prefabName);
			obj = CGameObject::Instantiate(prefabs[prefabName]);
			obj->SetActive(true);
			obj->SetStatic(false);
			obj->CreateTransformFromFile(inFile);
			return obj;
		}
		if (token == "<Frame>:") {
			ReadDateFromFile(inFile, obj->mName);
			if (obj->mName == "SM_Env_Bush_02") {
				int n{};
			}
		}
		else if (token == "<Tag>:") {
			ReadDateFromFile(inFile, obj->mTag);
		}
		else if (token == "<Transform>:") {
			obj->CreateTransformFromFile(inFile);
		}
		else if (token == "<Mesh>:") {
			obj->CreateMeshRendererFromFile(inFile);
		}
		else if (token == "<Terrain>:") {
			obj->CreateTerrainFromFile(inFile);
		}
		else if (token == "<Collider>:") {
			Vec3 center{};
			Vec3 size{};
			ReadDateFromFile(inFile, center);
			ReadDateFromFile(inFile, size);

			auto collider = obj->AddComponent<CCollider>();
			collider->Initialize(center, size);
		}
		else if (token == "<Children>:") {
			int childrenCnt{};
			ReadDateFromFile(inFile, childrenCnt);

			for (int i = 0; i < childrenCnt; i++) {
				std::shared_ptr<CGameObject> child = InitFromFile(inFile, prefabs);
				child->SetParent(obj);
			}
		}
		else if (token == "</Frame>") {
			break;
		}
	}
	return obj;
}

void CGameObject::CreateTransformFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;
	ReadDateFromFile(inFile, mTransform->mLocalPosition);
	ReadDateFromFile(inFile, mTransform->mLocalEulerAngle);
	ReadDateFromFile(inFile, mTransform->mLocalScale);
	ReadDateFromFile(inFile, mTransform->mLocalRotation);

	ReadDateFromFile(inFile, mTransform->mLocalMat);
}

void CGameObject::CreateMeshRendererFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;

	mMeshRenderer = AddComponent<CMeshRenderer>();

	std::string meshName{};
	ReadDateFromFile(inFile, meshName);
	mMeshRenderer->SetMesh(meshName);

	int materialCnt{};
	std::string materialName{};
	ReadDateFromFile(inFile, materialCnt);
	for (int i = 0; i < materialCnt; i++) {
		ReadDateFromFile(inFile, materialName);
		mMeshRenderer->AddMaterial(materialName);
	}


}

void CGameObject::CreateTerrainFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;
	std::string name{};
	ReadDateFromFile(inFile, name);
	UINT resolution{};
	Vec3 size{};
	Vec3 offset{};
	ReadDateFromFile(inFile, resolution);

	auto material = std::static_pointer_cast<CTerrainMaterial>(RESOURCE.Get<CMaterial>(name + "Material"));
	if (!material) {
		material = std::make_shared<CTerrainMaterial>();
		material->LoadTerrainData(inFile);
		material->SetName(name + "Material");
		RESOURCE.Add(material);
	}
	size = material->GetSize();

	ReadDateFromFile(inFile, offset);

	auto mesh = std::static_pointer_cast<CHeightMapGridMesh>(RESOURCE.Get<CMesh>(name + "Mesh"));
	if (!mesh) {
		mesh = std::make_shared<CHeightMapGridMesh>();
		mesh->Initialize(name + "Heightmap", resolution, size, offset);
		mesh->SetName(name + "Mesh");
		RESOURCE.Add(mesh);
	}
	auto terrain = AddComponent<CTerrain>();
	terrain->SetHeightMapGridMesh(mesh);
	terrain->SetMaterial(material);

	INSTANCE(CSceneManager).GetCurScene()->SetTerrain(terrain);
}

std::shared_ptr<CGameObject> CGameObject::FindChildByName(const std::string& name)
{
	std::shared_ptr<CGameObject> obj{ nullptr };

	for (const auto& child : mChildren) {
		if (child->mName == name) 
			return child;

		if (obj = child->FindChildByName(name))
			return obj;
	}

	return obj;
}

void CGameObject::AddChild(std::shared_ptr<CGameObject> child)
{
	auto itr = findByRawPointer(mChildren, child.get());

	if (itr != mChildren.end()) {
		return;
	}
	mChildren.push_back(child);
}

void CGameObject::RemoveChild(std::shared_ptr<CGameObject> child)
{
	auto itr = findByRawPointer(mChildren, child.get());

	if (itr != mChildren.end()) {
		mChildren.erase(itr);
	}
}


