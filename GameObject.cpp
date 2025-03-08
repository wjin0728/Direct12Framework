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

void CGameObject::Render()
{
	if (!mActive) {
		return;
	}

	if (mMeshRenderer) {
		mMeshRenderer->Render();
	}

	for (auto& child : mChildren) {
		child->Render();
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
		mTransform->ReturnCBVIndex();
	}
	else {
		mTransform->SetCBVIndex();
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
	mTransform->ReturnCBVIndex();
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

	object->SetActive(true);
	
	INSTANCE(CSceneManager).GetCurScene()->AddCamera(camera);

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateRenderObject(const std::wstring& tag, 
	const std::wstring& meshName, const std::wstring& materialName)
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

std::shared_ptr<CGameObject> CGameObject::CreateUIObject(const std::wstring& tag, const std::wstring& materialName, Vec2 pos)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->mTag = tag;

	object->mMeshRenderer = std::make_shared<CMeshRenderer>();
	object->AddComponent(object->mMeshRenderer);
	object->mMeshRenderer->SetMesh(RESOURCE.Get<CMesh>(L"Rectangle"));
	object->mMeshRenderer->AddMaterial(RESOURCE.Get<CMaterial>(materialName));

	object->GetTransform()->SetLocalPosition({ pos.x, pos.y, 0.f });
	object->SetActive(true);
	object->SetRenderLayer(L"UI");

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateTerrainObject(const std::wstring& tag, const std::wstring& heightMapName, 
	UINT width, UINT height, Vec3 scale)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->mTag = tag;

	auto mesh = std::static_pointer_cast<CHeightMapGridMesh>(RESOURCE.Get<CMesh>(L"HeightMap01"));
	if (!mesh) {
		mesh = std::make_shared<CHeightMapGridMesh>();
		mesh->Initialize(heightMapName, width, scale);
		mesh->SetName(L"HeightMap01");
		RESOURCE.Add(mesh);
	}

	std::shared_ptr<CTerrainMaterial> material = std::make_shared<CTerrainMaterial>();
	material->mSpecularColor = { 0.0,0.0,0.0,0.001 };

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

	auto terrain = object->AddComponent<CTerrain>();
	terrain->SetHeightMapGridMesh(mesh);
	terrain->SetMaterial(material);

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

	Vec3 boundingCenter;
	float radius;

	BinaryReader::ReadDateFromFile(ifs, boundingCenter);
	BinaryReader::ReadDateFromFile(ifs, radius);

	root->mRootLocalBS = BoundingSphere(boundingCenter, radius);

	if (!root->GetCollider()) {
		auto collider = std::make_shared<CCollider>();
		root->AddComponent(collider);
		root->mCollider = collider;
	}

	

	return root;
}

std::shared_ptr<CGameObject> CGameObject::GetSptrFromThis()
{
	return shared_from_this();
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
			obj->mTransform->SetOwner(obj.get());
		}
		else if (token == "<Mesh>:") {
			obj->CreateMeshRendererFromFile(inFile);
			obj->mMeshRenderer->SetOwner(obj.get());
			obj->mCollider->SetOwner(obj.get());
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


