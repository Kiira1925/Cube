#pragma once

#include	<DirectXMath.h>
#include	"vector.h"


/*******************************************************************************
	�u�J�����v�N���X
*******************************************************************************/
class Camera
{
public:
	/*******************************************************************************
	TODO:01 �J������ԊǗ��ԍ��̐錾
	����    �O��̓G�s���Ǘ��ԍ����l�A�J�����ɂ��Ă��Ǘ��ԍ���錾����
	--------------------------------------------------------------------------------
	�菇    enum�ňȉ��̖��̂̒萔��錾����
			�E�u�Ď��J�����v
			�E�u���Έʒu�Œ�J�����v
			�E�u�ǐՃJ�����v
			�E�u3�l�̃J����(TPS)�v
			�E�u1�l�̃J����(FPS)�v
			�E�u�J�����Ǘ��ԍ��ő�l�v	������͖����Ă��ǂ����A����ƕ֗�
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
	TODO:02 �J������ԕۑ��p�ϐ��̒�`
	����    TODO01�ō쐬����enum��ۑ�����ׂ̕ϐ����쐬����
			����͈����Ղ��D���int�^�Ő錾����
	--------------------------------------------------------------------------------
	�菇    int�^�ϐ��u��ԁv���`����
			�܂��A�R���X�g���N�^���Łu�Ď��J�����v�̒l�ŏ���������
	*******************************************************************************/
	int	state;

public:
	VECTOR3	pos;				//	�u�ʒu(���W)�v
	VECTOR3	target;				//	�u�����_�v
	DirectX::XMMATRIX	projection;			//	�u���e�s��v

	Camera();

	/*******************************************************************************
	TODO:03 �J�����X�V�֐��̐錾
	����    �J�����̐؂�ւ��A�܂��͍X�V���s���ׂ̊֐���p�ӂ���
	--------------------------------------------------------------------------------
	�菇    �ȉ��̃����o�֐���ǉ�����
			�Evoid �X�V()
			���K�v�ɉ����ă����o��ǉ����Ă��ǂ�
	*******************************************************************************/
	void Updata();

	bool push_beforeflg;
	bool push_afterflg;

	void SLCamera();
	void FRPCamera();
	void TCamera();
	void TPCamera();
	void FPCamera();




	DirectX::XMMATRIX	SetOrthographicMatrix(float w, float h, float znear, float zfar);			//	���s���e�s��ݒ�֐�
	DirectX::XMMATRIX	SetPerspectiveMatrix(float fov, float aspect, float znear, float zfar);	//	�������e�s��ݒ�֐�
	DirectX::XMMATRIX	GetViewMatrix();															//	�r���[�s��擾�֐�
	DirectX::XMMATRIX	GetProjectionMatrix() { return	projection; }								//	���e�s��擾�֐�

};