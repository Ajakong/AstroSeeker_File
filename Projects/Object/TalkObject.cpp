#include "TalkObject.h"
#include"ColliderSphere.h"

#include"Easing.h"

namespace
{
	constexpr float kTalkingRadius = 20.f;
	constexpr float kRadius = 5.f;
	constexpr float kScaleMag = 0.5f;

}

TalkObject::TalkObject(Vec3 pos, int modelHandle) : Collidable(Priority::Static, ObjectTag::TalkObject),
m_canTalk(false)
{
	SetAntiGravity(true);
	m_modelHandle = modelHandle;
	MV1SetScale(m_modelHandle, VGet(kScaleMag, kScaleMag, kScaleMag));
	MV1SetPosition(m_modelHandle, m_rigid->GetPos().VGet());
	m_rigid->SetPos(pos);
	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::Body);
		auto col = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		col->radius = kRadius;
	}

	{
		AddCollider(MyEngine::ColliderBase::Kind::Sphere, ColideTag::one);
		auto item = dynamic_pointer_cast<MyEngine::ColliderSphere>(m_colliders.back()->col);
		item->radius = kTalkingRadius;
		item->m_isTrigger = true;
		
	}
	

	m_update = &TalkObject::WaitingUpdate;
	m_draw = &TalkObject::NormalDraw;

}

TalkObject::~TalkObject()
{
}

void TalkObject::Init()
{
}

void TalkObject::Update()
{
	//ローカル上方向ベクトルをいい感じ線形保管
	m_upVec = Slerp(m_upVec, m_nextUpVec, 1.f);
	(this->*m_update)();
	SetMatrix();
}

void TalkObject::Draw()
{
	(this->*m_draw)();
}

void TalkObject::SetMatrix()
{

	float angle = GetAngle(m_postUpVec, m_upVec);//前のフレームの上方向ベクトルと今の上方向ベクトル

	//printf("角度1=%f\n角度2=%f\n", angle, angle * 180.0f / 3.141592f);

	Vec3 axis = Cross(m_upVec, m_frontVec);//上方向ベクトルと進行方向ベクトルの外積から回転軸を生成
	axis.Normalize();//単位ベクトル化

	m_myQ = m_myQ.CreateRotationQuaternion(angle, axis) * m_myQ;//回転の掛け算

	auto rotatemat = m_myQ.ToMat();//クォータニオンから行列に変換

	//printf("x:%f,y:%f,z:%f\n", axis.x, axis.y, axis.z);


#ifdef DEBUG
	//回転軸のデバッグ表示(紫)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + axis * 100).VGet(), 0xff00ff);

	//上方向ベクトルのデバッグ表示(赤)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_upVec * 100).VGet(), 0xff0000);

	//1フレーム前の上ベクトルの表示(暗赤)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_postUpVec * 100).VGet(), 0xaa0000);

	//進行方向ベクトルのデバッグ表示(黄色)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_frontVec * 100).VGet(), 0xffff00);

	//右側ベクトルのデバッグ表示(緑)
	DrawLine3D(m_rigid->GetPos().VGet(), Vec3(m_rigid->GetPos() + m_sideVec * 100).VGet(), 0x00ff00);

#endif 

	m_postUpVec = m_upVec;//上方向ベクトルを前のフレームの上方向ベクトルにする

	MV1SetRotationZYAxis(m_modelHandle, (m_frontVec * -1).VGet(), m_upVec.GetNormalized().VGet(), 0);

	//MV1SetRotationMatrix(m_modelHandle, rotatemat);//回転行列を反映
	MV1SetPosition(m_modelHandle, m_rigid->GetPos().VGet());

	//MV1SetRotationZYAxis(m_modelHandle, (m_frontVec * -1).VGet(), m_upVec.GetNormalized().VGet(), 0);
	
	auto modelMat = MV1GetMatrix(m_modelHandle);
}

std::list<std::list<std::string>> TalkObject::GetTexts()
{
	

	return m_talkTexts;
}

void TalkObject::CanWeJustTalk()
{

}

void TalkObject::WaitingUpdate()
{
}

void TalkObject::NormalDraw()
{
	//DrawSphere3D(m_rigid->GetPos().VGet(), kRadius, 7, 0x00ff00, 0xffffff, true);
	//DrawSphere3D(m_rigid->GetPos().VGet(), kTalkingRadius, 7, 0x00ff00, 0xffffff, false);
	MV1DrawModel(m_modelHandle);
}

void TalkObject::OnTriggerEnter(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Player)
	{
		UI::GetInstance().WannaTalk(std::dynamic_pointer_cast<TalkObject>(shared_from_this()));
		m_canTalk = true;

	}
}

void TalkObject::OnTriggerStay(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Player)
	{
		m_frontVec = m_rigid->GetPos()-colider->GetRigidbody()->GetPos();
		m_frontVec.Normalize();
	}
}

void TalkObject::OnTriggerExit(std::shared_ptr<Collidable> colider, ColideTag ownTag, ColideTag targetTag)
{
	if (colider->GetTag() == ObjectTag::Player)
	{
		UI::GetInstance().TalkExit();
		m_update = &TalkObject::WaitingUpdate;
	}
}
