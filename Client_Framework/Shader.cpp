#include "stdafx.h"
#include "Shader.h"
#include"DX12Manager.h"

std::array<std::string, PASS_TYPE_COUNT> CShader::passName = { "Forward","GPass","Shadow","Stencil","Directional", "Lighting", "PostProcessing", "Final"};

void CShader::CreateGPUResource()
{
	if (d3dPiplineState) return;
	if (isLoaded) return;
	auto device = INSTANCE(CDX12Manager).GetDevice();
	HRESULT hr = device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&d3dPiplineState));
	if (FAILED(hr)) {
		OutputDebugStringA((name).c_str());
		return;
	}
	if (pipelineStateDesc.InputLayout.pInputElementDescs)
		delete[] pipelineStateDesc.InputLayout.pInputElementDescs;

	isLoaded = true;
}

D3D12_INPUT_LAYOUT_DESC CShader::InitInputLayout()
{
	D3D12_INPUT_ELEMENT_DESC* desc{};
	size_t elementNum{};

	switch (mInfo.inputLayoutYype)
	{
	case INPUT_LAYOUT_TYPE::NONE:
	{
		D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc{};
		d3dInputLayoutDesc.pInputElementDescs = nullptr;
		d3dInputLayoutDesc.NumElements = 0;
		return d3dInputLayoutDesc;
	}
		break;
	case INPUT_LAYOUT_TYPE::DEFAULT: 
	case INPUT_LAYOUT_TYPE::TERRAIN:
	{
		elementNum = 5;
		desc = new D3D12_INPUT_ELEMENT_DESC[elementNum];
		desc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[2] = { "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[3] = { "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 32,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[4] = { "COLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 44,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		break;
	}
	case INPUT_LAYOUT_TYPE::TEXTURE: {
		elementNum = 2;
		desc = new D3D12_INPUT_ELEMENT_DESC[elementNum];
		desc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		break;
	}
	case INPUT_LAYOUT_TYPE::BILLBOARD: {
		elementNum = 8;
		desc = new D3D12_INPUT_ELEMENT_DESC[elementNum];
		desc[0] = { "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 };
		desc[1] = { "POSITIONW",	0, DXGI_FORMAT_R32G32B32_FLOAT,		1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[2] = { "SIZE",			0, DXGI_FORMAT_R32G32_FLOAT,	   1, 12, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[3] = { "MATERIAL_IDX", 0, DXGI_FORMAT_R32_UINT,		   1, 20, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[4] = { "texMatrix",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 24, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[5] = { "texMatrix",	1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 40, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[6] = { "texMatrix",	2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 56, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[7] = { "texMatrix",	3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 72, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		break;
	}
	case INPUT_LAYOUT_TYPE::PARTICLE: {
		elementNum = 11;
		desc = new D3D12_INPUT_ELEMENT_DESC[elementNum];
		desc[0] = { "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 };
		desc[1] = { "POSITIONW",	0, DXGI_FORMAT_R32G32B32_FLOAT,		1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[2] = { "SIZE",			0, DXGI_FORMAT_R32G32_FLOAT,	   1, 12, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[3] = { "MATERIAL_IDX", 0, DXGI_FORMAT_R32_UINT,		   1, 20, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[4] = { "texMatrix",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 24, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[5] = { "texMatrix",	1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 40, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[6] = { "texMatrix",	2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 56, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[7] = { "texMatrix",	3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 72, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[8] = { "VELOCITY",	0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 88, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[9] = { "TYPE",	0, DXGI_FORMAT_R32_UINT, 1, 100, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[10] = { "AGE",	0, DXGI_FORMAT_R32_FLOAT, 1, 104, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		break;
	}
	case INPUT_LAYOUT_TYPE::ANIMATION: {
		elementNum = 7;
		desc = new D3D12_INPUT_ELEMENT_DESC[elementNum];
		desc[0] = { "POSITION",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[1] = { "TEXCOORD",		0,	DXGI_FORMAT_R32G32_FLOAT,		0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[2] = { "NORMAL",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[3] = { "TANGENT",		0,	DXGI_FORMAT_R32G32B32_FLOAT,	0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[4] = { "COLOR",		0,	DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[5] = { "BONEWEIGHTS",	0,	DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }; // ���� ����ġ
		desc[6] = { "BONEINDICES",	0,	DXGI_FORMAT_R32G32B32A32_UINT,	1, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }; // ���� �ε���
		break;
	}
	case INPUT_LAYOUT_TYPE::INSTANCE: {
		elementNum = 14;
		desc = new D3D12_INPUT_ELEMENT_DESC[elementNum];
		desc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, 12,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[2] = { "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 20,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[3] = { "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 32,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[4] = { "COLOR",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 44,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
		desc[5] = { "TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[6] = { "TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[7] = { "TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[8] = { "TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[9] = { "INVTRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[10] = { "INVTRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[11] = { "INVTRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[12] = { "INVTRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		desc[13] = { "INDEX", 0, DXGI_FORMAT_R32_SINT, 1, 128, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
		break;
	}
	default:
		break;
	}
	
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc{};
	d3dInputLayoutDesc.pInputElementDescs = desc;
	d3dInputLayoutDesc.NumElements = elementNum;

	return d3dInputLayoutDesc;
}

D3D12_RASTERIZER_DESC CShader::InitRasterizerState()
{
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	switch (mInfo.rasterizerType)
	{
	case RASTERIZER_TYPE::CULL_BACK:
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		break;
	case RASTERIZER_TYPE::CULL_FRONT:
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
		break;
	case RASTERIZER_TYPE::CULL_NONE:
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		break;
	case RASTERIZER_TYPE::WIREFRAME:
		rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
		rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		break;
	}
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return rasterizerDesc;
}

D3D12_BLEND_DESC CShader::InitBlendState()
{
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = TRUE;
	blendDesc.IndependentBlendEnable = FALSE;

	D3D12_RENDER_TARGET_BLEND_DESC& renderTarget = blendDesc.RenderTarget[0];
	renderTarget.LogicOpEnable = FALSE;
	renderTarget.LogicOp = D3D12_LOGIC_OP_NOOP;
	renderTarget.SrcBlendAlpha = D3D12_BLEND_ONE;
	renderTarget.DestBlendAlpha = D3D12_BLEND_ZERO;
	renderTarget.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	renderTarget.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	switch (mInfo.blendType)
	{
	case BLEND_TYPE::DEFAULT:
		renderTarget.BlendEnable = FALSE;
		renderTarget.SrcBlend = D3D12_BLEND_ONE;
		renderTarget.DestBlend = D3D12_BLEND_ZERO;
		renderTarget.BlendOp = D3D12_BLEND_OP_ADD;
		
		break;
	case BLEND_TYPE::ALPHA_BLEND:
		blendDesc.AlphaToCoverageEnable = FALSE;
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		renderTarget.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		renderTarget.BlendOp = D3D12_BLEND_OP_ADD;


		break;
	case BLEND_TYPE::ADD_BLEND:
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D12_BLEND_ONE;
		renderTarget.DestBlend = D3D12_BLEND_ONE;
		renderTarget.BlendOp = D3D12_BLEND_OP_ADD;
		break;
	case BLEND_TYPE::SUBTRACT_BLEND:
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D12_BLEND_ONE;
		renderTarget.DestBlend = D3D12_BLEND_ONE;
		renderTarget.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		break;
	case BLEND_TYPE::MULTIPLE_BLEND:
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D12_BLEND_ZERO;
		renderTarget.DestBlend = D3D12_BLEND_SRC_COLOR;
		renderTarget.BlendOp = D3D12_BLEND_OP_ADD;
		break;
	default:
		break;
	}
	

	if (mInfo.shaderType == PASS_TYPE::LIGHTING) {
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D12_BLEND_ONE;
		renderTarget.DestBlend = D3D12_BLEND_ONE;
		renderTarget.BlendOp = D3D12_BLEND_OP_ADD;
	}

	return blendDesc;
}

D3D12_DEPTH_STENCIL_DESC CShader::InitDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc{};
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;

	switch (mInfo.depthStencilType)
	{
	case DEPTH_STENCIL_TYPE::LESS:
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case DEPTH_STENCIL_TYPE::LESS_EQUAL:
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::GREATER:
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
		break;
	case DEPTH_STENCIL_TYPE::GREATER_EQUAL:
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::NO_DEPTH_TEST:
		d3dDepthStencilDesc.DepthEnable = FALSE;
		break;
	case DEPTH_STENCIL_TYPE::LESS_NO_WRITE:
		d3dDepthStencilDesc.DepthEnable = TRUE;
		d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	case DEPTH_STENCIL_TYPE::NO_DEPTH_TEST_NO_WRITE:
		d3dDepthStencilDesc.DepthEnable = FALSE;
		d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	default:
		break;
	}
	
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	switch (mInfo.shaderType)
	{
	case PASS_TYPE::DIRECTIONAL:
		d3dDepthStencilDesc.DepthEnable = TRUE;
		d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
#ifdef REVERSE_Z
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
#else // REVERSE_Z
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
#endif // REVERSE_Z
		break;
	case PASS_TYPE::STENCIL:
		d3dDepthStencilDesc.DepthEnable = TRUE;
#ifdef REVERSE_Z
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
#else // REVERSE_Z
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
#endif // REVERSE_Z
		d3dDepthStencilDesc.StencilWriteMask = 0xFF;
		d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR_SAT;
		d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR_SAT;
		d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		break;
	case PASS_TYPE::LIGHTING:
		d3dDepthStencilDesc.DepthEnable = TRUE;
#ifdef REVERSE_Z
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
#else // REVERSE_Z
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
#endif // REVERSE_Z
		d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL;
		d3dDepthStencilDesc.StencilEnable = TRUE;

		d3dDepthStencilDesc.StencilReadMask = 0xFF;
		//d3dDepthStencilDesc.StencilWriteMask = 0xFF;
		
		break;
	case PASS_TYPE::SHADOW:
		d3dDepthStencilDesc.DepthEnable = TRUE;
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		break;
	default:
		break;
	}

	return d3dDepthStencilDesc;
}

D3D12_SHADER_BYTECODE CShader::CreateShader(ComPtr<ID3DBlob>& blob, const std::string& name, const std::string& fname, const std::string& version)
{
	UINT32 compileFlag = 0;
#ifdef _DEBUG
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ComPtr<ID3DBlob> errBlob{};

	std::vector<D3D_SHADER_MACRO> macros;
	if (mInfo.inputLayoutYype == INPUT_LAYOUT_TYPE::ANIMATION)
		macros.push_back({ "USE_SKINNING", "1" });
	if (mInfo.inputLayoutYype == INPUT_LAYOUT_TYPE::INSTANCE)
		macros.push_back({ "USE_INSTANCING", "1" });
	if (mInfo.blendType != BLEND_TYPE::ALPHA_BLEND)
		macros.push_back({ "TRANSPARENT_CLIP", "1" });
	macros.push_back({ nullptr, nullptr });

	/*D3D_SHADER_MACRO macros[] = {
	{ "USE_SKINNING", mInfo.inputLayoutYype == INPUT_LAYOUT_TYPE::ANIMATION ? "1" : "0"},
	{ "TRANSPARENT_CLIP", mInfo.blendType == BLEND_TYPE::ALPHA_BLEND ? "0" : "1"},
	{ nullptr, nullptr }
	};*/

	std::string fileName = SHADER_PATH(name);
	std::wstring fileNameW = BinaryReader::stringToWstring(fileName);

	HRESULT hr = D3DCompileFromFile(fileNameW.c_str(), macros.data(),
		D3D_COMPILE_STANDARD_FILE_INCLUDE, fname.c_str(), version.c_str(), compileFlag, 0, &blob, &errBlob);

	if (FAILED(hr)) {
		if(errBlob) OutputDebugStringA((char*)errBlob->GetBufferPointer());
		else return {};
		//MessageBoxA(nullptr, (name + "Shader Create Failed !").c_str(), nullptr, MB_OK);
		if (blob) blob->Release();
		if (errBlob) errBlob->Release();
		return {};
	}
	D3D12_SHADER_BYTECODE bytecode{};
	bytecode.pShaderBytecode = blob->GetBufferPointer();
	bytecode.BytecodeLength = blob->GetBufferSize();

	return bytecode;
}

bool CShader::Initialize(const std::string& shaderName, const ShaderInfo& info, const std::string& name, bool getPassName)
{
	mInfo = info;

	pipelineStateDesc.pRootSignature = INSTANCE(CDX12Manager).GetRootSignature();
	std::string passType = passName[mInfo.shaderType];
	std::string sName = name;

	if (getPassName) {
		sName = passType;
	}

	pipelineStateDesc.VS = CreateShader(vsBlob, name, "VS_" + sName, "vs_5_1");
	pipelineStateDesc.PS = CreateShader(psBlob, name, "PS_" + sName, "ps_5_1");

	if (!vsBlob || !psBlob) {
		OutputDebugStringA((name + "Shader Create Failed !").c_str());
		return false;
	}

	if (mInfo.inputLayoutYype == INPUT_LAYOUT_TYPE::BILLBOARD || mInfo.inputLayoutYype == INPUT_LAYOUT_TYPE::PARTICLE) {
		pipelineStateDesc.GS = CreateShader(gsBlob, name, "GS_" + sName, "gs_5_1");
	}
	if (mInfo.inputLayoutYype == INPUT_LAYOUT_TYPE::TERRAIN) {
		pipelineStateDesc.HS = CreateShader(hsBlob, name, "HS_Forward", "hs_5_1");
		pipelineStateDesc.DS = CreateShader(dsBlob, name, "DS_Forward", "ds_5_1");
	}

	pipelineStateDesc.RasterizerState = InitRasterizerState();
	pipelineStateDesc.BlendState = InitBlendState();
	pipelineStateDesc.DepthStencilState = InitDepthStencilState();
	pipelineStateDesc.InputLayout = InitInputLayout();
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = info.topologyType;
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	switch (info.shaderType)
	{
	case PASS_TYPE::G_PASS:
		pipelineStateDesc.NumRenderTargets = 5;
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		pipelineStateDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		pipelineStateDesc.RTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		pipelineStateDesc.RTVFormats[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		pipelineStateDesc.RTVFormats[4] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case PASS_TYPE::FORWARD:
		pipelineStateDesc.NumRenderTargets = 1;
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case PASS_TYPE::SHADOW:
		pipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateDesc.NumRenderTargets = 0;
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;

		pipelineStateDesc.RasterizerState.DepthBias = 1000.f;
		pipelineStateDesc.RasterizerState.DepthBiasClamp = 0.0f;
		pipelineStateDesc.RasterizerState.SlopeScaledDepthBias = 3.0f;
		break;
	case PASS_TYPE::STENCIL:
		pipelineStateDesc.NumRenderTargets = 0;
		pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	}

	if(getPassName) this->name = shaderName + passType;
	else this->name = shaderName;
	return true;
}
