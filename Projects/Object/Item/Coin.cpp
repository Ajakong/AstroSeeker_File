#include "Coin.h"
#include"Physics.h"
#include"SoundManager.h"
#include"ModelManager.h"
namespace
{
	constexpr int kStarCoinPower = 10;

	constexpr float kScale = 0.1f;
	constexpr float kStarCoinRadius = 2.5f;

	constexpr float kMoveMag = 0.1f;


	const char* kGetSoundEffectName = "GetCoin.mp3";
	const char* kCoinModelName = "Star";
}

Coin::Coin(Vec3 pos, bool antiGravity) : Item(pos,ObjectTag::Coin,antiGravity),
m_getSoundEffectHandle(-1),
m_modelHandle(-1),
m_rotateYAngle(1)
{
	//影響度の設定
	m_power = kStarCoinPower;

	//ハンドルの取得
	m_getSoundEffectHandle=SoundManager::GetInstance().GetSoundData(kGetSoundEffectName);
	m_modelHandle = ModelManager::GetInstance().GetModelData(kCoinModelName);
	//モデルのスケールの設定
	MV1SetScale(m_modelHandle, VGet(kScale, kScale, kScale));
}

Coin::~Coin()
{
}

void Coin::Init()
{
	//タグの設定
	SetObjectTag(ObjectTag::Coin);
	
	//当たり判定の追加
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	m_col = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	m_col->radius = kStarCoinRadius;
	m_col->m_isTrigger = true;
}

void Coin::Update()
{
	//m_rotateYAngle++;
	m_upVec = (m_rigid->GetPos() - m_nowPlanetPos).GetNormalized();
	
	
	MATRIX scaleMat = MGetScale(VGet(kScale, kScale, kScale)); // Y軸方向に0.1倍のスケーリング

	// 回転行列の作成（モデルの上方向ベクトルを法線ベクトルに合わせる）
	//MATRIX rotationY = MGetRotY(m_rotateYAngle);
	MATRIX rotMat = MGetRotVec2(Vec3::Up().VGet(), m_upVec.VGet());//MMult(, m_upVec.VGet()),rotationY);
	

	// 平行移動行列の作成（モデルを衝突位置に移動）
	MATRIX transMat = MGetTranslate((m_rigid->GetPos()).VGet());

	// 変換行列の合成（スケーリング → 回転 → 平行移動の順）
	MATRIX transformMat = MMult(scaleMat, rotMat);
	transformMat = MMult(transformMat, transMat);

	// モデルの行列を設定
	MV1SetMatrix(m_modelHandle, transformMat);

	//モデルの位置を設定
	MV1GetPosition(m_modelHandle);

	m_angle += 0.02f;

	//ローカル座標系で上下に移動する
	Vec3 vel = m_upVec * sin(m_angle);
	m_rigid->SetVelocity(vel * kMoveMag);
}

void Coin::Draw()
{
	//モデルの描画
	MV1DrawModel(m_modelHandle);
}

void Coin::OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	//ステージと衝突
	if (colider->GetTag() == ObjectTag::Stage)
	{
		//現在の惑星を設定
		m_nowPlanetPos = colider->GetRigidbody()->GetPos();
	}

	//プレイヤーと衝突
	if (colider->GetTag() == ObjectTag::Player)
	{
		PlaySoundMem(m_getSoundEffectHandle, DX_PLAYTYPE_BACK);
		
		//物理エンジンから抜ける
		MyEngine::Physics::GetInstance().Exit(shared_from_this());
	}
}

