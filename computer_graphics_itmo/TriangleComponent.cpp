#include "Headers/TriangleComponent.h"
#include "Headers/Game.h"
#include "Headers/DisplayWin32.h"
#include <d3dcompiler.h>
#include <directxmath.h>
#include <iostream>

#pragma comment(lib, "d3dcompiler.lib")

TriangleComponent::TriangleComponent(Game* game)
	: GameComponent(game)
	, Layout(nullptr)
	, vertexShader_(nullptr)
	, vertexShaderByteCode_(nullptr)
	, pixelShader_(nullptr)
	, pixelShaderByteCode_(nullptr)
	, vertexBuffer_(nullptr)
	, indexBuffer_(nullptr) 
	, rasterState_(nullptr)
	, indexCount_(0)
{
}

TriangleComponent::~TriangleComponent()
{
	DestroyResources();
}

void TriangleComponent::Initialize()
{
	ID3D11Device* Device = game->GetDevice();

	ID3DBlob* ErrorCode = nullptr;
	auto Result = D3DCompileFromFile(
		L"./Shaders/MyVeryFirstShader.hlsl",
		nullptr,
		nullptr,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexShaderByteCode_,
		&ErrorCode);

	if (FAILED(Result))
	{
		if (ErrorCode)
		{
			char* CompileErrors = static_cast<char*>(ErrorCode->GetBufferPointer());
			std::cout << CompileErrors << std::endl;
		}
		else
		{
			MessageBox(
				game->GetDisplay()->GetWindowHandle(),
				L"MyVeryFirstShader.hlsl",
				L"Missing Shader File",
				MB_OK);
		}
		return;
	}

	D3D_SHADER_MACRO ShaderMacros[] = {
		"TEST", "1",
		"TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)",
		nullptr, nullptr
	};

	ID3DBlob* PixelErrorCode = nullptr;
	Result = D3DCompileFromFile(
		L"./Shaders/MyVeryFirstShader.hlsl",
		ShaderMacros,
		nullptr,
		"PSMain",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShaderByteCode_,
		&PixelErrorCode);

	if (FAILED(Result))
	{
		if (PixelErrorCode)
		{
			char* CompileErrors = static_cast<char*>(PixelErrorCode->GetBufferPointer());
			std::cout << CompileErrors << std::endl;
		}
		return;
	}

	Device->CreateVertexShader(
		vertexShaderByteCode_->GetBufferPointer(),
		vertexShaderByteCode_->GetBufferSize(),
		nullptr,
		&vertexShader_);

	Device->CreatePixelShader(
		pixelShaderByteCode_->GetBufferPointer(),
		pixelShaderByteCode_->GetBufferSize(),
		nullptr,
		&pixelShader_);

	D3D11_INPUT_ELEMENT_DESC InputElements[] = {
		D3D11_INPUT_ELEMENT_DESC{
			"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		D3D11_INPUT_ELEMENT_DESC{
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	Device->CreateInputLayout(
		InputElements,
		2,
		vertexShaderByteCode_->GetBufferPointer(),
		vertexShaderByteCode_->GetBufferSize(),
		&Layout);

	DirectX::XMFLOAT4 Points[8] = {
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),   DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),//������ offset
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),  DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),   DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),   DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};

	D3D11_BUFFER_DESC VertexBufferDescription = {};
	VertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	VertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDescription.CPUAccessFlags = 0;
	VertexBufferDescription.MiscFlags = 0;
	VertexBufferDescription.StructureByteStride = 0;
	VertexBufferDescription.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(Points);

	D3D11_SUBRESOURCE_DATA VertexData = {};
	VertexData.pSysMem = Points;
	VertexData.SysMemPitch = 0;
	VertexData.SysMemSlicePitch = 0;

	Device->CreateBuffer(&VertexBufferDescription, &VertexData, &vertexBuffer_);

	int Indices[] = { 0, 1, 2, 1, 0, 3 };
	indexCount_ = static_cast<UINT>(std::size(Indices));

	D3D11_BUFFER_DESC IndexBufferDescription = {};
	IndexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDescription.CPUAccessFlags = 0;
	IndexBufferDescription.MiscFlags = 0;
	IndexBufferDescription.StructureByteStride = 0;
	IndexBufferDescription.ByteWidth = sizeof(int) * indexCount_;

	D3D11_SUBRESOURCE_DATA IndexData = {};
	IndexData.pSysMem = Indices;
	IndexData.SysMemPitch = 0;
	IndexData.SysMemSlicePitch = 0;

	Device->CreateBuffer(&IndexBufferDescription, &IndexData, &indexBuffer_);

	CD3D11_RASTERIZER_DESC RasterizerDescription = {};
	RasterizerDescription.CullMode = D3D11_CULL_NONE;
	RasterizerDescription.FillMode = D3D11_FILL_SOLID;

	Device->CreateRasterizerState(&RasterizerDescription, &rasterState_);
}

void TriangleComponent::Update(float DeltaTime)
{
}

void TriangleComponent::Draw()
{
	ID3D11DeviceContext* DeviceContext = game->GetDeviceContext();

	DeviceContext->RSSetState(rasterState_);
	DeviceContext->IASetInputLayout(Layout);
	DeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DeviceContext->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	UINT Strides[] = { 32 };
	UINT Offsets[] = { 0 };
	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer_, Strides, Offsets);

	DeviceContext->VSSetShader(vertexShader_, nullptr, 0);
	DeviceContext->PSSetShader(pixelShader_, nullptr, 0);

	DeviceContext->DrawIndexed(indexCount_, 0, 0);
}

void TriangleComponent::DestroyResources()
{
	if (Layout)
	{
		Layout->Release();
		Layout = nullptr;
	}

	if (vertexShader_)
	{
		vertexShader_->Release();
		vertexShader_ = nullptr;
	}

	if (vertexShaderByteCode_)
	{
		vertexShaderByteCode_->Release();
		vertexShaderByteCode_ = nullptr;
	}

	if (pixelShader_)
	{
		pixelShader_->Release();
		pixelShader_ = nullptr;
	}

	if (pixelShaderByteCode_)
	{
		pixelShaderByteCode_->Release();
		pixelShaderByteCode_ = nullptr;
	}

	if (vertexBuffer_)
	{
		vertexBuffer_->Release();
		vertexBuffer_ = nullptr;
	}

	if (indexBuffer_)
	{
		indexBuffer_->Release();
		indexBuffer_ = nullptr;
	}

	if (rasterState_)
	{
		rasterState_->Release();
		rasterState_ = nullptr;
	}
}
