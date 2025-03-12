#pragma once
#include"Vec3.h"
#include"Quaternion.h"

class Player;


class Camera
{
public:
	Camera(Vec3 pos= { 100.0f,200.0f, 300.0f });							// コンストラクタ.
	~Camera();							// デストラクタ.

	void Update(Vec3 LookPoint);	// 更新.
	void SetCamera(Vec3 LookPoint);
	void Set();
	void SetAimCamera(Vec3 LookPoint);
	void DebagDraw();
	Vec3 cameraToPlayer(const Vec3& targetPos);
	
	const Vec3& GetPos() const { return m_pos; }
	const Vec3& GetPlayerCameraPoint() const { return m_playerCameraPoint; }

	void SetCameraPos(Vec3 LookPoint);
	void SetCameraFirstPersonPos(Vec3 LookPoint);
	void SetCameraThirdPersonPos(Vec3 LookPoint);
	float GetCameraAngle() const { return m_cameraAngle; }
	void SetUpVec(Vec3 upDir) { if (m_watchCount == 0) m_upVec = upDir; }
	Vec3 GetUpVec() const { return m_upVec; }
	void SetFrontVec(Vec3 front) { m_frontVec = front; }
	void Setting(bool boost,bool aim);
	void SetBoost(bool boost) { m_isBoost = boost; }
	void SetCameraPoint(Vec3 pos) { if (m_watchCount == 0)m_cameraPoint = pos; }
	Vec3 GetCameraPoint() { return m_cameraPoint; }
	void SetEasingSpeed(float speed) { m_easingSpeed = speed; }

	/// <summary>
	/// 対象を注視中の更新
	/// </summary>
	/// <param name="lookpoint">対象の場所</param>
	/// <param name="cameraPos">カメラの位置</param>
	/// <param name="upVec">カメラの上方向ベクトル</param>
	void WatchThis(Vec3 lookpoint,Vec3 cameraPos,Vec3 upVec,float easingSpeed);
	/// <summary>
	/// 通常更新
	/// </summary>
	/// <param name="LookPoint">対象の場所</param>
	void NeutralUpdate(Vec3 LookPoint);
	/// <summary>
	/// エイム中の更新
	/// </summary>
	/// <param name="LookPoint">対象の場所</param>
	void AimingUpdate(Vec3 LookPoint);
	/// <summary>
	/// 対象に注視
	/// </summary>
	/// <param name="LookPoint"></param>
	void WatchThisUpdate(Vec3 LookPoint);

	//メンバ関数ポインタ
	using cameraState_t = void(Camera::*)(Vec3 lookpoint);
	cameraState_t m_cameraUpdate;
private:
	

private:
	int m_lightHandle = -1;
	int m_watchCount;
	float m_easingSpeed;
	float m_postEasingSpeed;

	bool m_isFirstPerson;
	bool m_isBoost;
	bool m_isAim;

	Vec3 m_frontVec;
	Quaternion m_myQ;
	Vec3 m_centerPosition;// 回転の中心座標
	Vec3 m_pos;			// ポジション.
	Vec3 m_cameraPoint;//カメラが移動してほしい位置
	Vec3 m_playerCameraPoint;
	Vec3 m_lookPoint;
	Vec3 m_postLookPointPos;
	float m_cameraAngle = -DX_PI_F / 2;
	float m_pitchAngle;
	Vec3 m_velocity;
	Vec3 m_nextUpVec;

	//プレイヤーの上方向ベクトル
	Vec3 m_upVec;
	Vec3 m_watchThisPos;
	/// <summary>
	/// プレイヤーから見たカメラの位置ベクトル
	/// </summary>
	Vec3 m_playerToCameraVec;
	Vec3 m_fowardVec;
};