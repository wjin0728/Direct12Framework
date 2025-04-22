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
#include"Renderer.h"
#include"SkinnedMeshRenderer.h"

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

	mRenderer = GetComponent<CRenderer>();
	if (!mRenderer) {
		mRenderer = GetComponent<CSkinnedMeshRenderer>();
	}
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
	if (!mActive) return;
	if (mAnimationController) {
		mAnimationController->BindSkinningMatrix();
	}
	if (mRenderer) {
		mRenderer->Render(camera, pass);
	}
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
		mRenderer->ReturnCBVIndex();
	}
	else {
		mRenderer->SetCBVIndex();
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
	if (!mRenderer) return;

	mRenderer->ReturnCBVIndex();
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
	instance->mObjectType = original->mObjectType;
	instance->mRootLocalBS = original->mRootLocalBS;
	instance->mName = original->mName;

	instance->mRenderer = instance->GetComponent<CMeshRenderer>();
	if (!instance->mRenderer) {
		instance->mRenderer = instance->GetComponent<CSkinnedMeshRenderer>();
	}
	instance->mCollider = instance->GetComponent<CCollider>();
	instance->mAnimationController = instance->GetComponent<CAnimationController>();

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
	instance->mObjectType = original->mObjectType;
	instance->mRootLocalBS = original->mRootLocalBS;
	instance->mName = original->mName;

	instance->mRenderer = instance->GetComponent<CMeshRenderer>();
	if (!instance->mRenderer) {
		instance->mRenderer = instance->GetComponent<CSkinnedMeshRenderer>();
	}
	instance->mCollider = instance->GetComponent<CCollider>();
	instance->mAnimationController = instance->GetComponent<CAnimationController>();

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

std::shared_ptr<CGameObject> CGameObject::CreateCameraObject(const std::string& tag, Vec2 rtSize,
	float nearPlane, float farPlane, Vec2 size)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->SetTag(tag);

	auto camera = std::make_shared<CCamera>();
	object->AddComponent(camera);
	camera->SetViewport(0, 0, rtSize.x, rtSize.y);
	camera->SetScissorRect(0, 0, rtSize.x, rtSize.y);
	camera->GenerateOrthographicProjectionMatrix(nearPlane, farPlane, size.x, size.y);

	object->SetActive(true);

	INSTANCE(CSceneManager).GetCurScene()->AddCamera(camera);
	INSTANCE(CSceneManager).GetCurScene()->AddObject(object);

	return object;
}

std::shared_ptr<CGameObject> CGameObject::CreateUIObject(const std::string& materialName, Vec2 pos, Vec2 size)
{
	std::shared_ptr<CGameObject> object = std::make_shared<CGameObject>();
	object->mTag = "UI";

	auto meshRenderer = std::make_shared<CMeshRenderer>();
	object->mRenderer = meshRenderer;
	object->AddComponent(object->mRenderer);
	meshRenderer->SetMesh(RESOURCE.Get<CMesh>("Rectangle"));
	meshRenderer->AddMaterial(RESOURCE.Get<CMaterial>(materialName));

	object->GetTransform()->SetLocalPosition({ pos.x, pos.y, 0.f });
	object->GetTransform()->SetLocalScale({ size.x, size.y, 1.f });
	object->SetActive(true);
	object->SetRenderLayer("UI");

	INSTANCE(CSceneManager).GetCurScene()->AddObject(object);

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

		float isStatic{};
		BinaryReader::ReadDateFromFile(ifs, isStatic);

		if (isStatic) {
			root->SetStatic(true);
		}
		else {
			root->SetStatic(false);
		}
	}

	if (root->mTag == "Obstacle") {
		root->mObjectType = OBJECT_TYPE::OBSTACLE;
	}
	else if (root->mTag == "Enemy") {
		root->mObjectType = OBJECT_TYPE::ENEMY;
	}
	else root->mObjectType = OBJECT_TYPE::NONE;
	return root;
}

std::shared_ptr<CGameObject> CGameObject::CreateObjectFromFile(const std::string& name)
{
	std::ifstream ifs{ OBJECT_PATH(name), std::ios::binary };
	if (!ifs) {
		return nullptr;
	}
	std::unordered_map<std::string, std::shared_ptr<CGameObject>> prefabs{};
	return CGameObject::CreateObjectFromFile(ifs, prefabs);
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
			obj->CreateTransformFromFile(inFile);

			std::string tag{};
			ReadDateFromFile(inFile, tag);
			obj->SetTag(tag);
			
			return obj;
		}
		if (token == "<Frame>:") {
			ReadDateFromFile(inFile, obj->mName);
		}
		else if (token == "<Tag>:") {
			ReadDateFromFile(inFile, obj->mTag);
		}
		else if (token == "<Transform>:") {
			obj->CreateTransformFromFile(inFile);
		}
		else if (token == "<Renderer>:") {
			obj->CreateRendererFromFile(inFile);
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
		else if (token == "<Animation>:") {
			std::string animName{};
			ReadDateFromFile(inFile, animName);
			size_t n = animName.find('@');
			if (n != std::string::npos) 
				animName = animName.substr(0, n + 1) + "anim";

			obj->CreateAnimationFromFile(ANIMATION_PATH(animName));
		}
		else if (token == "</Frame>") {
			break;
		}
	}

	

	return obj;
}

void CGameObject::CreateAnimationFromFile(std::string& fileName)
{
	using namespace BinaryReader;

	std::ifstream ifs{ fileName, std::ios::binary};
	if (!ifs) {
		return;
	}
	

	mAnimationController = AddComponent<CAnimationController>();

	std::string token{};
	int setsNum{};

	while (true) {
		ReadDateFromFile(ifs, token);

		if (token == "<AnimationSets>:") {
			ReadDateFromFile(ifs, setsNum);
			mAnimationController->mAnimationSets = std::make_shared<CAnimationSets>(setsNum);
		}
		else if (token == "<AnimationSet>:") {
			int setNum{};
			float startTime{};
			float endTime{};
			std::string setName;

			ReadDateFromFile(ifs, setNum);
			ReadDateFromFile(ifs, setName);
			ReadDateFromFile(ifs, startTime);
			ReadDateFromFile(ifs, endTime);

			auto animSet = std::make_shared<CAnimationSet>(startTime, endTime, setName);
			mAnimationController->mAnimationSets->mAnimationSet[setNum] = animSet;

			ReadDateFromFile(ifs, token);
			if (token == "<AnimationLayers>:") {
				int layersNum{};
				ReadDateFromFile(ifs, layersNum);

				animSet->mLayers.resize(layersNum);
				animSet->mScales.resize(layersNum);
				animSet->mRotations.resize(layersNum);
				animSet->mTranslations.resize(layersNum);

				for (int i = 0; i < layersNum; i++) {
					ReadDateFromFile(ifs, token);
					if (token == "<AnimationLayer>:") {
						int layerNum{};
						ReadDateFromFile(ifs, layerNum);

						animSet->mLayers[i] = std::make_shared<CAnimationLayer>();
						auto& layer = animSet->mLayers[i];

						int cacheNum{};
						ReadDateFromFile(ifs, cacheNum);

						layer->mBoneNames.resize(cacheNum);
						layer->mBoneFrameCaches.resize(cacheNum);
						layer->mAnimationCurves.resize(cacheNum);

						animSet->mScales[i].resize(cacheNum);
						animSet->mRotations[i].resize(cacheNum);
						animSet->mTranslations[i].resize(cacheNum);

						ReadDateFromFile(ifs, layer->mWeight);

						for (int j = 0; j < cacheNum; j++)
						{
							ReadDateFromFile(ifs, token);
							if (token == "<AnimationCurve>:") {
								int curveNude{};
								ReadDateFromFile(ifs, curveNude); //j
								ReadDateFromFile(ifs, layer->mBoneNames[j]);

								while (true) {
									ReadDateFromFile(ifs, token);

									if (token == "<TX>:") layer->LoadKeyValues(j, 0, ifs);
									else if (token == "<TY>:") layer->LoadKeyValues(j, 1, ifs);
									else if (token == "<TZ>:") layer->LoadKeyValues(j, 2, ifs);
									else if (token == "<RX>:") layer->LoadKeyValues(j, 3, ifs);
									else if (token == "<RY>:") layer->LoadKeyValues(j, 4, ifs);
									else if (token == "<RZ>:") layer->LoadKeyValues(j, 5, ifs);
									else if (token == "<SX>:") layer->LoadKeyValues(j, 6, ifs);
									else if (token == "<SY>:") layer->LoadKeyValues(j, 7, ifs);
									else if (token == "<SZ>:") layer->LoadKeyValues(j, 8, ifs);
									else if (token == "</AnimationCurve>")
									{
										break;
									}
								}
							}
						}
						ReadDateFromFile(ifs, token); //</AnimationLayer>
					}
				}
				ReadDateFromFile(ifs, token); //</AnimationLayers>

				//int64_t commonBoneNum = 0;
				//std::unordered_map<CTransform*, std::pair<int64_t, int64_t>> global_map;
				//
				//for (const auto& layer : animSet->mLayers) {
				//	std::unordered_map<CTransform*, int> local_count;
				//	for (const auto& cache : layer->mBoneFrameCaches) {
				//		local_count[cache.lock().get()]++;
				//	}
				//
				//	for (const auto& [cache, m] : local_count) {
				//		auto& sums = global_map[cache];
				//		sums.first += m;
				//		sums.second += static_cast<int64_t>(m) * m;
				//	}
				//}
				//
				//for (const auto& [cache, sums] : global_map) {
				//	int64_t sum_m = sums.first;
				//	int64_t sum_m2 = sums.second;
				//	int64_t contrib = (sum_m * sum_m - sum_m2) / 2;
				//	commonBoneNum += contrib;
				//}
				//
				//if (commonBoneNum) {
				//	animSet->mBoneFrameCaches.resize(commonBoneNum);
				//}
			}
			ReadDateFromFile(ifs, token); //</AnimationSet>
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
	ReadDateFromFile(inFile, mTransform->mLocalPosition);
	ReadDateFromFile(inFile, mTransform->mLocalEulerAngle);
	ReadDateFromFile(inFile, mTransform->mLocalScale);
	ReadDateFromFile(inFile, mTransform->mLocalRotation);

	ReadDateFromFile(inFile, mTransform->mLocalMat);

	//if (mName == "Premade_Necromancer") {
	//	std::cout << mTransform->mLocalScale.x << " " << mTransform->mLocalScale.y << " " << mTransform->mLocalScale.z << std::endl;
	//	std::cout << mTransform->mLocalEulerAngle.x << " " << mTransform->mLocalEulerAngle.y << " " << mTransform->mLocalEulerAngle.z << std::endl;
	//	std::cout << mTransform->mLocalPosition.x << " " << mTransform->mLocalPosition.y << " " << mTransform->mLocalPosition.z << std::endl;
	//	std::cout << mTransform->mLocalRotation.x << " " << mTransform->mLocalRotation.y << " " << mTransform->mLocalRotation.z << std::endl;
	//	std::cout << std::endl;
	//	PrintMatrix(mTransform->mLocalMat);
	//	std::cout << "======================" << std::endl;
	//}
}

void CGameObject::CreateRendererFromFile(std::ifstream& inFile)
{
	using namespace BinaryReader;
	std::string token{};

	ReadDateFromFile(inFile, token);
	if (token == "<SkinnedMesh>:") {
		auto skinnedMeshRenderer = AddComponent<CSkinnedMeshRenderer>();
		mRenderer = skinnedMeshRenderer;

		std::string meshName{};
		ReadDateFromFile(inFile, meshName);
		skinnedMeshRenderer->SetSkinnedMesh(meshName);

		UINT boneCount{};
		ReadDateFromFile(inFile, boneCount);
		skinnedMeshRenderer->mBoneNames.resize(boneCount);
		for (UINT i = 0; i < boneCount; i++) {
			std::string boneName{};
			ReadDateFromFile(inFile, boneName);
			skinnedMeshRenderer->mBoneNames[i] = boneName;
		}
	}
	else if (token == "<Mesh>:") {
		auto meshRenderer = AddComponent<CMeshRenderer>();
		mRenderer = meshRenderer;

		std::string meshName{};
		ReadDateFromFile(inFile, meshName);
		meshRenderer->SetMesh(meshName);
	}

	int materialCnt{};
	std::string materialName{};
	ReadDateFromFile(inFile, materialCnt);
	for (int i = 0; i < materialCnt; i++) {
		ReadDateFromFile(inFile, materialName);
		mRenderer->AddMaterial(materialName);
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
	terrain->MakeNavMap(name + "NavMap", resolution*2);

	INSTANCE(CSceneManager).GetCurScene()->SetTerrain(terrain);
}

void CGameObject::CacheFrameHierarchies(std::vector<std::shared_ptr<CGameObject>>& boneFrameCaches)
{
	boneFrameCaches.push_back(GetSptrFromThis());

	for (auto& child : mChildren) {
		child->CacheFrameHierarchies(boneFrameCaches);
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

void CGameObject::PrepareSkinning()
{
	mAnimationController->PrepareSkinning();
}

void CGameObject::UpdateWorldMatrices(std::shared_ptr<CTransform> parent)
{
	GetTransform()->UpdateWorldMatrix(parent, false);

	for (auto& child : mChildren) {
		child->UpdateWorldMatrices(mTransform);
	}
}

void CGameObject::PrintSRT()
{
	std::cout << "S     : " << mTransform->mLocalScale.x << " " << mTransform->mLocalScale.y << " " << mTransform->mLocalScale.z << std::endl;
	std::cout << "Euler : " << mTransform->mLocalEulerAngle.x << " " << mTransform->mLocalEulerAngle.y << " " << mTransform->mLocalEulerAngle.z << std::endl;
	std::cout << "T     : " << mTransform->mLocalPosition.x << " " << mTransform->mLocalPosition.y << " " << mTransform->mLocalPosition.z << std::endl;
	std::cout << "Quat  : " << mTransform->mLocalRotation.x << " " << mTransform->mLocalRotation.y << " " << mTransform->mLocalRotation.z << " " << mTransform->mLocalRotation.w << std::endl;
	std::cout << std::endl;
	std::cout << "로컬 변환 행렬" << std::endl;
	PrintMatrix(mTransform->mLocalMat);
	std::cout << std::endl;
	std::cout << "월드 변환 행렬" << std::endl;
	PrintMatrix(mTransform->mWorldMat);
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

