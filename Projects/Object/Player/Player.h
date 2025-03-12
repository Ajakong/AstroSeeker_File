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
	//アニメーション番号
	enum AnimNum : int
	{
		AnimationNumEmpty,//からのアニメーション
		AnimationNumTpose,//Tポーズアニメーション
		AnimationNumDeath,//死亡
		AnimationNumRun,//ダッシュ
		AnimationNumFall,//落下(浮遊)
		AnimationNumSpin,//スピン
		AnimationNumHit,//ダメージ
		AnimationNumIdle,//立ち
		AnimationNumJump,//ジャンプ
		AnimationNumJumpAttack,//ヒップドロップ
		AnimationNumJumpDive,//ダッシュジャンプ
		AnimationNumRolling,//体育すわり
		AnimationNumRollingAttack,//回転攻撃
		AnimationNumShotPose//ショット時
	};


	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="pos">初期位置(デフォルトで0,0,0)</param>
	Player(Vec3 pos = Vec3::Zero());
	~Player();

	void Init();

	void Update();
	void SetMatrix();
	void Draw();


	/// <summary>
	/// 移動ベクトルのセット
	/// </summary>
	/// <param name="pos"></param>
	void SetVelocity(Vec3 vec) { m_velocity = vec; m_rigid->SetVelocity(vec); }

	/// <summary>
	/// 移動ベクトルの加算
	/// </summary>
	/// <param name="pos"></param>
	void AddVelocity(Vec3 vec) { m_rigid->AddVelocity(vec); }

	/// <summary>
	/// プレイヤーの位置を設定
	/// </summary>
	/// <param name="pos">プレイヤーの位置</param>
	void SetPos(Vec3 pos) { m_rigid->SetPos(pos); }

	/// <summary>
	/// プレイヤーを加速させる
	/// </summary>
	/// <param name="sideVec">加速させる方向の横ベクトル</param>
	void SetBoost(Vec3 sideVec);

	/// <summary>
	/// プレイヤーの挙動を操作するモードに移行
	/// </summary>
	/// <param name="flag">操作モードのオンオフ</param>
	void SetIsOperation(bool flag);

	/// <summary>
	/// 状態関数にニュートラルを設定する
	/// </summary>
	void SetNeutralUpdate() { m_playerUpdate = &Player::NeutralUpdate; }
	/// <summary>
	/// 状態関数にブーストを設定する
	/// </summary>
	void SetBoostUpdate() { m_playerUpdate = &Player::BoostUpdate; }

	/// <summary>
	/// 着地
	/// </summary>
	/// <param name="recast">着地の隙(デフォルト15フレーム)</param>
	void Landing(int recast = 15);

	/// <summary>
	/// プレイヤーの位置の取得
	/// </summary>
	/// <returns>プレイヤーの位置</returns>
	Vec3 GetPos() const { return  m_rigid->GetPos(); }

	/// <summary>
	/// プレイヤーの右手の位置の取得
	/// </summary>
	/// <returns>プレイヤーのモデルの右手位置</returns>
	Vec3 GetLeftHandPos() const { return Vec3(MV1GetFramePosition(m_modelHandle, m_leftHandFrameIndex)); }

	/// <summary>
	/// プレイヤーの現在の上方向ベクトルを取得
	/// </summary>
	/// <returns>プレイヤーの上方向ベクトル</returns>
	Vec3 GetNormVec() const { return m_upVec.GetNormalized(); }

	/// <summary>
	/// プレイヤーの現在の正面ベクトルの取得
	/// </summary>
	/// <returns>プレイヤーの正面ベクトル</returns>
	Vec3 GetFrontVec() const { return m_frontVec.GetNormalized(); }

	/// <summary>
	/// プレイヤーの現在の右方向ベクトルの取得
	/// </summary>
	/// <returns>プレイヤーの右方向ベクトル</returns>
	Vec3 GetSideVec() const { return m_sideVec.GetNormalized(); }

	/// <summary>
	/// (デバッグにしか使われてない)1フレーム前のプレイヤーの移動方向ベクトルの取得
	/// </summary>
	/// <returns>1フレーム前のプレイヤーの上方向ベクトル</returns>
	Vec3 GetPostMoveDir()const { return m_postMoveDir; }

	/// <summary>
	/// (デバッグにしか使われてない)1フレーム前のプレイヤーのコントローラースティック入力方向ベクトルの取得
	/// </summary>
	/// <returns>プレイヤーのコントローラースティック入力方向ベクトル</returns>
	Vec3 GetInputVec()const { return m_inputVec; }

	/// <summary>
	/// プレイヤーの現在の射撃方向ベクトルの取得
	/// </summary>
	/// <returns>プレイヤーの射撃方向ベクトル</returns>
	Vec3 GetShotDir()const { return m_shotDir; }

	/// <summary>
	/// カメラが見てほしい注視点の位置の取得
	/// </summary>
	/// <returns>プレイヤーがカメラが見てほしいと思っている位置</returns>
	Vec3 GetLookPoint() const { return m_lookPoint; }

	/// <summary>
	/// (シェーダー用)プレイヤーのモデルが下から生成されている部分の長さ
	/// </summary>
	/// <returns></returns>
	float GetRegenerationRange() const { return m_regeneRange; }

	/// <summary>
	/// カメラの追ってくる速度の取得
	/// </summary>
	/// <returns>カメラの追ってくる速度の取得</returns>
	float GetCameraEasingSpeed()const { return m_cameraEasingSpeed; }

	/// <summary>
	/// プレイヤーの現在のヒットポイントの取得
	/// </summary>
	/// <returns>ヒットポイント残量</returns>
	float GetHp() const { return m_hp; }

	/// <summary>
	/// プレイヤーがほかのオブジェクトに操作されているか
	/// </summary>
	/// <returns>プレイヤーが操作されているかフラグ</returns>
	bool GetOperationFlag()const { return m_playerUpdate == &Player::OperationUpdate; }

	/// <summary>
	/// プレイヤーが加速状態か
	/// </summary>
	/// <returns>プレイヤーが加速状態かフラグ</returns>
	bool GetBoostFlag() const { return m_playerUpdate == &Player::BoostUpdate; }

	/// <summary>
	/// プレイヤーがエイム中か
	/// </summary>
	/// <returns>プレイヤーがエイム中かフラグ</returns>
	bool GetIsAiming() const { return m_isAiming; }

	/// <summary>
	/// (デバッグにしか使われていない)プレイヤーが何かに着地しているか
	/// </summary>
	/// <returns></returns>
	bool GetFootOnHit() const { return m_footCol->OnHit(); }

	/// <summary>
	/// (デバッグにしか使われてない)プレイヤーがデバッグ情報表示状態か
	/// </summary>
	/// <returns>プレイヤーがデバッグ情報表示状態かフラグ</returns>
	bool IsSearch() { return m_isSearch; }

	/// <summary>
	/// プレイヤーがクリア条件を満たしたか
	/// </summary>
	/// <returns>クリア条件を満たしたかフラグ</returns>
	bool IsClear() const { return m_isClear; }

	/// <summary>
	/// プレイヤーがジャンプ中か
	/// </summary>
	/// <returns>プレイヤーがジャンプ中かフラグ</returns>
	bool GetJumpFlag() const { return m_isJump; }

	/// <summary>
	/// プレイヤーが死亡したか
	/// </summary>
	/// <returns>プレイヤーが死亡したかフラグ</returns>
	bool GetDeathFlag() const { return m_isDead; }

	/// <summary>
	/// プレイヤーが持っているスターコインの数
	/// </summary>
	/// <returns>スターコインの数</returns>
	int GetStarNum()const { return m_coinCount; }

	/// <summary>
	/// ダメージ中のフレーム取得
	/// </summary>
	/// <returns>ダメージ中のフレーム</returns>
	int GetDamageFrame() const { return m_damageFrame; }

	/// <summary>
	/// ノックバックさせてダメージを受け、無敵状態にする
	/// </summary>
	/// <param name="knockBackVec">ノックバックのベクトル</param>
	/// <param name="damage">ダメージ量</param>
	void OnDamege(Vec3 knockBackVec, float damage);

	virtual void OnCollideEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	virtual void OnCollideStay(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);

	virtual void OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);
	virtual void OnTriggerStay(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag);

	/// <summary>
	/// ジャンプさせる
	/// </summary>
	void CommandJump();

	/// <summary>
	/// 惑星移動中の処理
	/// </summary>
	void BoostUpdate();

	/// <summary>
	/// 操作される更新処理
	/// </summary>
	void OperationUpdate();


	//TitlePlayerからポインタを通してアクセスするためPublic(ポインタを通す場合継承していてもProtectedでは扱えない)
	/// <summary>
	/// 弾を発射
	/// </summary>
	void ShotTheStar();
	/// <summary>
	/// グラッピング弾を発射
	/// </summary>
	void ShotTheStickStar();

	/// <summary>
	/// 通常時
	/// </summary>
	void NeutralUpdate();
protected:

	//アニメーションの進行
	/// <summary>
	/// ループしたかどうかを返す
	/// </summary>
	/// <param name="attachNo">現在アタッチしているアニメーション番号</param>
	/// <returns>ループしたかどうか</returns>
	bool UpdateAnim(int attachNo);

	/// <summary>
	/// アニメーションの変更
	/// </summary>
	/// <param name="animIndex">アニメーションのインデックス</param>
	/// <param name="speed">アニメーション再生速度</param>
	void ChangeAnim(int animIndex, float speed = 1.f);

	/// <summary>
	/// コントローラーの入力に合わせた射撃方向の設定
	/// </summary>
	void SetShotDir();

	

	/// <summary>
	/// 弾の削除処理
	/// </summary>
	void DeleteManage();

	template <typename T>
	/// <summary>
	/// 配列の削除処理
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="objects"></param>
	void DeleteObject(std::vector<std::shared_ptr<T>>& objects);

protected:

	//メンバ関数ポインタ
	using playerState_t = void(Player::*)();
	playerState_t m_postUpdate;

	//プレイヤーの状態
	playerState_t m_playerUpdate;
	//前のプレイヤーの状態
	playerState_t m_prevUpdate;

	playerState_t m_cameraUpdate;
	// 弾の種類によって中身を入れ替える
	playerState_t m_shotUpdate;

	// 現在のジャンプアクションによって中身を入れ替える
	playerState_t m_jumpActionUpdate;

	// ジャンプ中の落下攻撃
	playerState_t m_dropAttackUpdate;

	// スピンの更新処理
	playerState_t m_spinAttackUpdate;

	float m_regeneRange;//(シェーダー用)モデルの生成されているフレーム数

	int m_modelHandle;//プレイヤーモデルハンドル

	int m_leftHandFrameIndex;//プレイヤーモデルの左手のフレームインデックス

	std::list<std::shared_ptr<PlayerSphere>> m_sphere;//プレイヤーの生成弾ポインターリスト

	std::vector<std::shared_ptr<StampImpact>> m_impacts;//プレイヤーの生成衝撃波リスト

	std::shared_ptr<Planet>m_nowPlanet;//現在いる惑星のポインタ
	/// <summary>
	/// 最後に立っていた惑星の地面の位置
	/// </summary>
	Vec3 m_postPlayerGroundPos;

	bool m_isVisible;//無敵状態フラグ
	bool m_isJump;//ジャンプ状態フラグ
	bool m_isOperation;//プレイヤーがほかのオブジェクトに操作されているかフラグ
	bool m_isSearch;//(デバッグ用)デバッグ情報表示状態かフラグ
	bool m_isAiming;//エイム状態かフラグ
	bool m_isDead;//死亡したかフラグ

	bool m_isClear;//クリア条件を満たしたかフラグ


	float m_spinAngle;//プレイヤーの回転量

	float m_fullPowerChargeCount;//力をためられるもののためた力のカウント


	Vec3 m_lookPoint;//カメラに見てほしい位置
	Vec3 m_postPos;//1フレーム前のプレイヤーの位置

	Vec3 m_velocity;//プレイヤーの移動ベクトル
	Vec3 m_moveDir;//プレイヤーの移動方向ベクトル
	Vec3 m_postMoveDir;//(デバッグ用にしか使われていない)1フレーム前のプレイヤーの移動方向ベクトル
	Vec3 m_postShotVec;//1フレーム前の弾の射撃方向ベクトル
	Vec3 m_inputVec;//プレイヤーの入力方向ベクトル
	Vec3 m_frontVec;//プレイヤーの正面方向ベクトル
	Vec3 m_sideVec;//プレイヤーの右方向ベクトル
	Vec3 m_postUpVec;//1フレーム前のプレイヤーの上方向ベクトル
	Vec3 m_shotDir;//弾の射撃方向ベクトル

	

	//アニメーション変数

	//0.0f:prevが再生
	//1.0:currentが再生
	int m_currentAnimNo;//現在のアニメーション
	int m_prevAnimNo;//変更前のアニメーション
	float m_animBlendRate;//アニメーションの合成割合

private:
	/// <summary>
	/// コントローラースティックに合わせたプレイヤーのローカル移動方向を取得
	/// </summary>
	/// <returns>プレイヤーのローカル移動方向</returns>
	Vec3 Move();

	//状態別関数(ポインタで呼び出す)
	/*m_playerUpdateで使う*/

	/// <summary>
	/// 開始直後に呼ばれる
	/// </summary>
	void StartUpdate();
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
	/// 死亡
	/// </summary>
	void DeathUpdate();
	/// <summary>
	/// 会話中
	/// </summary>
	void TalkingUpdate();


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

private:
	/// <summary>
	/// シェーダー用プロパティ
	/// </summary>
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

	/// <summary>
	/// 頭のコリジョンポインタ
	/// </summary>
	std::shared_ptr<MyEngine::ColliderSphere> m_headCol;
	/// <summary>
	/// 足のコリジョンポインタ
	/// </summary>
	std::shared_ptr<MyEngine::ColliderSphere> m_footCol;
	/// <summary>
	/// 胴体のコリジョンポインタ
	/// </summary>
	std::shared_ptr<MyEngine::ColliderSphere> m_bodyCol;

	/// <summary>
	/// スピン攻撃のコリジョンポインタ
	/// </summary>
	std::shared_ptr<MyEngine::ColliderSphere> m_spinCol;


	int m_landingStanFrame;//プレイヤーの着地の隙

	//プレイヤーのステータス
	float m_hp;//ヒットポイント
	float m_speed;//移動速度
	float m_currentOxygen;//プレイヤーの酸素残量(0で死亡)

	float m_cameraEasingSpeed;//カメラが追ってきてほしい速度

	int m_fragmentCount;//星のかけらの所持数
	int m_coinCount;//スターコインの所持数

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

	int m_revivalCount;//蘇生回数カウンター


	int m_damageFrame;//ダメージ状態の現在のフレーム数

	int m_visibleCount;//無敵時間カウント
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
