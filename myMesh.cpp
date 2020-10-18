#include "myMesh.h"
#include "framework.h"

void MyMesh::Initialize()
{
	pos		= VECTOR3(0, 0, 0);
	angle	= VECTOR3(0, 0, 0);
	scale	= VECTOR3(1, 1, 1);

	color = VECTOR4(1, 1, 1, 1);

	// ���f���ǂݍ���
	skinnedobj = nullptr;
	staticObj = nullptr;
	primitive = nullptr;
	staticObj = nullptr;
}

bool MyMesh::skinnedLoad(const char* fbxFilename,bool clockflg)
{
	skinnedobj = new Skinned_Mesh(pFramework->getDevice(), fbxFilename, clockflg);
	bLoad = true;

	return true;
}

bool MyMesh::staticLoad(const wchar_t* objfilename, const char* shaderfilename)
{
	staticObj = new Static_mesh(pFramework->getDevice(), objfilename, shaderfilename, false);
	bLoad = true;
	return true;
}

void MyMesh::SetPrimitive(GeometricPrimitive* _primitive)
{
	primitive = _primitive;
	bLoad = true;
}

void MyMesh::SetMesh(MyMesh& org)
{
	*this = org;
	bLoad = false;
}

void MyMesh::Release()
{
	if (primitive && bLoad)
	{
		delete primitive;
	}
	primitive = nullptr;
	bLoad = false;
}

void MyMesh::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir ,bool wireframe, float elapsedTime)
{
	if (primitive)
	{
		// ���[���h�ϊ�
		DirectX::XMMATRIX worldMatrix = GetWorldMatrix();

		// Matrix����Float4x4��
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&worldViewProjection, worldMatrix * view * projection);
		DirectX::XMStoreFloat4x4(&world, worldMatrix);

		primitive->render(pFramework->getDeviceContext(), worldViewProjection, world, light_dir, color, wireframe);

	}

	if (staticObj)
	{
		// ���[���h�ϊ�
		DirectX::XMMATRIX worldMatrix = GetWorldMatrix();

		// Matrix����Float4x4��
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&worldViewProjection, worldMatrix * view * projection);
		DirectX::XMStoreFloat4x4(&world, worldMatrix);

		staticObj->render(pFramework->getDeviceContext(), worldViewProjection, world, light_dir, color, wireframe);

	}

	if (skinnedobj)
	{
		// ���[���h�ϊ�
		DirectX::XMMATRIX worldMatrix = GetWorldMatrix();

		// Matrix����Float4x4��
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&worldViewProjection, worldMatrix * view * projection);
		DirectX::XMStoreFloat4x4(&world, worldMatrix);

		skinnedobj->render(pFramework->getDeviceContext(), worldViewProjection, world, light_dir, color, wireframe, elapsedTime);

	}
}

DirectX::XMMATRIX MyMesh::GetWorldMatrix()
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX Scale, Rotation, Rx, Ry, Rz, Translation;

	//	������
	world = DirectX::XMMatrixIdentity();
	//	�g��E�k��
	Scale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	��]
	Rx = DirectX::XMMatrixRotationX(angle.x);				//	X������Ƃ�����]�s��
	Ry = DirectX::XMMatrixRotationY(angle.y);				//	Y������Ƃ�����]�s��
	Rz = DirectX::XMMatrixRotationZ(angle.z);				//	Z������Ƃ�����]�s��
	Rotation = Rz * Ry * Rx;

	//	���s�ړ�
	Translation = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//	���[���h�ϊ��s��
	world = Scale * Rotation * Translation;


	return	world;
}
