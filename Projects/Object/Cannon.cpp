#include "Cannon.h"

#include"CannonSphere.h"
#include"ModelManager.h"
#include"Physics.h"



namespace
{
	constexpr float kNeutralRadius = 3;

	constexpr int kCreateBulletFrameMax = 300;

	

	const char* kModelName = "Cannon";

	constexpr float kScale = 0.05f;
}

Cannon::Cannon(Vec3 pos, ObjectTag shotTag,int color) : MyEngine::Collidable(Priority::StageGimmick,ObjectTag::Cannon),
m_createBulletFrame(0)
{
	m_modelHandle = ModelManager::GetInstance().GetModelData(kModelName);

	MV1SetPosition(m_modelHandle, pos.VGet());
	MV1SetScale(m_modelHandle, Vec3(kScale, kScale, kScale).VGet());
	m_cylinderIndex = MV1SearchFrame(m_modelHandle, "Cylinder");
	m_cylinderPos = MV1GetFramePosition(m_modelHandle, m_cylinderIndex);
	m_bulletTag = shotTag;
	m_rigid->SetPos(pos);
	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Head);
		m_col = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		m_col->radius = kNeutralRadius;
		m_col->SetShiftPosNum(Vec3(m_upVec* kNeutralRadius));
		m_col->m_isTrigger = true;

	}

	SetAntiGravity(true);
	AddThroughTag(shotTag);
	m_bulletColor = color;
}

Cannon::~Cannon()
{
}

void Cannon::Init()
{
}

void Cannon::Update()
{
	m_postUpVec = m_upVec;//上方向ベクトルを前のフレームの上方向ベクトルにする

	for (auto& item : m_cannonSphere)
	{
		if (item->GetRadius() > 1.5f)
		{
			item->Shot();
		}
	}

	m_createBulletFrame++;
	
	if (m_createBulletFrame > kCreateBulletFrameMax)
	{
		m_createBulletFrame = 0;

		m_cylinderPos = MV1GetFramePosition(m_modelHandle, m_cylinderIndex);
		m_cannonSphere.push_back(std::make_shared<CannonSphere>(m_bulletTag, m_cylinderPos, Vec3(0, 0, 1), m_bulletColor));
		MyEngine::Physics::GetInstance().Entry(m_cannonSphere.back());

	}
}

void Cannon::Draw()
{
	MV1DrawModel(m_modelHandle);
	
}

void Cannon::SetMatrix()
{
	Quaternion myQ;

	Vec3 axis = Cross(m_upVec, m_frontVec);
	axis.Normalize();
	DrawLine3D(m_rigid->GetPos().VGet(), (m_rigid->GetPos() + axis * 60).VGet(), 0xff00ff);
	//myQ =myQ.QMult(myQ,myQ.CreateRotationQuaternion(angle, axis));
	myQ = myQ.QMult(myQ, myQ.CreateRotationQuaternion(atan2(-m_frontVec.x, -m_frontVec.z), m_upVec));
	auto rotatemat = myQ.ToMat();


	MV1SetRotationMatrix(m_modelHandle, rotatemat /*Vec3(0, atan2(moveDir.z, -moveDir.x) + DX_PI_F / 2, 0).VGet()*/);


	m_postUpVec = m_upVec;//上方向ベクトルを前のフレームの上方向ベクトルにする

	//MV1SetRotationMatrix(m_modelHandle, rotatemat);//回転行列を反映
	MV1SetPosition(m_modelHandle, m_rigid->GetPos().VGet());

	MV1SetRotationZYAxis(m_modelHandle, (m_frontVec * -1).VGet(), m_upVec.GetNormalized().VGet(), 0);
}

void Cannon::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{

}
