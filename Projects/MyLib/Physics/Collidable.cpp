#include "Collidable.h"
#include "ColliderSphere.h"
#include "ColliderBox.h"
#include"ColliderPolygonModel.h"
#include"ColliderLine3D.h"
#include<cassert>


using namespace MyEngine;

namespace
{
	//他のオブジェクトの状態を認識できるようにすべてのオブジェクトの状態をぶち込んでるが、無駄があるので何とかしたい
	std::map<Collidable::State, std::string> kStateName
	{
		{Collidable::State::Intro,"Intro"},
		{Collidable::State::Neutral,"Neutral"},
		{Collidable::State::Walk,"Walk"},
		{Collidable::State::Dash,"Dash"},
		{Collidable::State::Jump,"Jump"},
		{Collidable::State::DashJump,"DashJump"},
		{Collidable::State::JumpBoost,"JumpBoost"},
		{Collidable::State::Spin,"Spin"},
		{Collidable::State::Roll,"Roll"},
		{Collidable::State::JumpDrop,"JumpDrop"},
		{Collidable::State::FullpowerJumpDrop,"FullPowerJumpDrop"},
		{Collidable::State::Land,"Land"},
		{Collidable::State::Damage,"Damage"},
		{Collidable::State::Boosting,"Boosting"},
		{Collidable::State::Operation,"Oparation"},
		{Collidable::State::Search,"Search"},
		{Collidable::State::Chase,"Chase"},
		{Collidable::State::ComeBack,"ComeBack"},
		{Collidable::State::Attack,"Attack"},
		{Collidable::State::Tackle,"Tackle"},
		{Collidable::State::Stan,"Stan"},
		{Collidable::State::Death,"Death"},
		{Collidable::State::Talk,"Talk"},
		{Collidable::State::End,"End"}
	};
}

Collidable::Collidable(Priority priority, ObjectTag tag) :
	m_priority(priority),
	m_tag(tag),
	m_userData(nullptr),
	m_state(State::Neutral),
	m_isAntiGravity(false),
	m_isDestroy(false),
	m_isIgnore(false),
	m_upVec(Vec3::Up()),
	m_postUpVec(Vec3::Up()),
	m_nextUpVec(Vec3::Up()),
	m_frontVec(Vec3::Front()),
	m_sideVec(Vec3::Left()),
	m_power(0)
{
	m_rigid = std::make_shared<Rigidbody>();
	m_cbuffH = CreateShaderConstantBuffer(sizeof(UserData));
	m_userData= static_cast<UserData*>(GetBufferShaderConstantBuffer(m_cbuffH));
	m_userData->dissolveY = 1.0f;
	m_isActive = true;
}

MyEngine::Collidable::Collidable(std::shared_ptr<Collidable> col) :
	m_rigid(col->m_rigid),
	m_colliders(col->m_colliders),
	m_upVec(col->m_upVec),
	m_throughTags(col->m_throughTags),
	m_tag(col->m_tag),
	m_priority(col->m_priority),
	m_userData(nullptr),
	m_state(State::Neutral),
	m_isAntiGravity(false),
	m_isDestroy(false),
	m_isIgnore(false),
	m_postUpVec(Vec3::Up()),
	m_nextUpVec(Vec3::Up()),
	m_frontVec(Vec3::Front()),
	m_sideVec(Vec3::Left()),
	m_power(0)
{
	m_rigid = std::make_shared<Rigidbody>();
}

Collidable::~Collidable()
{
}

std::shared_ptr<Collidable::CollideInfo> MyEngine::Collidable::AddCollider(const ColliderBase::Kind& kind, const ColideTag& tag)
{
	std::shared_ptr<CollideInfo> collider;
	collider = std::make_shared<CollideInfo>();

	if (kind == ColliderBase::Kind::Sphere)
	{
		collider->col = std::make_shared<ColliderSphere>();

	}
	else if (kind == ColliderBase::Kind::Capsule)
	{

	}
	else if (kind == ColliderBase::Kind::Box)
	{
		collider->col = std::make_shared<ColliderBox>();
	}
	else if (kind == ColliderBase::Kind::Polygons)
	{
		collider->col = std::make_shared<ColliderPolygonModel>();
	}
	else if (kind == ColliderBase::Kind::Line)
	{
		collider->col = std::make_shared<ColliderLine3D>();
	}
	collider->tag = tag;
	if (collider)
	{
		m_colliders.emplace_back(collider);
	}
	
	return collider;

}
void MyEngine::Collidable::RemoveCollider(std::shared_ptr<Collidable::CollideInfo> col)
{
	auto it = std::find(m_colliders.begin(), m_colliders.end(), col);
	if (it == m_colliders.end()) return;
	m_colliders.erase(it);
}
std::string MyEngine::Collidable::GetStateName() const
{
	return kStateName[m_state];
}
/// <summary>
/// 当たり判定を無視（スルー）するタグの追加
/// </summary>
void Collidable::AddThroughTag(ObjectTag tag)
{
	bool found = (std::find(m_throughTags.begin(), m_throughTags.end(), tag) != m_throughTags.end());
	if (found)
	{
		assert(0 && L"指定タグは既に追加されています");
	}
	else
	{
		m_throughTags.emplace_back(tag);
	}
}

/// <summary>
/// 当たり判定を無視（スルー）するタグの削除
/// </summary>
void Collidable::RemoveThroughTag(ObjectTag tag)
{
	bool found = (std::find(m_throughTags.begin(), m_throughTags.end(), tag) != m_throughTags.end());
	if (!found)
	{
		assert(0 && L"指定タグは存在しません");
	}
	else
	{
		m_throughTags.remove(tag);
	}
}

// 当たり判定を無視（スルー）する対象かどうか
bool Collidable::IsThroughTarget(const std::shared_ptr<Collidable> target) const
{
	bool found = (std::find(m_throughTags.begin(), m_throughTags.end(), target->GetTag()) != m_throughTags.end());
	return found;
}