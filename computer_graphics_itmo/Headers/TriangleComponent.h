#pragma once

#include "GameComponent.h"
#include <d3d11.h>

class TriangleComponent : public GameComponent
{
public:
	TriangleComponent(Game* game);
	~TriangleComponent() override;

	void Initialize() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void DestroyResources() override;

private:
	ID3D11InputLayout* Layout;
	ID3D11VertexShader* vertexShader_;
	ID3DBlob* vertexShaderByteCode_;
	ID3D11PixelShader* pixelShader_;
	ID3DBlob* pixelShaderByteCode_;
	ID3D11Buffer* vertexBuffer_;
	ID3D11Buffer* indexBuffer_;
	ID3D11RasterizerState* rasterState_;
	UINT indexCount_;
};
