#include"stdafx.h"
#include "GameObject.h"
#include"MonoBehaviour.h"
#include"Transform.h"
#include"MeshRenderer.h"
#include"Camera.h"
#include"ResourceManager.h"
#include"Terrain.h"
#include"Collider.h"


CGameObject::CGameObject(bool makeTransform)
{
	if (makeTransform) {
		mTransform = std::make_shared<CTransform>();
		mComponents[COMPONENT_TYPE::TRANSFORM] = mTransform;
	}
}

CGameObject::~CGameObject()
{
}

void CGameObject::Awake()
{
	for (auto& [type, component] : mComponents) {
		component->Awake();
	}
	for (auto& [type, script] : mScripts) {
		script->Awake();
	}

	for (auto& child : mChildren) {
		child->Awake();
	}

	if(!mMeshRenderer)
		mMeshRenderer = std::static_pointer_cast<CMeshRenderer>(GetComponent(COMPONENT_TYPE::MESH_RENDERER));
	if (!mCollider) {
		mCollider = std::static_pointer_cast<CCollider>(GetComponent(COMPONENT_TYPE::COLLIDER));
	}
}

void CGameObject::Start()
{
	for (auto& [type, component] : mComponents) {
		component->Start();
	}
	for (auto& [type, script] : mScripts) {
		script->Start();
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

	for (auto& [type, component] : mComponents) {
		component->Update();
	}
	for (auto& [type, script] : mScripts) {
		script->Update();
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

	for (auto& [type, component] : mComponents) {
		component->LateUpdate();
	}
	for (auto& [type, script] : mScripts) {
		script->LateUpdate();
	}

	for (auto& child : mChildren) {
		child->LateUpdate();
	}
}

void CGameObject::Render(const std::shared_ptr<CCamera>& camera)
{
	if (!mActive) {
		return;
	}

	mTransform->BindConstantBuffer();

	if (mMeshRenderer) {
		if (mCollider) {
			if (camera->IsInFrustum(mCollider->GetWorldOOBB())) {
				mMeshRenderer->Render();
			}
		}
	}

	for (auto& child : mChildren) {
		child->Render(camera);
	}
}

void CGameObject::Render()
{
	if (!mActive) {
		return;
	}

	mTransform->BindConstantBuffer();
	if (mMeshRenderer) {
		mMeshRenderer->Render();
	}

	for (auto& child : mChildren) {
		child->Render();
	}
}

bool CGameObject::AddComponent(const std::shared_ptr<CComponent>& component)
{
	COMPONENT_TYPE componentType = component->GetType();

	auto itr = mComponents.find(componentType);

	if (itr != mComponents.end()) {
		return false;
	}

	mComponents[componentType] = component;

	return true;
}

bool CGameObject::AddScript(const std::shared_ptr<CMonoBehaviour>& component)
{
	const std::string& name = component->GetName();

	auto itr = mScripts.find(name);

	if (itr != mScripts.end()) {
		return false;
	}

	mScripts[name] = component;

	return true;
}

std::shared_ptr<CComponent> CGameObject::AddComponent(COMPONENT_TYPE type)
{
	auto itr = mComponents.find(type);

	if (itr != mComponents.end()) {
		return itr->second;
	}
	
	return nullptr;
}

std::shared_ptr<CMonoBehaviour> CGameObject::AddComponent(const std::string& name)
{
	auto itr = mScripts.find(name);

	if (itr != mScripts.end()) {
		return itr->second;
	}


	return std::shared_ptr<CMonoBehaviour>();
}

std::shared_ptr<CComponent> CGameObject::GetComponent(COMPONENT_TYPE type)
{
	auto itr = mComponents.find(type);

	if (itr != mComponents.end()) {
		return itr->second;
	}

	return nullptr;
}

std::shared_ptr<CMonoBehaviour> CGameObject::GetComponent(const std::string& name)
{
	auto itr = mScripts.find(name);

	if (itr != mScripts.end()) {
		return itr->second;
	}

	return nullptr;
}

void CGameObject::SetComponentOwner(const std::shared_ptr<CGameObject>& owner)
{
	for (auto& [type, component] : mComponents) {
		component->SetOwner(owner);
	}
	for (auto& [type, script] : mScripts) {
		script->SetOwner(owner);
	}
}

void CGameObject::SetParent(const std::shared_ptr<CGameObject>& parent)
{
	mTransform->SetParent(parent->mTransform);
}

void CGameObject::ReturnCBVIndex()
{
	mTransform->ReturnCBVIndex();
	for (auto& child : mChildren) {
		child->ReturnCBVIndex();
	}
}


std::shared_ptr<CGameObject> CGameObject::Instantiate(const std::shared_ptr<CGameObject>& original, 
	const std::shared_ptr<CTransform>& parentTransform)
{
	std::shared_ptr<CGameObject> instance = std::make_shared<CGameObject>();

	for (const auto& [key, component] : original->mComponents) {
		instance->mComponents[key] = component->Clone();
		instance->mComponents[key]->SetOwner(instance);
	}

	instance->mTransform = std::static_pointer_cast<CTransform>(instance->GetComponent(COMPONENT_TYPE::TRANSFORM));
	instance->mTag = original->mTag;
	instance->mLayerType = original->mLayerType;

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

	for (const auto& [key, component] : original->mComponents) {
		instance->mComponents[key] = component->Clone();
		instance->mComponents[key]->SetOwner(instance);
	}

	instance->mTransform = std::static_pointer_cast<CTransform>(instance->GetComponent(COMPONENT_TYPE::TRANSFORM));
	instance->mTag = original->mTag;
	instance->mLayerType = original->mLayerType;

	if (parentTransform) {
		instance->mTransform->SetParent(parentTransform);
	}

	for (const auto& originChild : original->mChildren) {
		auto childInstance = Instantiate(originChild, instance->mTransform);
	}

	return instance;
}


std::shared_ptr<CGameObject> CGameObject::CreateCameraObject(const std::wstring& tag, Vec2 rtSize, 
	float nearPlane, float farPlane, float fovAngle)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->SetTag(tag);

	auto camera = std::make_shared<CCamera>();
	object->AddComponent(camera);
	camera->SetViewport(0, 0, rtSize.x, rtSize.y);
	camera->SetScissorRect(0, 0, rtSize.x, rtSize.y);
	camera->GeneratePerspectiveProjectionMatrix(nearPlane, farPlane, fovAngle);

	object->SetComponentOwner(object);
	object->SetActive(true);
	CCamera::AddCamera(camera);

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateRenderObject(const std::wstring& tag, 
	const std::wstring& meshName, const std::wstring& materialName)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->mTag = tag;

	auto meshRenderer = std::make_shared<CMeshRenderer>();
	object->AddComponent(meshRenderer);
	meshRenderer->SetMesh(RESOURCE.Get<CMesh>(meshName));
	meshRenderer->AddMaterial(RESOURCE.Get<CMaterial>(materialName));

	object->SetComponentOwner(object);
	object->SetActive(true);

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateTerrainObject(const std::wstring& tag, const std::wstring& heightMapName, 
	UINT width, UINT height, Vec3 scale)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->mTag = tag;

	std::shared_ptr<CHeightMapGridMesh> mesh = std::make_shared<CHeightMapGridMesh>();
	mesh->Initialize(heightMapName, width, height, scale);
	mesh->SetName(L"HeightMap01");
	RESOURCE.Add(mesh);

	std::shared_ptr<CTerrainMaterial> material = std::make_shared<CTerrainMaterial>();
	material->mSpecularColor = { 0.2,0.2,0.2,0.001 };

	auto texture = RESOURCE.Get<CTexture>(L"TerrainBase");
	if (texture) {
		material->mDiffuseMapIdx = texture->GetSrvIndex();
	}
	texture = RESOURCE.Get<CTexture>(L"TerrainDetail");
	if (texture) {
		material->mDetailMap1Idx = texture->GetSrvIndex();
	}
	texture = RESOURCE.Get<CTexture>(L"TerrainNormal");
	if (texture) {
		material->mNormalMapIdx = texture->GetSrvIndex();
	}

	auto terrain = std::make_shared<CTerrain>();
	object->AddComponent(terrain);
	terrain->SetHeightMapGridMesh(mesh);
	terrain->SetMaterial(material);

	object->SetComponentOwner(object);
	object->SetActive(true);

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateObjectFromFile(const std::wstring& tag, const std::wstring& fileName)
{
	std::ifstream ifs{ fileName, std::ios::binary };
	if (!ifs) {
		return nullptr;
	}

	std::shared_ptr<CGameObject> root = std::make_shared<CGameObject>();
	root->mTag = tag;
	InitFromFile(root, ifs);

	if (!root->GetCollider()) {
		auto collider = std::make_shared<CCollider>();
		root->AddComponent(collider);
		root->mCollider = collider;
	}

	return root;
}

void CGameObject::InitFromFile(std::shared_ptr<CGameObject> obj, std::ifstream& inFile)
{
	using namespace BinaryReader;

	int frameNum{};
	int textureCnt{};

	std::string token{};

	while (true) {
		ReadDateFromFile(inFile, token);
		if (token == "<Frame>:") {
			ReadDateFromFile(inFile, frameNum);
			ReadDateFromFile(inFile, textureCnt);

			std::string name{};
			ReadDateFromFile(inFile, name);
			obj->mName = stringToWstring(name);
		}
		else if (token == "<Transform>:") {
			obj->CreateTransformFromFile(inFile);
			obj->mTransform->SetOwner(obj);
		}
		else if (token == "<Mesh>:") {
			obj->CreateMeshRendererFromFile(inFile);
			obj->mMeshRenderer->SetOwner(obj);
			obj->mCollider->SetOwner(obj);
		}
		else if (token == "<Children>:") {
			int childrenCnt{};
			ReadDateFromFile(inFile, childrenCnt);

			for (int i = 0; i < childrenCnt; i++) {
				std::shared_ptr<CGameObject> child = std::make_shared<CGameObject>();
				InitFromFile(child, inFile);
				child->SetParent(obj);
			}
		}
		else if (token == "</Frame>") {
			break;
		}
	}
}

void CGameObject::CreateTransformFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;
	ReadDateFromFile(inFile, mTransform->mLocalPosition);
	ReadDateFromFile(inFile, mTransform->mLocalEulerAngle);
	ReadDateFromFile(inFile, mTransform->mLocalScale);
	ReadDateFromFile(inFile, mTransform->mLocalRotation);

	std::string token{};
	ReadDateFromFile(inFile, token);
	if (token == "<TransformMatrix>:") {
		ReadDateFromFile(inFile, mTransform->mLocalMat);
	}
}

void CGameObject::CreateMeshRendererFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;
	mMeshRenderer = std::make_shared<CMeshRenderer>();
	AddComponent(mMeshRenderer);
	auto mesh = CMesh::CreateMeshFromFile(inFile);
	mMeshRenderer->SetMesh(mesh);
	RESOURCE.Add(mesh);

	mCollider = std::make_shared<CCollider>();
	AddComponent(mCollider);
	mCollider->SetLocalOOBB(mesh->oobb);

	std::string token{};
	ReadDateFromFile(inFile, token);

	if (token == "<Materials>:") {
		int materialCnt{};
		ReadDateFromFile(inFile, materialCnt);
		ReadDateFromFile(inFile, token);
		for (int i = 0; i < materialCnt; i++) {
			int matIdx{};
			ReadDateFromFile(inFile, matIdx);

			auto material = CMaterial::CreateMaterialFromFile(inFile);
			material->SetName(std::to_wstring(RANDOM_COLOR + matIdx));
			mMeshRenderer->AddMaterial(material);
			RESOURCE.Add(material);
		}
	}
}

const BoundingBox& CGameObject::CombineChildrenOOBB()
{
	if (!mCollider) {
		return BoundingBox();
	}

	BoundingBox result = mCollider->GetAABB();

	for (const auto& child : mChildren) {
		BoundingBox childAABB = child->CombineChildrenOOBB();
		BoundingBox::CreateMerged(result, result, childAABB);
	}

	return result;
}

void CGameObject::CalculateRootOOBB()
{
	BoundingBox combinedAABB = CombineChildrenOOBB();
	BoundingOrientedBox oobb{};
	BoundingOrientedBox::CreateFromBoundingBox(oobb, combinedAABB);

	mCollider->SetLocalOOBB(oobb);
}

std::shared_ptr<CGameObject> CGameObject::FindChildByName(const std::wstring& name)
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


