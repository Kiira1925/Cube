#include	"camera.h"

#include	"framework.h"
#include	"player.h"

extern	Player	player;

/*******************************************************************************
	�u�J�����v�N���X�̃R���X�g���N�^
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
	���s���e�s����Z�o����֐�
*******************************************************************************/
DirectX::XMMATRIX	Camera::SetOrthographicMatrix(float w, float h, float znear, float zfar)
{
	projection = DirectX::XMMatrixOrthographicLH(w, h, znear, zfar);
	return	projection;
}


/*******************************************************************************
	�������e�s����Z�o����֐�
*******************************************************************************/
DirectX::XMMATRIX	Camera::SetPerspectiveMatrix(float fov, float aspect, float znear, float zfar)
{
	projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, znear, zfar);
	return	projection;
}


/*******************************************************************************
	�r���[�s����擾����֐�
*******************************************************************************/
DirectX::XMMATRIX	Camera::GetViewMatrix()
{
	DirectX::XMVECTOR	p = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	DirectX::XMVECTOR	t = DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f);
	DirectX::XMVECTOR	up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	return	DirectX::XMMatrixLookAtLH(p, t, up);
}


/*******************************************************************************
TODO:04 �J�����p�X�V�֐��̎���
����    ��Ԃ̐؂�ւ��y�эX�V�����������ōs��
--------------------------------------------------------------------------------
�菇    �ȉ��̓��e����������
		�E�u��ԁv�ɉ������A�J�����X�V�����̍쐬
		�@�q���g�Fswitch��֐��|�C���^���g���̂��ǂ����H
		�@�@�@�@�@���̓f�U�C���p�^�[���́u�ڏ��v
		�E�uC�L�[�v�������ꂽ��A���̏�Ԃɐ؂�ւ���
		�@���A�Ō�܂Ő؂�ւ�����ꍇ�͍ŏ��ɖ߂�悤�ɂ���
		�@��̓I�ɂ́u1�l�̃J����(FPS)�v���u�Œ�J�����v
		���e��Ԃ̏ڍׂ͎��ȍ~��TODO�Ŏ���

		�J�����̐؂�ւ����m�F�������ꍇ�͈ȉ��̃f�o�b�O�v���O����������Ɨǂ�

		char	str[64];
		sprintf_s(str, 64, "CameraMode:%d", �u��ԁv);
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
	// state = (state +1) % CAMERA_ACT::MaximumCameraManagementNumber; �X�C�b�`�̐؂�ւ�

}



/*******************************************************************************
TODO:06 �u�Ď��J�����v�̎���
����    �u�Ď��J�����v�Ƃ́u�ʒu�v�͕ω����Ȃ���
		��Ƀv���C���[�𑨂������悤�Ƃ���J�������w��
--------------------------------------------------------------------------------
�菇    �J�����́u�ʒu�v�Ɓu�����_�v���ȉ��̂悤�ɏC������
		�E�u�ʒu�v	�F(0, 10, -15)�Œ�
		�E�u�����_�v�F��Ƀv���C���[����������
�m�F	�u�Ď��J�����v�؂�ւ����ɁA�v���C���[���ړ����Ă�
		��Ƀv���C���[����������悤�ɂȂ��OK
*******************************************************************************/
//	�Ď��J�����֐�
void Camera::SLCamera()
{
	pos = FLOAT3(0.0f, 10.0f, -15.0f);
	// target = player.pos;
	target = FLOAT3(0.0f, 0.0f, 0.0f);
}



/*******************************************************************************
TODO:07 �u���Έʒu�Œ�J�����v�̎���
����    �u���Έʒu�Œ�J�����v�Ƃ̓v���C���[�Ƃ̑��Έʒu����Ɉ��̃J�������w��
--------------------------------------------------------------------------------
�菇    �J�����́u�ʒu�v�Ɓu�����_�v���ȉ��̂悤�ɏC������
		�E�u�ʒu�v	�F�v���C���[�́u�ʒu�v����(+8,+5,-8)���ꂽ�ʒu�ɍ݂�悤�ɂ���
		�E�u�����_�v�F��Ƀv���C���[����������
�m�F	�u���Έʒu�Œ�J�����v�؂�ւ����ɁA��Ƀv���C���[�Ƃ̋��������ɂȂ��OK
		�A�����̎��A�v���C���[�̌����ɂ͈ˑ����Ȃ�
*******************************************************************************/
//	���Έʒu�Œ�J�����֐�
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
TODO:08 �u3�l�̃J�����v�̎���
����    �u3�l�̃J�����v�Ƃ̓v���C���[�̌��������Ղ���悤�Ȏ����̃J�������w��
--------------------------------------------------------------------------------
�菇    �J�����́u�ʒu�v�Ɓu�����_�v���ȉ��̂悤�ɏC������
		�E�u�ʒu�v	�F�v���C���[�́w����x�AXZ���ʏ�ɂċ���12�̈ʒu
		�E�u�����_�v�F��Ƀv���C���[����������
�m�F	�u3�l�̃J�����v�؂�ւ����ɁA�w����x������Ղ���悤�ɂȂ��OK
		���u���Έʒu�Œ�J�����v�Ɓu3�l�̃J�����v�̊ԂɁu�ǐՃJ�����v�����邪�A
		�@�����������̂ō��͋C�ɂ��Ȃ�
*******************************************************************************/
//	3�l�̃J����(TPS)�֐�
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
TODO:09 �u1�l�̃J�����v�̎���
����    �u1�l�̃J�����v�Ƃ̓v���C���[�̖ڐ��̈ʒu�ɃJ������z�u�����悤�ȃJ�������w��
--------------------------------------------------------------------------------
�菇    �J�����́u�ʒu�v�Ɓu�����_�v���ȉ��̂悤�ɏC������
		�E�u�ʒu�v	�F�v���C���[�́u�ʒu�v���獂��0.75�̈ʒu
		�E�u�����_�v�F�J�����́u�ʒu�v����v���C���[�w�O���x�ɋ���1�̍��W
�m�F	�u1�l�̃J�����v�؂�ւ����ɁA��Ԗڐ��ɂȂ��OK
*******************************************************************************/
//	1�l�̃J����(FPS)�֐�
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
TODO:10�u�ǐՃJ�����v�̎���
����    �u�ǐՃJ�����v�Ƃ͈��͈͂̋�����ۂ��ǐՂ���J�������w��
		�u���Έʒu�Œ�J�����v�Ƃ̈Ⴂ�́A�u�����v�������Ă��Ȃ��̂ŁA
		�p�x���ω�����
--------------------------------------------------------------------------------
�菇    �J�����́u�ʒu�v�Ɓu�����_�v���ȉ��̂悤�ɏC������
		�E�u�ʒu�v	�FXZ���ʂɊւ��āA�v���C���[�́u�ʒu�v�ƃJ�����́u�ʒu�v���狗�������߁A
�@					�@������ 5�����Ȃ狗���� 5�ɂȂ�悤�ɏC��(�����͈ێ�)
 �@					�@������15�ȏ�Ȃ狗����15�ɂȂ�悤�ɏC��(�����͈ێ�)
  �@				�@��L�ȊO�Ȃ���ɏC�����Ȃ�
					�@�Ȃ��AY�̓v���C���[��Y���W+5�����ʒu�Ƃ���
   �@	�@���q���g�F2�_�Ԃ̋����́u�O�����̒藝�v�Q�ƁA��������sqrtf�֐��Q��
		�E�u�����_�v�F��Ƀv���C���[����������
�m�F	�u�ǐՃJ�����v�؂�ւ����ɁA�v���C���[��ǂ�������悤�ɂȂ��OK
		�A���A����5�`15�̊Ԃ̓J�����͈ړ����Ȃ�
		���������Ԃ́u3�l�̃J�����v�̌ゾ���A�؂�ւ��̏��Ԃ�
		�@�u���Έʒu�Œ�J�����v�̎��ɂ��鎖
*******************************************************************************/
//	�ǐՃJ�����֐�
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