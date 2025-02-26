#pragma once
#include <memory>
#include <list>
#include <functional>
#include"Vec3.h"
#include "ObjectTag.h"
#include <DxLib.h>
#include "Collidable.h"


namespace MyEngine
{
	class Galaxy;
	class Rigidbody;
	class Collidable;
	class ColliderBase;

	class Physics final
	{
	private:
		friend Galaxy;

		enum class OnCollideInfoKind
		{
			CollideEnter,
			CollideStay,
			CollideExit,
			TriggerEnter,
			TriggerStay,
			TriggerExit
		};

		struct CollideHitInfo
		{
			bool isHit = false;
			Vec3 hitPos;
			Vec3 Norm;
		};

		struct CollideFuncArgs
		{
			std::shared_ptr<Collidable> send;
			Collidable::CollideInfo myCollide;
			Collidable::CollideInfo sendCollide;
		};
		struct SendInfo
		{
			std::weak_ptr<Collidable> own;
			std::weak_ptr<Collidable> send;
			ColideTag ownTag;
			ColideTag sendTag; 
			Vec3 hitPos;
		};
		struct OnCollideInfoData
		{
			std::weak_ptr<Collidable> own;
			std::weak_ptr<Collidable> send;
			ColideTag ownTag;
			ColideTag sendTag;
			Vec3 hitPos;
			OnCollideInfoKind kind;
		};
		using SendCollideInfo = std::list<SendInfo>;
	private:

		Physics();

		Physics(const Physics&) = delete;
		void operator= (const Physics&) = delete;

	public:

		static Physics& GetInstance();

		void Entry(const std::shared_ptr<Collidable>& collidable);
		void Exit(const std::shared_ptr<Collidable>& collidable);
		/// <summary>
		/// オブジェクトの初期情報追加
		/// 
		/// </summary>
		/// <param name="col"></param>
		void Initialize(std::shared_ptr<Collidable> col);
		void Update();
		void Draw();

		void Clear();

		std::vector<std::shared_ptr<Collidable>> m_collidables;

	private:
		
		/// <summary>
		/// オブジェクトに重力をかける
		/// </summary>
		void UpdatePlanetPhysics();
		//摩擦をかける
		void Friction();

		void MoveNextPos() const;
		/// <summary>
		/// 判定リストの取得
		/// </summary>
		/// <returns>判定リスト</returns>
		std::vector<std::shared_ptr<Collidable>> GetCollisionList() const;
		void CheckCollide();

		CollideHitInfo IsCollide(const std::shared_ptr<Rigidbody> rigidA, const std::shared_ptr<Rigidbody> rigidB, const std::shared_ptr<Collidable::CollideInfo> & colliderA, const std::shared_ptr<Collidable::CollideInfo>& colliderB) const;
		void FixNextPos(const std::shared_ptr<Rigidbody> primaryRigid, std::shared_ptr<Rigidbody> secondaryRigid, const std::shared_ptr<Collidable::CollideInfo>& primaryCollider, const std::shared_ptr<Collidable::CollideInfo>& secondaryCollider, const CollideHitInfo info);
		/// <summary>
		/// 判定通知リストに追加する
		/// </summary>
		/// <param name="objA">オブジェクトA</param>
		/// <param name="objB">オブジェクトB</param>
		/// <param name="info">通知リスト</param>
		/// <param name="hitPos">当たった座標</param>
		void AddNewCollideInfo(const std::weak_ptr<Collidable>& objA, const std::weak_ptr<Collidable>& objB, ColideTag ownTag, ColideTag sendTag, SendCollideInfo& info, const Vec3& hitPos = Vec3()); void CheckSendOnCollideInfo(SendCollideInfo& preSendInfo, SendCollideInfo& newSendInfo, bool isTrigger);
		/// <summary>
		/// obj1とobj2の当たり情報をどちらも格納
		/// </summary>
		/// <param name="info"></param>
		/// <param name="kind"></param>
		void AddOnCollideInfo(const SendInfo& info, OnCollideInfoKind kind);
		void OnCollideInfo(const std::weak_ptr<Collidable>& own, const std::weak_ptr<Collidable>& send, ColideTag ownTag, ColideTag sendTag, const Vec3& hitPos, OnCollideInfoKind kind);
		void FixPos() const;

		
		/// <summary>
		/// その球は辺と接触してますかぁぁぁあ！！？
		/// </summary>
		/// <param name="point">判定したい球の中心点</param>
		/// <param name="v0">辺の始点</param>
		/// <param name="v1">辺の終点</param>
		/// <param name="radius">球の半径</param>
		/// <returns>true:してまぁす！！,false:してますぇん</returns>
		bool IsPointOnEdge(const Vec3& point, float radius, const Vec3& v0, const Vec3& v1) const;
	private:
		
		std::vector<std::shared_ptr<Collidable>> m_stageCollidables;

		std::list<OnCollideInfoData> m_onCollideInfo;
		SendCollideInfo m_newCollideInfo;
		SendCollideInfo m_preCollideInfo;
		SendCollideInfo m_newTirrigerInfo;
		SendCollideInfo m_preTirrigerInfo;

		int m_shadowHandle;
		std::vector<int> m_shadowHandles;
	};
}

