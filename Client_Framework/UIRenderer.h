#pragma once
#include "stdafx.h"
#include "Renderer.h"

class CUIRenderer : public CRenderer
{
public:
	static std::shared_ptr<class CMesh> mQuad;
private:
	CBUIData mUIData{};
	std::shared_ptr<class CShader> mShader{};

public:
	CUIRenderer();
	CUIRenderer(const CUIRenderer& other) : CRenderer(other) {}
	virtual ~CUIRenderer();
public:
	void Render(std::shared_ptr<CCamera> camera, int pass = 0) override;
	std::shared_ptr<CComponent> Clone() override { return std::make_shared<CUIRenderer>(*this); }

public:
	void SetUIData(const CBUIData& data) { mUIData = data; }
	void SetColor(const Vec3& color) { mUIData.color = color; }
	void SetType(int type) { mUIData.type = type; }
	void SetTextureIdx(int idx) { mUIData.textureIdx = idx; }
	void SetTexture(const std::string& name);
	void SetScale(const Vec2& scale) { mUIData.scale = scale; }
	void SetPosition(const Vec2& position) { mUIData.position = position; }
	void SetSize(const Vec2& size) { mUIData.size = size; }
	void SetUVOffset(const Vec2& uvOffset) { mUIData.uvOffset = uvOffset; }
	void SetUVScale(const Vec2& uvScale) { mUIData.uvScale = uvScale; }

	CBUIData GetUIData() const { return mUIData; }
	Vec3 GetColor() const { return mUIData.color; }
	Vec2 GetSize() const { return mUIData.size; }
	Vec2 GetPosition() const { return mUIData.position; }
	Vec2 GetScale() const { return mUIData.scale; }
	Vec2 GetUVOffset() const { return mUIData.uvOffset; }
	Vec2 GetUVScale() const { return mUIData.uvScale; }
	int GetTextureIdx() const { return mUIData.textureIdx; }
	int GetType() const { return mUIData.type; }


};

