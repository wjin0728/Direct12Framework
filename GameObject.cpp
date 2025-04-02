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
#include"Animation.h"
#include"SkinnedMesh.h"
#include <unordered_set>

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
	if (!mCollider)
		mCollider = GetComponent<CCollider>();
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
		mesh->Initialize(heightMapName, width, height, scale);
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

std::shared_ptr<CModelInfo> CGameObject::CreateObjectFromFile(const std::wstring& tag, const std::wstring& fileName)
{
	std::ifstream ifs{ fileName, std::ios::binary };
	if (!ifs) {
		return nullptr;
	}
	ifs.seekg(0, std::ios_base::beg);

	std::shared_ptr<CModelInfo> modelInfo = std::make_shared<CModelInfo>();
	modelInfo->mRootObject->mTag = tag;
	InitFromFile(modelInfo, ifs);

	Vec3 boundingCenter;
	float radius;

	BinaryReader::ReadDateFromFile(ifs, boundingCenter);
	BinaryReader::ReadDateFromFile(ifs, radius);

	modelInfo->mRootObject->mRootLocalBS = BoundingSphere(boundingCenter, radius);

	if (!modelInfo->mRootObject->GetCollider()) {
		auto collider = std::make_shared<CCollider>();
		modelInfo->mRootObject->AddComponent(collider);
		modelInfo->mRootObject->mCollider = collider;
	}

	

	return modelInfo;
}

std::shared_ptr<CGameObject> CGameObject::GetSptrFromThis()
{
	return shared_from_this();
}

void CGameObject::InitFromFile(std::shared_ptr<CModelInfo> model, std::ifstream& inFile)
{
	using namespace BinaryReader;

	int frameNum{};
	int textureCnt{};
	int skinnedMeshCnt{};

	std::string token{};

	while (true) {
		ReadDateFromFile(inFile, token);
		if (token == "<Hierarchy>") {
			while (true) {
				ReadDateFromFile(inFile, token); // </Hierarchy>
				if (token == "<Frame>:") {
					std::shared_ptr<CGameObject> root = std::make_shared<CGameObject>();
					InitHierarchyFromFile(root, inFile, &skinnedMeshCnt);
					if (root) model->mRootObject->AddChild(root);
				}
				else if (token == "</Hierarchy>") {
					break;
				}
			}
		}
		else if (token == "<Animation>") {
			InitAnimationFromFile(model, inFile);

			model->mAnimationSets->mBoneFrameCaches.resize(model->mRootObject->mChildren.size());
			model->mRootObject->mChildren.front()->CacheFrameHierarchies(model->mAnimationSets->mBoneFrameCaches);

			model->PrepareSkinning(skinnedMeshCnt);
		}
		else if (token == "</Animation>") {
			break;
		}
	}
}

void CGameObject::InitHierarchyFromFile(std::shared_ptr<CGameObject> obj, std::ifstream& inFile, int* skinnedMeshCnt) {
	using namespace BinaryReader;

	int frameNum{};
	int textureCnt{};

	std::string token{};

	ReadDateFromFile(inFile, frameNum);
	ReadDateFromFile(inFile, textureCnt);

	std::string name{};

	ReadDateFromFile(inFile, name);
	if (name == "ID55inst") {
		std::cout << "Asdfasdf";
	}
	obj->mName = stringToWstring(name);

	while (true) {
		ReadDateFromFile(inFile, token);
		if (token == "<Transform>:") {
			obj->CreateTransformFromFile(inFile);
			obj->mTransform->SetOwner(obj.get());
		}
		else if (token == "<Mesh>:" || token == "<SkinDeformations>:") {
			obj->CreateMeshRendererFromFile(inFile, token);
			obj->mMeshRenderer->SetOwner(obj.get());
			obj->mCollider->SetOwner(obj.get());
		}
		else if (token == "<Children>:") {
			int childrenCnt{};
			ReadDateFromFile(inFile, childrenCnt);

			for (int i = 0; i < childrenCnt; i++) {
				ReadDateFromFile(inFile, token);
				if (token == "<Frame>:") {
					std::shared_ptr<CGameObject> child = std::make_shared<CGameObject>();
					InitHierarchyFromFile(child, inFile, skinnedMeshCnt);
					if (child) child->SetParent(obj);
				}
			}
		}
		else if (token == "</Frame>") {
			break;
		}
	}
}
void CGameObject::InitAnimationFromFile(std::shared_ptr<CModelInfo> model, std::ifstream& inFile)
{
	using namespace BinaryReader;

	std::string token{};
	int setsNum{};

	while (true) {
		ReadDateFromFile(inFile, token);

		if (token == "<AnimationSets>:") {
			ReadDateFromFile(inFile, setsNum);
			model->mAnimationSets = std::make_shared<CAnimationSets>(setsNum);
		}
		else if (token == "<AnimationSet>:") {
			CreateAnimationSetFromFile(model, inFile);
		}
		else if (token == "</AnimationSets>")
		{

			break;
		}
	}
}

void CGameObject::CreateTransformFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;
	ReadDateFromFile(inFile, mTransform->mLocalMat);
	ReadDateFromFile(inFile, mTransform->mLocalScale);
	ReadDateFromFile(inFile, mTransform->mLocalEulerAngle);
	ReadDateFromFile(inFile, mTransform->mLocalPosition);

	//std::string token{};
	//ReadDateFromFile(inFile, token);
	//if (token == "<TransformMatrix>:") {
	//	ReadDateFromFile(inFile, mTransform->mLocalMat);
	//}
}

void CGameObject::CreateMeshRendererFromFile(std::ifstream& inFile, std::string& meshType)
{
	using namespace BinaryReader;

	mMeshRenderer = std::make_shared<CMeshRenderer>();
	AddComponent(mMeshRenderer);

	std::shared_ptr<CMesh> mesh;
	std::string token{};

	if (meshType == "<SkinDeformations>:") {
		mesh = CSkinnedMesh::CreateSkinnedMeshFromFile(inFile);
		ReadDateFromFile(inFile, token); // <Mesh>:
	}
	mesh = CMesh::CreateMeshFromFile(inFile);

	std::string meshName;
	BinaryReader::ReadDateFromFile(inFile, meshName);
	std::wstring meshNameW = BinaryReader::stringToWstring(meshName);

	mesh->SetName(meshNameW);
	mMeshRenderer->SetMesh(mesh);
	RESOURCE.Add(mesh);

	mCollider = std::make_shared<CCollider>();
	AddComponent(mCollider);
	mCollider->SetLocalOOBB(mesh->oobb);

	ReadDateFromFile(inFile, token);

	if (token == "<Materials>:") {
		int materialCnt{};
		ReadDateFromFile(inFile, materialCnt);

		for (int i = 0; i < materialCnt; i++) {
			ReadDateFromFile(inFile, token);

			std::shared_ptr<CMaterial> material = CMaterial::CreateMaterialFromFile(inFile);

			mMeshRenderer->AddMaterial(material);
		}

		ReadDateFromFile(inFile, token);
	}
}

void CGameObject::CreateAnimationSetFromFile(std::shared_ptr<CModelInfo>& model, std::ifstream& inFile)
{
	using namespace BinaryReader;
	
	int setNum{};
	float startTime{};
	float endTime{};
	std::string token;

	ReadDateFromFile(inFile, setNum);
	ReadDateFromFile(inFile, token); // Animation Set Name
	ReadDateFromFile(inFile, startTime);
	ReadDateFromFile(inFile, endTime);

	auto animSet = std::make_shared<CAnimationSet>(startTime, endTime, token);
	model->mAnimationSets->mAnimationSet[setNum] = animSet;

	ReadDateFromFile(inFile, token);
	if (token != "<AnimationLayers>:") {
		int layersNum{};
		ReadDateFromFile(inFile, layersNum);

		animSet->mLayers.resize(layersNum);
		animSet->mScales.resize(layersNum);
		animSet->mRotations.resize(layersNum);
		animSet->mTranslations.resize(layersNum);

		for (int i = 0; i < layersNum; i++) {
			CreateAnimationLayerFromFile(model, inFile, animSet, i);
		}

		ReadDateFromFile(inFile, token); //</AnimationLayers>

		int commonBoneNum = CalculateCommonBoneNum(animSet);
		if (commonBoneNum) {
			animSet->mCommonBoneFrameCaches.resize(commonBoneNum);
		}

	}

	ReadDateFromFile(inFile, token); //</AnimationSet>
}

void CGameObject::CreateAnimationLayerFromFile(std::shared_ptr<CModelInfo>& model, std::ifstream& inFile, std::shared_ptr<CAnimationSet>& animSet, int layerIndex)
{
	using namespace BinaryReader;

	std::string token;
	ReadDateFromFile(inFile, token);
	if (token != "<AnimationLayer>:")
		return;

	int layerNum{};
	ReadDateFromFile(inFile, layerNum);

	animSet->mLayers[layerIndex] = std::make_shared<CAnimationLayer>();
	auto layer = animSet->mLayers[layerIndex];

	int cacheNum{};
	ReadDateFromFile(inFile, cacheNum);

	layer->mBoneFrameCaches.resize(cacheNum);
	layer->mAnimationCurves.resize(cacheNum);

	animSet->mScales[layerIndex].resize(cacheNum);
	animSet->mRotations[layerIndex].resize(cacheNum);
	animSet->mTranslations[layerIndex].resize(cacheNum);

	ReadDateFromFile(inFile, layer->mWeight);

	for (int j = 0; j < cacheNum; j++)
	{
		ReadDateFromFile(inFile, token);
		if (token != "<AnimationCurve>:")
			break;

		int curveNude{};
		ReadDateFromFile(inFile, curveNude); //j

		ReadDateFromFile(inFile, token); //Frame Name
		layer->mBoneFrameCaches[j] = model->mRootObject->FindChildByName(stringToWstring(token));

		CreateAnimationCurvesFromFile(inFile, layer, j);
	}

	ReadDateFromFile(inFile, token); //</AnimationLayer>
}

void CGameObject::CreateAnimationCurvesFromFile(std::ifstream& inFile, std::shared_ptr<CAnimationLayer>& layer, int curveIndex)
{
	using namespace BinaryReader;
	std::string token;

	while (true) {
		ReadDateFromFile(inFile, token);

		if (token == "<TX>:") layer->LoadKeyValues(curveIndex, 0, inFile);
		else if (token == "<TY>:") layer->LoadKeyValues(curveIndex, 1, inFile);
		else if (token == "<TZ>:") layer->LoadKeyValues(curveIndex, 2, inFile);
		else if (token == "<RX>:") layer->LoadKeyValues(curveIndex, 3, inFile);
		else if (token == "<RY>:") layer->LoadKeyValues(curveIndex, 4, inFile);
		else if (token == "<RZ>:") layer->LoadKeyValues(curveIndex, 5, inFile);
		else if (token == "<SX>:") layer->LoadKeyValues(curveIndex, 6, inFile);
		else if (token == "<SY>:") layer->LoadKeyValues(curveIndex, 7, inFile);
		else if (token == "<SZ>:") layer->LoadKeyValues(curveIndex, 8, inFile);
		else if (token == "</AnimationCurve>")
		{
			break;
		}
	}
}

int CGameObject::CalculateCommonBoneNum(std::shared_ptr<CAnimationSet>& animSet)
{
	int64_t commonBoneNum = 0;
	std::unordered_map<std::shared_ptr<CGameObject>, std::pair<int64_t, int64_t>> global_map;

	for (const auto& layer : animSet->mLayers) {
		std::unordered_map<std::shared_ptr<CGameObject>, int> local_count;
		for (const auto& cache : layer->mBoneFrameCaches) {
			local_count[cache]++;
		}

		for (const auto& [cache, m] : local_count) {
			auto& sums = global_map[cache];
			sums.first += m;
			sums.second += static_cast<int64_t>(m) * m;
		}
	}

	for (const auto& [cache, sums] : global_map) {
		int64_t sum_m = sums.first;
		int64_t sum_m2 = sums.second;
		int64_t contrib = (sum_m * sum_m - sum_m2) / 2;
		commonBoneNum += contrib;
	}

	return commonBoneNum;
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

void CGameObject::UpdateTransform(const Matrix* parent)
{
	if (parent) {
		mTransform->mWorldMat = mTransform->mLocalMat * (*parent);
	}
	else {
		mTransform->mWorldMat = mTransform->mLocalMat;
	}

	// 필요하다면 여기서 콜라이더나 바운딩볼 갱신

	for (auto& child : mChildren) {
		child->UpdateTransform(&mTransform->mWorldMat);
	}
}

void CGameObject::CacheFrameHierarchies(std::vector<std::shared_ptr<CGameObject>>& boneFrameCaches)
{
	boneFrameCaches.push_back(GetSptrFromThis());

	for (auto& child : mChildren) {
		child->CacheFrameHierarchies(boneFrameCaches);
	}
}

void CGameObject::Animate(float elapsedTime)
{
	//OnPrepareRender();
	
	if (mAnimationController) {
		ResetForAnimationBlending();
		mAnimationController->AdvanceTime(elapsedTime, shared_from_this());
	}

	for (auto& child : mChildren) {
		child->Animate(elapsedTime);
	}
}

void CGameObject::ResetForAnimationBlending()
{
	mTransform->mScaleLayerBlending = Vec3(0.0f, 0.0f, 0.0f);
	mTransform->mRotationLayerBlending = Vec3(0.0f, 0.0f, 0.0f);
	mTransform->mPositionLayerBlending = Vec3(0.0f, 0.0f, 0.0f);

	for (auto& child : mChildren) {
		child->ResetForAnimationBlending();
	}
}

void CGameObject::FindAndSetSkinnedMesh(std::vector<std::shared_ptr<CSkinnedMesh>>& skinnedMeshes, int skinnedMeshNum)
{
	if (mMeshRenderer) {
		auto mesh = mMeshRenderer->GetMesh();
		if (mesh) {
			auto skinned = std::dynamic_pointer_cast<CSkinnedMesh>(mesh);
			if (skinned) {
				skinnedMeshes.push_back(skinned);
				skinnedMeshNum++;
			}
		}
	}

	for (auto& child : mChildren) {
		child->FindAndSetSkinnedMesh(skinnedMeshes, skinnedMeshNum);
	}
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

void CGameObject::SetTrackAnimationSet(int trackIndex, int setIndex)
{
	if (mAnimationController) mAnimationController->SetTrackAnimationSet(trackIndex, setIndex);
}

void CGameObject::SetTrackAnimationPosition(int trackIndex, float position)
{
	if (mAnimationController) mAnimationController->SetTrackPosition(trackIndex, position);
}

