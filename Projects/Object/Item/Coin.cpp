#include "Coin.h"
#include"Physics.h"
#include"SoundManager.h"
#include"ModelManager.h"
namespace
{
	constexpr float kScale = 0.1f;

	const char* kGetSoundEffectName = "GetCoin.mp3";
	const char* kCoinModelName = "Star";
}

Coin::Coin(Vec3 pos, bool antiGravity) : Item(pos,ObjectTag::Coin,antiGravity),
m_getSoundEffectHandle(),
m_modelHandle(),
m_rotateYAngle(1)
{
	m_getSoundEffectHandle=SoundManager::GetInstance().GetSoundData(kGetSoundEffectName);
	m_modelHandle = ModelManager::GetInstance().GetModelData(kCoinModelName);
	MV1SetScale(m_modelHandle, VGet(kScale, kScale, kScale));


}

Coin::~Coin()
{
}

void Coin::Init()
{
	SetObjectTag(ObjectTag::Coin);
	AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
	m_col = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
	m_col->radius = 2.5f;
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

	auto pos=MV1GetPosition(m_modelHandle);

	Vec3 vel = m_upVec * sin(angle);
	m_rigid->SetVelocity(vel * 0.1f);


}

void Coin::Draw()
{
	/*Vec3 zero = { 0,0,0 };
	m_rigid->SetVelocity(Vec3(0, 0, 0));
	Vec3 offSetVec = GetCameraRightVector();
	offSetVec -= GetCameraUpVector();
	offSetVec *= 0.9f;
	Quaternion myQ;
	angle += 0.05f;

	Vec3 front = GetCameraFrontVector();
	for (int i = 0; i < 3; i++)
	{
		myQ.SetQuaternion(offSetVec);
		myQ.SetMove(DX_PI_F * 2 / 3 * i + angle, front);
		Vec3 offSet = myQ.Move(offSetVec, zero);
		DrawSphere3D((m_rigid->GetPos() + offSet).VGet(), 1.0, 8, 0x0000aa, 0x000000, false);
	}

	DrawSphere3D(m_rigid->GetPos().VGet(), m_col->radius, 8, 0xffff00, 0xffffff, false);*/

	MV1DrawModel(m_modelHandle);
}

void Coin::OnTriggerEnter(std::shared_ptr<Collidable> colider,ColideTag ownTag,ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Stage)
	{
		m_nowPlanetPos = colider->GetRigidbody()->GetPos();
	}
	if (colider->GetTag() == ObjectTag::Player)
	{
		PlaySoundMem(m_getSoundEffectHandle, DX_PLAYTYPE_BACK);
		MyEngine::Physics::GetInstance().Exit(shared_from_this());
	}
}

