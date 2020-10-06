#pragma once

#include	<DirectXMath.h>
#include	"vector.h"


/*******************************************************************************
	「カメラ」クラス
*******************************************************************************/
class Camera
{
public:
	/*******************************************************************************
	TODO:01 カメラ状態管理番号の宣言
	説明    前回の敵行動管理番号同様、カメラについても管理番号を宣言する
	--------------------------------------------------------------------------------
	手順    enumで以下の名称の定数を宣言せよ
			・「監視カメラ」
			・「相対位置固定カメラ」
			・「追跡カメラ」
			・「3人称カメラ(TPS)」
			・「1人称カメラ(FPS)」
			・「カメラ管理番号最大値」	←これは無くても良いが、あると便利
	*******************************************************************************/
	enum CAMERA_ACT
	{
		SurveillanceCamera,
		FixedRelativePositionCamera,
		TrackingCamera,
		ThirdPersonCamera,
		FirstPersonCamera,
		MaximumCameraManagementNumber,
	};


private:
	/*******************************************************************************
	TODO:02 カメラ状態保存用変数の定義
	説明    TODO01で作成したenumを保存する為の変数を作成する
			今回は扱い易さ優先でint型で宣言する
	--------------------------------------------------------------------------------
	手順    int型変数「状態」を定義する
			また、コンストラクタ内で「監視カメラ」の値で初期化する
	*******************************************************************************/
	int	state;

public:
	VECTOR3	pos;				//	「位置(座標)」
	VECTOR3	target;				//	「注視点」
	DirectX::XMMATRIX	projection;			//	「投影行列」

	Camera();

	/*******************************************************************************
	TODO:03 カメラ更新関数の宣言
	説明    カメラの切り替え、または更新を行う為の関数を用意する
	--------------------------------------------------------------------------------
	手順    以下のメンバ関数を追加する
			・void 更新()
			※必要に応じてメンバを追加しても良い
	*******************************************************************************/
	void Updata();

	bool push_beforeflg;
	bool push_afterflg;

	void SLCamera();
	void FRPCamera();
	void TCamera();
	void TPCamera();
	void FPCamera();




	DirectX::XMMATRIX	SetOrthographicMatrix(float w, float h, float znear, float zfar);			//	平行投影行列設定関数
	DirectX::XMMATRIX	SetPerspectiveMatrix(float fov, float aspect, float znear, float zfar);	//	透視投影行列設定関数
	DirectX::XMMATRIX	GetViewMatrix();															//	ビュー行列取得関数
	DirectX::XMMATRIX	GetProjectionMatrix() { return	projection; }								//	投影行列取得関数

};