#include	"camera.h"

#include	"framework.h"
#include	"player.h"

extern	Player	player;

/*******************************************************************************
	「カメラ」クラスのコンストラクタ
*******************************************************************************/
Camera::Camera()
{
	pos = FLOAT3(.0f, 10.0f, -15.0f);
	target = FLOAT3(.0f, .0f, .0f);
	state = CAMERA_ACT::SurveillanceCamera;
	push_beforeflg = false;
	push_afterflg = false;

	float	fov = DirectX::XMConvertToRadians(30.0f);
	float	aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	SetPerspectiveMatrix(fov, aspect, 0.1f, 1000.0f);

	

}


/*******************************************************************************
	平行投影行列を算出する関数
*******************************************************************************/
DirectX::XMMATRIX	Camera::SetOrthographicMatrix(float w, float h, float znear, float zfar)
{
	projection = DirectX::XMMatrixOrthographicLH(w, h, znear, zfar);
	return	projection;
}


/*******************************************************************************
	透視投影行列を算出する関数
*******************************************************************************/
DirectX::XMMATRIX	Camera::SetPerspectiveMatrix(float fov, float aspect, float znear, float zfar)
{
	projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, znear, zfar);
	return	projection;
}


/*******************************************************************************
	ビュー行列を取得する関数
*******************************************************************************/
DirectX::XMMATRIX	Camera::GetViewMatrix()
{
	DirectX::XMVECTOR	p = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	DirectX::XMVECTOR	t = DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f);
	DirectX::XMVECTOR	up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	return	DirectX::XMMatrixLookAtLH(p, t, up);
}


/*******************************************************************************
TODO:04 カメラ用更新関数の実装
説明    状態の切り替え及び更新処理をここで行う
--------------------------------------------------------------------------------
手順    以下の内容を実装せよ
		・「状態」に応じた、カメラ更新処理の作成
		　ヒント：switchや関数ポインタを使うのが良いか？
		　　　　　又はデザインパターンの「移譲」
		・「Cキー」が押されたら、次の状態に切り替える
		　又、最後まで切り替わった場合は最初に戻るようにする
		　具体的には「1人称カメラ(FPS)」→「固定カメラ」
		※各状態の詳細は次以降のTODOで実装

		カメラの切り替えを確認したい場合は以下のデバッグプログラムを入れると良い

		char	str[64];
		sprintf_s(str, 64, "CameraMode:%d", 「状態」);
		SetText(str, 5, 5);
*******************************************************************************/
void Camera::Updata()
{
	push_afterflg = false;
	switch (state)
	{
	case SurveillanceCamera:
		SLCamera();
		if (GetAsyncKeyState('C') < 0)
		{
			push_afterflg = true;
			if (push_afterflg != push_beforeflg)
				state = FixedRelativePositionCamera;
		}
		break;
	case FixedRelativePositionCamera:
		FRPCamera();
		if (GetAsyncKeyState('C') < 0)
		{
			push_afterflg = true;
			if (push_afterflg != push_beforeflg)
				state = TrackingCamera;
		}
		break;
	case TrackingCamera:
		TCamera();
		if (GetAsyncKeyState('C') < 0)
		{
			push_afterflg = true;
			if (push_afterflg != push_beforeflg)
				state = ThirdPersonCamera;
		}
		break;
	case ThirdPersonCamera:
		TPCamera();
		if (GetAsyncKeyState('C') < 0)
		{
			push_afterflg = true;
			if (push_afterflg != push_beforeflg)
				state = FirstPersonCamera;
		}
		break;
	case FirstPersonCamera:
		FPCamera();
		if (GetAsyncKeyState('C') < 0)
		{
			push_afterflg = true;
			if (push_afterflg != push_beforeflg)
				state = SurveillanceCamera;
		}
		break;
	}

	framework::getInstance()->debug->setString("Camera.target.x:%2.f", target.x);
	framework::getInstance()->debug->setString("Camera.target.y:%2.f", target.y);
	framework::getInstance()->debug->setString("Camera.target.z:%2.f", target.z);
	push_beforeflg = push_afterflg;
	// state = (state +1) % CAMERA_ACT::MaximumCameraManagementNumber; スイッチの切り替え

}



/*******************************************************************************
TODO:06 「監視カメラ」の実装
説明    「監視カメラ」とは「位置」は変化しないが
		常にプレイヤーを捉え続けようとするカメラを指す
--------------------------------------------------------------------------------
手順    カメラの「位置」と「注視点」を以下のように修正せよ
		・「位置」	：(0, 10, -15)固定
		・「注視点」：常にプレイヤーを見続ける
確認	「監視カメラ」切り替え時に、プレイヤーが移動しても
		常にプレイヤーを見続けるようになればOK
*******************************************************************************/
//	監視カメラ関数
void Camera::SLCamera()
{
	pos = FLOAT3(0.0f, 10.0f, -15.0f);
	// target = player.pos;
	target = FLOAT3(0.0f, 0.0f, 0.0f);
}



/*******************************************************************************
TODO:07 「相対位置固定カメラ」の実装
説明    「相対位置固定カメラ」とはプレイヤーとの相対位置が常に一定のカメラを指す
--------------------------------------------------------------------------------
手順    カメラの「位置」と「注視点」を以下のように修正せよ
		・「位置」	：プレイヤーの「位置」から(+8,+5,-8)離れた位置に在るようにする
		・「注視点」：常にプレイヤーを見続ける
確認	「相対位置固定カメラ」切り替え時に、常にプレイヤーとの距離が一定になればOK
		但しこの時、プレイヤーの向きには依存しない
*******************************************************************************/
//	相対位置固定カメラ関数
void Camera::FRPCamera()
{
	pos.x = (player.pos.x + 8.0f);
	pos.y = (player.pos.y + 7.0f);
	pos.z = (player.pos.z - 8.0f);

	target.x = player.pos.x;
	target.y = player.pos.y + 5.0f;
	target.z = player.pos.z;
}



/*******************************************************************************
TODO:08 「3人称カメラ」の実装
説明    「3人称カメラ」とはプレイヤーの後方から俯瞰するような視線のカメラを指す
--------------------------------------------------------------------------------
手順    カメラの「位置」と「注視点」を以下のように修正せよ
		・「位置」	：プレイヤーの『後方』、XZ平面上にて距離12の位置
		・「注視点」：常にプレイヤーを見続ける
確認	「3人称カメラ」切り替え時に、『後方』から俯瞰するようになればOK
		※「相対位置固定カメラ」と「3人称カメラ」の間に「追跡カメラ」があるが、
		　後程実装するので今は気にしない
*******************************************************************************/
//	3人称カメラ(TPS)関数
void Camera::TPCamera()
{
	const float Distance = 20.0f;

	float dx = sinf(player.angle.y);
	float dz = cosf(player.angle.y);

	pos.x = player.pos.x - Distance * dx;
	pos.y = player.pos. y + 5.0f;
	pos.z = player.pos.z - Distance * dz;

	target.x = player.pos.x;
	target.y = player.pos.y + 5.0f;
	target.z = player.pos.z;
}




/*******************************************************************************
TODO:09 「1人称カメラ」の実装
説明    「1人称カメラ」とはプレイヤーの目線の位置にカメラを配置したようなカメラを指す
--------------------------------------------------------------------------------
手順    カメラの「位置」と「注視点」を以下のように修正せよ
		・「位置」	：プレイヤーの「位置」から高さ0.75の位置
		・「注視点」：カメラの「位置」からプレイヤー『前方』に距離1の座標
確認	「1人称カメラ」切り替え時に、戦車目線になればOK
*******************************************************************************/
//	1人称カメラ(FPS)関数
void Camera::FPCamera()
{
	const float Height = 1.5f;

	float dx = sinf(player.angle.y);
	float dz = cosf(player.angle.y);

	pos.x = player.pos.x;
	pos.y = player.pos.y + Height;
	pos.z = player.pos.z;

	target.x = pos.x + dx;
	target.y = pos.y;
	target.z = pos.z + dz;

}


/*******************************************************************************
TODO:10「追跡カメラ」の実装
説明    「追跡カメラ」とは一定範囲の距離を保ちつつ追跡するカメラを指す
		「相対位置固定カメラ」との違いは、「距離」しか見ていないので、
		角度が変化する
--------------------------------------------------------------------------------
手順    カメラの「位置」と「注視点」を以下のように修正せよ
		・「位置」	：XZ平面に関して、プレイヤーの「位置」とカメラの「位置」から距離を求め、
　					　距離が 5未満なら距離が 5になるように修正(向きは維持)
 　					　距離が15以上なら距離が15になるように修正(向きは維持)
  　				　上記以外なら特に修正しない
					　なお、YはプレイヤーのY座標+5した位置とする
   　	　※ヒント：2点間の距離は「三平方の定理」参照、平方根はsqrtf関数参照
		・「注視点」：常にプレイヤーを見続ける
確認	「追跡カメラ」切り替え時に、プレイヤーを追い続けるようになればOK
		但し、距離5～15の間はカメラは移動しない
		※実装順番は「3人称カメラ」の後だが、切り替えの順番は
		　「相対位置固定カメラ」の次にする事
*******************************************************************************/
//	追跡カメラ関数
void Camera::TCamera()
{
	const float minDistance = 10.0f;
	const float maxDistance = 20.0f;

	float dx = player.pos.x - pos.x;
	float dz = player.pos.z - pos.z;

	float nowDistance = sqrtf((dx * dx) + (dz * dz));

	if (nowDistance < minDistance)
	{
		dx /= nowDistance;
		dz /= nowDistance;

		pos.x = player.pos.x - (minDistance * dx);
		pos.z = player.pos.z - (minDistance * dz);
	}
	if (nowDistance >= maxDistance)
	{
		dx /= nowDistance;
		dz /= nowDistance;

		pos.x = player.pos.x - (maxDistance * dx);
		pos.z = player.pos.z - (maxDistance * dz);
	}
	pos.y = player.pos.y + 5.0f;

	target.x = player.pos.x;
	target.y = player.pos.y + 5.0f;
	target.z = player.pos.z;

}