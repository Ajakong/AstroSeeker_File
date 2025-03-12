#include "PlayerSphere.h"
#include "ColliderSphere.h"
#include "PlayerSphere.h"
#include"Player.h"

#include"ModelManager.h"
namespace
{
	constexpr float kScale = 0.1f;
	/// <summary>
	/// 球の当たり判定半径
	/// </summary>
	constexpr float kSphereRadius = 0.2f;
	/// <summary>
	/// 球の生成間隔
	/// </summary>
	constexpr int kSphereCreateFrame = 50;
	
	constexpr int kLifeTimeMax = 100;
	const char* name = "Sphere";
	const char* kModelName = "Star";

}
PlayerSphere::PlayerSphere(MyEngine::Collidable::Priority priority, ObjectTag tag, std::shared_ptr<MyEngine::Collidable>player, Vec3 pos, Vec3 velocity,Vec3 sideVec ,int moveNum, int color) : SphereBase(priority, tag, pos, velocity, color, kSphereRadius),
m_player(std::dynamic_pointer_cast<Player>(player)),
m_sideVec(sideVec),
m_lifeTime(0),
m_stickFlag(false)
{
	m_modelHandle = ModelManager::GetInstance().GetModelData(kModelName);
	m_rigid->SetVelocity(VGet(m_velocity.x * 4, m_velocity.y * 4, m_velocity.z * 4));
	m_rigid->SetPos(pos);
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	item->radius = m_radius;
	m_color = color;
	//moveNumによって挙動が変わるのかもしれない(実装するかわからん)
	//if (moveNum == 0)
	{
		m_moveUpdate = &PlayerSphere::StraightUpdate;
	}

	MV1SetScale(m_modelHandle, VGet(kScale, kScale, kScale));
	
}

PlayerSphere::~PlayerSphere()
{
}

void PlayerSphere::Init()
{
	SetAntiGravity(true);
}

void PlayerSphere::Update()
{
	//m_rotateYAngle++;
	m_upVec = Cross(m_velocity.GetNormalized(),m_sideVec).GetNormalized();


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

	m_rigid->SetVelocity(VGet(m_velocity.x * 8, m_velocity.y * 8, m_velocity.z * 8));
	MV1SetPosition(m_modelHandle, m_rigid->GetPos().VGet());
	(this->*m_moveUpdate)();	
}

void PlayerSphere::Draw()
{
	DrawSphere3D(m_rigid->GetPos().VGet(), kSphereRadius, 10, 0xffff00, m_color, false);
	MV1DrawModel(m_modelHandle);
}

void PlayerSphere::Hit()
{
	
}

void PlayerSphere::Effect()
{
}

void PlayerSphere::OnCollideEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	m_isDestroyFlag = true;	
}

void PlayerSphere::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
}

void PlayerSphere::StraightUpdate()
{
	//m_velocity = Cross(m_upVec, m_sideVec);//地表に沿う

	m_lifeTime++;
	
	if (m_lifeTime > kLifeTimeMax)
	{
		m_isDestroyFlag = true;
	}
}
