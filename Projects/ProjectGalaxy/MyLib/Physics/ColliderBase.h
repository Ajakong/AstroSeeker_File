#pragma once
#include <unordered_map>
#include<memory>
#include"Vec3.h"
#include<string>
#include"ColideTag.h"

namespace MyEngine
{
	class ColliderBase abstract
	{
	public:
		// 当たり判定種別
		enum class Kind
		{
			Sphere,
			Capsule,
			Box,
			Polygons,
			Line
		};

		// コンストラクタ
		ColliderBase(Kind kind);
		virtual ~ColliderBase() {}		

		virtual void DebugDraw(Vec3 pos) {}
		// 当たり判定種別取得
		Kind GetKind() const { return m_kind; }
		// 当たった情報更新
		void UpdateHit(const std::shared_ptr<ColliderBase> collider, bool isHit);
		bool IsHit(const std::shared_ptr<ColliderBase> collider) const { return m_isHit.at(collider); }
		bool IsPreHit(const std::shared_ptr<ColliderBase> collider) const { return m_isPreHit.at(collider); }
		//bool NowOnHit() { return m_isNowOnHit; }
		//bool PreOnHit() { return m_isPreOnHit; }
		bool OnHit() { return m_isHitResult; }
		void SetNowOnHit(bool flag) { m_isNowOnHit = flag; }
		void SetOnHitResult(bool flag) { m_isHitResult = flag; }
		void SetPreOnHit(bool flag) { m_isPreOnHit = flag; }
		Vec3 GetShift() const{ return m_posShift; }
		/// <summary>
		/// オブジェクトの座標から当たり判定の相対位置をセット
		/// </summary>
		/// <param name="shift"></param>
		void SetShiftPosNum(Vec3 shift) { m_posShift = shift; }
	public:
		// MEMO: 色々なところで使うためColliderBase関係のpublic変数は
		// メンバ変数のコーディング規約無視している
		bool m_isTrigger;

	protected:
		//オブジェクトの位置からの当たり判定の相対量
		Vec3 m_posShift;
		Kind m_kind;
		std::unordered_map<std::shared_ptr<ColliderBase>, bool> m_isHit;
		std::unordered_map<std::shared_ptr<ColliderBase>, bool> m_isPreHit;
		//ライブラリの構造上2フレームの間衝突していなかった場合衝突していない判定とする
		bool m_isNowOnHit;
		bool m_isPreOnHit;
		bool m_isHitResult;
	};


}

