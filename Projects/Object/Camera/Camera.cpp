﻿#include "Camera.h"
#include"Pad.h"
#include"Easing.h"
#include <math.h>


namespace
{
	constexpr float kCameraDist = 96;
	constexpr float kCameraHeight = 32;

	constexpr float kCameraNear = 1.f;
	constexpr float kCameraFar =30000.0f;

	//constexpr VECTOR kFirstPos=VGet(300, 300, -200);

	constexpr float kCameraFOV = 60.0f;

	constexpr float kcameraRotateSpeed = 0.05f;

	constexpr float kOneRapAngle = 360.f;
	constexpr float kWatchThisTime = 200.f;

	constexpr float kCameraLocalPositionX = 0.f;

	constexpr float kCameraLocalPositionY = 100.f;
	constexpr float kCameraLocalPositionZ = -200.f;


	constexpr float kCameraRotationSpeed = 20000.f;//速:小～大:遅
}

Camera::Camera(Vec3 pos):
	//カメラのパラメータの初期化
	m_cameraAngle(0),
	m_pitchAngle(0),
	m_watchCount(0),
	m_easingSpeed(-1),
	m_postEasingSpeed(0),
	//フラグの初期化
	m_isAim(false),
	m_isBoost(false),
	m_isFirstPerson(false),

	m_upVec(Vec3(0, 1, 0))
{
	
	m_cameraUpdate = &Camera::NeutralUpdate;
	//奥行0.1～1000までをカメラの描画範囲とする
	
	// FOV(視野角)を60度に
	SetupCamera_Perspective(kCameraFOV * (DX_PI_F / 180.0f));

	m_pos = pos;
	m_centerPosition = pos;
	m_playerToCameraVec = { kCameraLocalPositionX,kCameraLocalPositionY,-kCameraLocalPositionZ };
	m_postLookPointPos = { 0,0,0 };
	m_fowardVec = { 0.f,0.f,0.1f };

	SetCameraNearFar(kCameraNear, kCameraFar);
}

Camera::~Camera()
{
	// 処理なし.
	DeleteLightHandle(m_lightHandle);
}

void Camera::Update(Vec3 LookPoint)
{
	//カメラこれを見ろ状態じゃなかったら
	if (m_cameraUpdate != &Camera::WatchThisUpdate)
	{
		//プレイヤーがエイムしている場合
		if (m_isAim)
		{
			m_cameraUpdate = &Camera::AimingUpdate;
		}
	}
	
	//プレイヤーが惑星移動中
	if (m_isBoost)
	{
		// FOV(視野角)を60度に
		SetupCamera_Perspective(kCameraFOV * (DX_PI_F*2.f / 180.0f));

	}
	else
	{
		// FOV(視野角)を60度に
		SetupCamera_Perspective(kCameraFOV * (DX_PI_F / 180.0f));

	}
	
	(this->*m_cameraUpdate)(LookPoint);
	
	//オーディオリスナーの位置の更新
	Set3DSoundListenerPosAndFrontPosAndUpVec(m_pos.VGet(), Vec3(m_pos + GetCameraFrontVector()).VGet(), m_upVec.VGet());
}

void Camera::SetCamera(Vec3 LookPoint)
{
	m_lookPoint = LookPoint;

	Vec3 velocity;
	velocity.x = (m_cameraPoint.x - m_pos.x) / m_easingSpeed;
	velocity.y = (m_cameraPoint.y - m_pos.y) / m_easingSpeed;
	velocity.z = (m_cameraPoint.z - m_pos.z) / m_easingSpeed;
	m_pos += velocity;//イージング

	m_playerCameraPoint = m_pos;

	// 入力された左スティック方向
	int directX = 0, directY = 0;
	GetJoypadAnalogInputRight(&directX, &directY, DX_INPUT_PAD1);
	//プレイヤーの上方向を回転軸にして左スティックのX方向入力の強度に応じて回転
	m_myQ = m_myQ.CreateRotationQuaternion(static_cast<float>(directX) / kCameraRotationSpeed, m_upVec);
	//ローカル座標系(原点中心)で回転させたのちに平行移動
	m_pos = m_myQ.Move(m_pos-LookPoint, LookPoint);

	SetCameraPositionAndTargetAndUpVec(m_pos.VGet(), Vec3(m_lookPoint + m_upVec).VGet(), m_upVec.VGet());
	m_postLookPointPos = m_lookPoint;
}

void Camera::Set()
{
	SetCameraPositionAndTargetAndUpVec(m_pos.VGet(), Vec3(m_lookPoint + m_upVec).VGet(), m_upVec.VGet());
}

void Camera::SetAimCamera(Vec3 LookPoint)
{
	m_lookPoint = LookPoint;

	m_pos = m_cameraPoint;

	m_playerCameraPoint = m_pos;

	SetCameraPositionAndTargetAndUpVec(m_pos.VGet(),Vec3(m_pos+LookPoint).VGet(), m_upVec.VGet());

	m_postLookPointPos = m_lookPoint+m_pos;
}

void Camera::DebagDraw()
{
}

Vec3 Camera::cameraToPlayer(const Vec3& targetPos)
{
	Vec3 cameraToPlayer (sqrt((targetPos.x - m_pos.x) * (targetPos.x - m_pos.x)), 0.0f, sqrt((targetPos.z - m_pos.z) * (targetPos.z - m_pos.z)) );
	return cameraToPlayer;
}

void Camera::SetCameraPos(Vec3 LookPoint)
{
}

void Camera::NeutralUpdate(Vec3 LookPoint)
{
	//LBボタンが入力されていたら、またはエイム中だったら
	if (Pad::IsTrigger(PAD_INPUT_Y ) || m_isAim)//XBoxコントローラーのL
	{
		m_cameraUpdate = &Camera::AimingUpdate;
	}
	SetCamera(LookPoint);
}

void Camera::AimingUpdate(Vec3 LookPoint)
{
	//LBボタンが入力されていたら、またはエイム中ではなかったら
	if (Pad::IsTrigger(PAD_INPUT_Y)||!m_isAim)//XBoxコントローラーのL
	{
		m_cameraUpdate = &Camera::NeutralUpdate;
	}
	SetAimCamera(LookPoint);
}

void Camera::WatchThisUpdate(Vec3 LookPoint)
{
	m_upVec = Slerp(m_upVec, m_nextUpVec, 0.1f);
	m_watchCount++;

	SetLightPositionHandle(m_lightHandle, m_pos.VGet());
	SetLightDirectionHandle(m_lightHandle, GetCameraFrontVector());
	Vec3 velocity;
	velocity.x = (m_watchThisPos.x - m_pos.x) / m_easingSpeed;
	velocity.y = (m_watchThisPos.y - m_pos.y) / m_easingSpeed;
	velocity.z = (m_watchThisPos.z - m_pos.z) / m_easingSpeed;
	m_pos += velocity;//イージング
	
	SetCameraPositionAndTargetAndUpVec(m_pos.VGet(),Vec3(m_lookPoint + m_upVec.GetNormalized() * 10.0f).VGet(), m_upVec.VGet());

	m_postLookPointPos = m_lookPoint;
	//LookPointを見ている時間が上限に達したら
	if (m_watchCount > kWatchThisTime)
	{
		m_watchCount = 0;
		m_easingSpeed = m_postEasingSpeed;
		if(m_isFirstPerson)m_cameraUpdate = &Camera::SetCameraFirstPersonPos;
		else { m_cameraUpdate = &Camera::NeutralUpdate; }
	}
}

void Camera::SetCameraFirstPersonPos(Vec3 LookPoint)
{
	SetLightPositionHandle(m_lightHandle, Vec3(LookPoint + m_upVec * 12).VGet());
	SetLightDirectionHandle(m_lightHandle, (m_upVec * -1).VGet());

	//LBボタンが入力されていたら
	if (Pad::IsTrigger(PAD_INPUT_Y))
	{
		m_cameraUpdate = &Camera::NeutralUpdate;
	}
	Vec3 target = LookPoint;
	SetCameraPositionAndTargetAndUpVec((LookPoint+m_upVec*30).VGet(), target.VGet(), m_upVec.VGet());
}

void Camera::SetCameraThirdPersonPos(Vec3 LookPoint)
{

	SetCameraPositionAndTargetAndUpVec(m_pos.VGet(), LookPoint.VGet(), m_upVec.VGet());
}

void Camera::Setting(bool boost, bool aim)
{
	m_isBoost = boost;
	m_isAim = aim;
}

void Camera::WatchThis(Vec3 lookpoint, Vec3 cameraPos, Vec3 upVec,float easingspeed)
{
	//初期化
	m_postEasingSpeed = m_easingSpeed;
	m_easingSpeed = easingspeed;
	
	m_cameraUpdate = &Camera::WatchThisUpdate;
	m_lookPoint = lookpoint;
	m_watchThisPos = cameraPos;
	m_nextUpVec = upVec;

	WatchThisUpdate(lookpoint);
}
