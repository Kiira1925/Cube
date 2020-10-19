
#include "player.h"

#include "framework.h"
#include "blender.h"
const float upS = 0.5f;
const float second = 30;
//void Player::Initialize(GeometricPrimitive* _primitive)
//{
//	obj.Initialize();
//	obj.SetPrimitive(_primitive);
//
//	speed.y = upS;
//	axis.x = 0.0f;
//	axis.y = 1.0f;
//	axis.z = 0.0f;
//	axis.w = 1.0f;
//	orientation.x = axis.x * sinf(0 / 2);
//	orientation.y = axis.y * sinf(0 / 2);
//	orientation.z = axis.z * sinf(0 / 2);
//	orientation.w = cosf(0 / 2);
//	scale = obj.scale;
//	color = obj.color;
//}

void Player::Initialize(const char* fileName)
{
	obj.Initialize();
	obj.skinnedLoad(fileName);

	speed.y = upS;
	axis.x = 0.0f;
	axis.y = 1.0f;
	axis.z = 0.0f;
	axis.w = 1.0f;
	orientation.x = axis.x * sinf(0 / 2);
	orientation.y = axis.y * sinf(0 / 2);
	orientation.z = axis.z * sinf(0 / 2);
	orientation.w = cosf(0 / 2);
	scale = obj.scale;
	color = obj.color;
}

void Player::InitStatus()
{
	pos.y = 0;
	axis.x = 0.0f;
	axis.y = 1.0f;
	axis.z = 0.0f;
	axis.w = 1.0f;
	orientation.x = axis.x * sinf(0 / 2);
	orientation.y = axis.y * sinf(0 / 2);
	orientation.z = axis.z * sinf(0 / 2);
	orientation.w = cosf(0 / 2);
	moveAngle = 0;
	speed = VECTOR3(0.0f, upS, 0.0f);
	timer = 0;
}

void Player::Move()
{
	const float dangle = DirectX::XMConvertToRadians(90.0f / second);		//	1度
	const float moveSpeed = 1.0f;/* / 60; 一秒で進める距離	*/								//	プレイヤーの速度

	if (speed.x == 0 && speed.z == 0)
	{
		//	回転
		if (GetAsyncKeyState(VK_LEFT) < 0)
		{
			axis.x = 0.0f;
			axis.y = 0.0f;
			axis.z = 1.0f;
			moveAngle += dangle;
			speed.x -= moveSpeed;
		}
		else if (GetAsyncKeyState(VK_RIGHT) < 0)
		{
			axis.x = 0.0f;
			axis.y = 0.0f;
			axis.z = 1.0f;
			moveAngle -= dangle;
			speed.x += moveSpeed;
		}
		else if (GetAsyncKeyState(VK_UP) < 0)
		{
			axis.x = 1.0f;
			axis.y = 0.0f;
			axis.z = 0.0f;
			moveAngle += dangle;
			speed.z += moveSpeed;
		}
		else if (GetAsyncKeyState(VK_DOWN) < 0)
		{
			axis.x = 1.0f;
			axis.y = 0.0f;
			axis.z = 0.0f;
			moveAngle -= dangle;
			speed.z -= moveSpeed;
		}
	}
	DirectX::XMVECTOR qua, Axis, delta, def;
	// 上で設定した軸( axis )を XMVECTOR ( Axis )に変換する
	Axis = XMLoadFloat4(&axis);
	// 軸( axis )をもとに moveAngle度 回転させたクォータニオンを作る
	delta = XMQuaternionRotationAxis(Axis, moveAngle);
	// 自機が持つ角度( float4 )を XMVECTOR に入れる
	def = XMLoadFloat4(&orientation);
	// クォータニオン同士を合成する
	qua = XMQuaternionMultiply(def, delta);
	// Float4 に戻す
	XMStoreFloat4(&orientation, qua);

	framework::getInstance()->debug->setString("Pl.moveAngle:%f", moveAngle);

	framework::getInstance()->debug->setString("Pl.speed.x:%f", speed.x);
	framework::getInstance()->debug->setString("Pl.speed.y:%f", speed.y);
	framework::getInstance()->debug->setString("Pl.speed.z:%f", speed.z);
	framework::getInstance()->debug->setString("Pl.axis.x:%2.f", axis.x);
	framework::getInstance()->debug->setString("Pl.axis.y:%2.f", axis.y);
	framework::getInstance()->debug->setString("Pl.axis.z:%2.f", axis.z);
	if (speed.x != 0 || speed.z != 0)
	{

		pos.x += speed.x / second;
		pos.y += speed.y / second;
		pos.z += speed.z / second;
		timer++;
		speed.y -= 0.01;
		if (timer == second/2)
		{
			speed.y *= -1;
		}
		if (timer >= second)
		{
			pos.y = 0;
			axis.x = 0.0f;
			axis.y = 1.0f;
			axis.z = 0.0f;
			axis.w = 1.0f;
			moveAngle = 0;
			speed = VECTOR3(0.0f, upS, 0.0f);
			timer = 0;
			pFramework->soundSE[2]->Play(false);

		}
	}

}

void Player::Release()
{
	obj.Release();
}

void Player::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& lightDir, bool wireframe,float elapsedTime)
{
	obj.pos = this->pos;

	if (obj.primitive)
	{
		// ワールド変換
		DirectX::XMMATRIX worldMatrix = GetWorldMatrix();

		// MatrixからFloat4x4へ
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&worldViewProjection, worldMatrix * view * projection);
		DirectX::XMStoreFloat4x4(&world, worldMatrix);

		obj.primitive->render(pFramework->getDeviceContext(), worldViewProjection, world, lightDir, color, wireframe);

	}

	if (obj.staticObj)
	{
		// ワールド変換
		DirectX::XMMATRIX worldMatrix = GetWorldMatrix();

		// MatrixからFloat4x4へ
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&worldViewProjection, worldMatrix * view * projection);
		DirectX::XMStoreFloat4x4(&world, worldMatrix);

		obj.staticObj->render(pFramework->getDeviceContext(), worldViewProjection, world, lightDir, color, wireframe);

	}

	if (obj.skinnedobj)
	{
		// ワールド変換
		DirectX::XMMATRIX worldMatrix = GetWorldMatrix();

		// MatrixからFloat4x4へ
		DirectX::XMFLOAT4X4 worldViewProjection;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&worldViewProjection, worldMatrix * view * projection);
		DirectX::XMStoreFloat4x4(&world, worldMatrix);

		obj.skinnedobj->render(pFramework->getDeviceContext(), worldViewProjection, world, lightDir, color, wireframe, elapsedTime);

	}

	// 整数を文字列に変換
	framework::getInstance()->debug->setString("Pl.Pos.x:%f", pos.x);
	framework::getInstance()->debug->setString("Pl.Pos.y:%f", pos.y);
	framework::getInstance()->debug->setString("Pl.Pos.z:%f", pos.z);
	//   obj.Render(view, projection, lightDir, wireframe);
}

DirectX::XMMATRIX Player::GetWorldMatrix()
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX Scale, Rotation, Translation;


	//	初期化
	world = DirectX::XMMatrixIdentity();
	//	拡大・縮小
	Scale = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	回転
	DirectX::XMVECTOR qua;
	// xmvector にfloat4を入れる
	qua = XMLoadFloat4(&orientation);
	// quaternion から回転行列を作る
	Rotation = XMMatrixRotationQuaternion(qua);



	//	平行移動
	Translation = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//	ワールド変換行列
	world = Scale * Rotation * Translation;


	return	world;
}
