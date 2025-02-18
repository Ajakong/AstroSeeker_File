#pragma once
#include "Collidable.h"
#include"Vec3.h"
#include"Quaternion.h"
#include"ColliderSphere.h"
#include"StampImpact.h"
#include"Planet.h"
#include<string>

class Camera;
class PlayerSphere;
class Planet;

class Player : public MyEngine::Collidable
{
public:
	enum AnimNum : int
	{
		
		AnimationNumEmpty,
		AnimationNumTpose,
		AnimationNumDeath,
		AnimationNumRun,
		AnimationNumFall,
		AnimationNumSpin,
		AnimationNumHit,
		AnimationNumIdle,
		AnimationNumJump,
		AnimationNumJumpAttack,
		AnimationNumJumpDive,
		AnimationNumRolling,
		AnimationNumRollingAttack,
		AnimationNumShotPose
	};

	

	Player(Vec3 pos=Vec3::Zero());
	~Player();

	void Init();

	void Update();
	void SetMatrix();
	void Draw();
	void SetVelocity(Vec3 pos) { m_velocity = pos; m_rigid->SetVelocity(pos); }
	void AddVelocity(Vec3 pos) { m_rigid->AddVelocity(pos); }
	void SetSideVec(Vec3 right) { m_sideVec = right; }
	void SetFrontVec(Vec3 front) { m_frontVec = front; }
	void SetPos(Vec3 pos) { m_rigid->SetPos(pos); }
	void SetCameraToPlayer(Vec3 cameraToPlayer);
	/// <param name="sideVec">加速させる方向の横ベクトル</param>
	void SetBoost(Vec3 sideVec);
	void SetIsOperation(bool flag);
	void SetCameraAngle(float cameraAngle);
	void IsWarp() { m_isJumpFlag = true; }
	
	void InitDush();
	void InitJump();

	void Landing(int recast=15);


	Vec3 GetPos() const { return  m_rigid->GetPos(); }
	Vec3 GetLeftHandPos() { return Vec3(MV1GetFramePosition(m_modelHandle, m_handFrameIndex)); }
	
	Vec3 GetOnPlanetPos() { return m_nowPlanet->GetRigidbody()->GetPos(); }
	Vec3 GetVelocity() const { return m_rigid->GetVelocity(); }
	Vec3 GetMoveDir() const{ return m_moveDir; }
	Vec3 GetNormVec() const { return m_upVec.GetNormalized(); }
	Vec3 GetFrontVec() const { return m_frontVec.GetNormalized(); }
	Vec3 GetSideVec() const { return m_sideVec.GetNormalized(); }
	Vec3 GetPostUpVec() const{ return m_postUpVec; }
	Vec3 GetPostMoveDir()const{ return m_postMoveDir; }
	Vec3 GetInputVec()const { return m_inputVec; }
	Vec3 GetInputRightVec()const { return m_inputRightVec; }
	Vec3 GetShotDir()const { return m_shotDir; }
	Vec3 GetLookPoint() const{ return m_lookPoint; }
	
	float GetRegenerationRange() const{ return m_regeneRange; }
	float GetCameraEasingSpeed()const { return m_cameraEasingSpeed; }
	float GetHp() { return m_hp; }

	
	bool GetOperationFlag()const { return m_playerUpdate==&Player::OperationUpdate; }
	bool GetBoostFlag() const{ return m_playerUpdate==&Player::BoostUpdate; }
	bool GetIsAiming() { return m_isAimFlag; }
	bool GetFootOnHit() { return m_footCol->OnHit(); }
	bool IsSearch() { return m_isSearchFlag; }
	bool OnDamage() { return m_playerUpdate == &Player::DamegeUpdate; }
	bool IsClear() { return m_isClearFlag; }
	bool GetJumpFlag() const { return m_isJumpFlag; }
	bool GetSpinFlag() const { return m_isSpinFlag; }
	bool GetDeathFlag() const { return m_isDeathFlag; }
	

	int WatchHp()const { return static_cast<int>(m_hp); }
	int GetStarNum()const { return m_coinCount; }
	int GetPlayerModelhandle() const { return m_modelHandle; }
	int GetTitleMoveNum() { return m_titleUpdateNum; }
	int GetDamageFrame() const { return m_damageFrame; }

	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);
	virtual void OnCollideStay(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag);

	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	virtual void OnTriggerStay(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	//メンバ関数ポインタ
	using playerState_t = void(Player::*)();
	playerState_t m_postUpdate;
	playerState_t m_playerUpdate;

	playerState_t m_prevUpdate;
	playerState_t m_cameraUpdate;


	/// <summary>
	/// 弾の種類によって中身を入れ替える
	/// </summary>
	playerState_t m_shotUpdate;
	/// <summary>
	/// 現在のジャンプアクションによって中身を入れ替える
	/// </summary>
	playerState_t m_jumpActionUpdate;
	/// <summary>
	/// ジャンプ中の落下攻撃
	/// </summary>
	playerState_t m_dropAttackUpdate;
	/// <summary>
	/// スピンの更新処理
	/// </summary>
	playerState_t m_spinAttackUpdate;

	void CommandJump();
	void BoostUpdate();
	void OperationUpdate();
	void MoveToTargetWithStickStar(Vec3 targetPos);

	//TitlePlayerからポインタを通してアクセスするためPublic
	void ShotTheStar();
	void ShotTheStickStar();

protected:
	Vec3 Move();

	

	//アニメーションの進行
	//ループしたかどうかを返す
	bool UpdateAnim(int attachNo);
	//アニメーションの変更
	void ChangeAnim(int animIndex,float speed=1.f);

	//状態別関数(ポインタで呼び出す)
	/*m_playerUpdateで使う*/
	/// <summary>
	/// 開始直後に呼ばれる
	/// </summary>
	void StartUpdate();
	/// <summary>
	/// 通常時
	/// </summary>
	void NeutralUpdate();
	/// <summary>
	/// 歩き
	/// </summary>
	void WalkingUpdate();
	/// <summary>
	/// 走り
	/// </summary>
	void DashUpdate();
	//スピンアクション
	/// <summary>
	/// スピンアクション統括
	/// </summary>
	void SpinActionUpdate();
	/// <summary>
	/// スピン攻撃
	/// </summary>
	void SpiningUpdate();
	/// <summary>
	/// ローリングアタック
	/// </summary>
	void RollingAttackUpdate();

	/// <summary>
	/// ジャンプ中
	/// </summary>
	void JumpingUpdate();
	/// <summary>
	/// ダッシュジャンプ中
	/// </summary>
	void DashJumpUpdate();
	
	//ジャンプ中の特殊アクション
	/*m_jumpActionUpdateで使う*/
	/// <summary>
	/// ジャンプ中のアクション統括
	/// </summary>
	void JumpActionUpdate();
	/// <summary>
	/// ジャンプ中にスピン
	/// </summary>
	void JumpingSpinUpdate();
	/// <summary>
	/// ジャンプ中に加速
	/// </summary>
	void JumpBoostUpdate();

	//ジャンプ中の特殊攻撃
	/*m_dropAttackUpdateで使う*/
	/// <summary>
	/// 落下攻撃統括
	/// </summary>
	void DropAttackUpdate();
	/// <summary>
	/// ヒップドロップ攻撃
	/// </summary>
	void NormalDropAttackUpdate();
	/// <summary>
	/// 最大火力落下攻撃
	/// </summary>
	void FullPowerDropAttackUpdate();

	/// <summary>
	/// 落下攻撃の怯み
	/// </summary>
	void LandingUpdate();

	/// <summary>
	/// 照準
	/// </summary>
	void AimingUpdate();

	/// <summary>
	/// ダメージ時
	/// </summary>
	void DamegeUpdate();
	/// <summary>
	/// 回避
	/// </summary>
	void AvoidUpdate();

	/// <summary>
	/// 死亡
	/// </summary>
	void DeathUpdate();
	void SetShotDir();
	/// <summary>
	/// 弾の削除処理
	/// </summary>
	void DeleteManage();

	void TalkingUpdate();
	
	

	template <typename T>
	/// <summary>
	/// 配列の削除処理
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="objects"></param>
	void DeleteObject(std::vector<std::shared_ptr<T>>& objects);
	Vec3 GetCameraToPlayer()const;

	


protected:
	struct UserData
	{
		float dissolveY;	// ディゾルヴしたい高さ
		float minY;
		float maxY;
		float dummy;
		float clickedU;
		float clickedV;
		float dummy2[2];
	};
	std::shared_ptr<MyEngine::ColliderSphere> m_headCol;
	std::shared_ptr<MyEngine::ColliderSphere> m_spinCol;
	std::shared_ptr<MyEngine::ColliderSphere> m_footCol;
	std::shared_ptr<MyEngine::ColliderSphere> m_bodyCol;
	
	int m_modelHandle = 0;
	int m_handFrameIndex;
	int m_shotAnimCount;

	int m_landingStanFrame;
	//プレイヤーのステータス
	int m_coinNum;
	float m_hp;
	float m_speed = 1.f;
	float m_cameraEasingSpeed;
	float m_currentOxygen;

	/// <summary>
	/// 行動のフレームを管理する
	/// </summary>
	int m_actionFrame = 0;

	int m_fragmentCount;
	int m_coinCount;

	int m_pointLightHandle = -1;


	//音
	int m_hitSEHandle;
	int m_elecSEHandle;
	int m_parrySEHandle;
	int m_searchSEHandle;
	int m_shotStickStarSEHandle;
	int m_shotTheStarSEHandle;
	int m_getItemHandle;
	int m_specialItemGetSEHandle;
	int m_powerUpItemGetSEHandle;

	//エフェクト
	int m_starEffect;

	int m_color;
	int m_spinCount;
	int m_pushCount;
	int m_revivalCount;

	bool m_shotAnimFlag;
	/// <summary>
	/// タイトル画面で今している行動の番号
	/// </summary>
	int m_titleUpdateNum;

	std::list<std::shared_ptr<PlayerSphere>> m_sphere;
	std::vector<std::shared_ptr<StampImpact>> m_impacts;

	
	std::shared_ptr<Planet>m_nowPlanet;

	bool m_isOnDamageFlag;
	bool m_isSpinFlag;
	bool m_isDeathFlag;

	float m_regeneRange;
	float m_angle;
	float m_modelDirAngle;
	float m_spinAngle;
	float m_radius = 0;
	float m_attackRadius;

	float m_fullPowerChargeCount;

	Quaternion m_myQ;
	Quaternion m_rotateYQ;

	MATRIX m_initMat;
	Vec3 m_cameraToPlayer;
	Vec3 m_cameraPos;
	Vec3 m_lookPoint;
	Vec3 m_postPos;
	Vec3 m_velocity;
	//std::shared_ptr<Camera> m_camera;
	Vec3 m_moveDir;
	Vec3 m_postMoveDir;
	Vec3 m_nowVec;
	Vec3 m_inputVec;
	Vec3 m_inputRightVec;

	Vec3 m_frontVec;
	Vec3 m_sideVec;
		
	Vec3 m_postUpVec;

	Vec3 m_shotDir;
	Vec3 m_modelBodyRotate;
	/// <summary>
	/// 最後に立っていた惑星の地面の位置
	/// </summary>
	Vec3 m_postPlayerGroundPos;

	//アニメーション変数

	//0.0f:prevが再生
	//1.0:currentが再生
	int m_currentAnimNo;//現在のアニメーション
	int m_prevAnimNo;//変更前のアニメーション
	float m_animBlendRate;//アニメーションの合成割合
	
	int m_damageFrame;//ダメージ状態の現在のフレーム数

	/*int m_searchRemainTime;
	int m_chargeRemainTime;*/

	float m_playAnimTime = 0.0f;
	float m_animTime_move = 0.0f;

	float m_playerRotateY = 0;
	float m_cameraAngle = 0;

	bool m_isVisibleFlag = false;
	bool m_isJumpFlag = false;
	bool m_isBoostFlag = false;
	bool m_isOperationFlag = false;
	bool m_isSearchFlag = false;
	bool m_isAimFlag = false;
	bool m_isClearFlag=false;

	int m_visibleCount;

	int m_hitCount = 0;
};

template<typename T>
inline void Player::DeleteObject(std::vector<std::shared_ptr<T>>& objects)
{
	auto result = remove_if(objects.begin(), objects.end(), [this](const auto& object)
		{
			if (object->GetDeleteFlag())
			{
				MyEngine::Physics::GetInstance().Exit(object);
			}
			return object->GetDeleteFlag(); // IsDestroy() が true の場合削除
		});
	objects.erase(result, objects.end());
}
