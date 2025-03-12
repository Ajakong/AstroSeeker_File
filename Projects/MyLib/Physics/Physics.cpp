#include "Physics.h"
#include <cassert>
#include <cmath>
#include <algorithm>
#include"CollisionUtil.h"

#include "ColliderSphere.h"
#include"ColliderBox.h"
#include"ColliderPolygonModel.h"
#include"ColliderLine3D.h"
#include "DebugDraw.h"
#include"Planet.h"

#include"GraphManager.h"
#include"ModelManager.h"

#include"UtilFunc.h"

#include"Easing.h"

using namespace MyEngine;

namespace
{
	// 判定最大回数
	constexpr int CHECK_COUNT_MAX = 100;


	constexpr float CHECK_COLLIDE_LENDGHT = 50.0f;
	constexpr float CHECK_COLLIDE_SQ_LENDGHT = CHECK_COLLIDE_LENDGHT * CHECK_COLLIDE_LENDGHT * 10;

	const char* kShadowGraphName = "shadow.png";
	const char* kShadowModelName = "Shadow";
}

Physics::Physics()
{
	m_shadowHandle = GraphManager::GetInstance().GetGraphData(kShadowGraphName);
}

Physics& Physics::GetInstance()
{
	static Physics instance;
	return instance;
}

void Physics::Entry(const std::shared_ptr<Collidable>& collidable)
{
	bool isFound = std::find(m_collidables.begin(), m_collidables.end(), collidable) != m_collidables.end();
	//// 未登録なら追加
	//if (!isFound)
	//{
	//    m_collidables.emplace_back(collidable);
	//}
	//// 登録済みなら無視
	//else
	//{
	//    assert(false);
	//}

	// 未登録なら追加
	if (!isFound && collidable->GetTag() != ObjectTag::Stage)
	{
		m_collidables.emplace_back(collidable);
	}
	else if (!isFound)
	{

		m_stageCollidables.emplace_back(collidable);
		m_collidables.emplace_back(collidable);
	}
	// 登録済みなら無視
	else
	{
		//assert(false);
	}
	collidable->Init();
}

void Physics::Exit(const std::shared_ptr<Collidable>& collidable)
{
	auto it = std::find(m_collidables.begin(), m_collidables.end(), collidable);
	auto stageit = std::find(m_stageCollidables.begin(), m_stageCollidables.end(), collidable);

	if (stageit != m_stageCollidables.end())
	{
		int index = static_cast<int>(std::distance(m_stageCollidables.begin(), stageit));
		auto iterater = m_stageCollidables.begin();
		for (int i = 0; i < index; i++)
		{
			iterater++;
		}
		m_stageCollidables.erase(iterater);
	}

	// 登録済みなら削除
	if (it != m_collidables.end())
	{
		int index = static_cast<int>(std::distance(m_collidables.begin(), it));
		auto iterater = m_collidables.begin();
		for (int i = 0; i < index; i++)
		{
			iterater++;
		}
		m_collidables.erase(iterater);
	}
	// 未登録なら無視
	else
	{
		//assert(false);
	}
}

void Physics::Update()
{

	std::reverse_iterator<std::vector<int>::iterator> rit;
	for (auto& item : std::vector<std::shared_ptr<MyEngine::Collidable>>(m_collidables.rbegin(), m_collidables.rend()))//途中で要素を削除してもいいように逆順
	{
		//次の上方向ベクトルを設定(このオブジェクトに影響している全重力の合計の反対方向になってるので方向だけにする)
		item->SetNextUpVec(item->GetNextUpVec().GetNormalized());

		//補完しながら現在の上方向ベクトルを設定
		item->SetUpVec(Slerp(item->GetUpVec(), item->GetNextUpVec(), 0.1f));
		
		//右方向ベクトルを設定
		item->SetSideVec(Cross(item->GetFrontVec(), item->GetUpVec()));
		if (!item->GetIsActive())continue;
		item->Update();
	}

	// 通知リストのクリア・更新
	m_onCollideInfo.clear();
	m_preCollideInfo = m_newCollideInfo;
	m_newCollideInfo.clear();
	m_preTirrigerInfo = m_newTirrigerInfo;
	m_newTirrigerInfo.clear();

	// 次の移動先を仮決定
	MoveNextPos();
	for (auto& item : std::vector<std::shared_ptr<MyEngine::Collidable>>(m_collidables.rbegin(), m_collidables.rend()))//途中で要素を削除してもいいように逆順
	{
		for (auto& col : item->m_colliders)
		{
			col->col->SetOnHitResult(false);
		}
	}

	//次の位置を決定した後ではないと補正されて衝突していなくなる
	UpdatePlanetPhysics();
	Friction();

	// 判定確認
	CheckCollide();

	// 通知リストを確認
	CheckSendOnCollideInfo(m_preCollideInfo, m_newCollideInfo, false);
	CheckSendOnCollideInfo(m_preTirrigerInfo, m_newTirrigerInfo, true);

	// 座標を決定
	FixPos();

	// 通知を送る
	for (const auto& item : m_onCollideInfo)
	{
		OnCollideInfo(item.own, item.send, item.ownTag, item.sendTag, item.hitPos, item.kind);
	}

	for (auto& item : std::vector<std::shared_ptr<MyEngine::Collidable>>(m_collidables.rbegin(), m_collidables.rend()))//途中で要素を削除してもいいように逆順
	{
		if (item->IsDestroy())Exit(item);
	}
}

void MyEngine::Physics::Draw()
{
	for (const auto& obj : m_collidables)
	{
		if (!obj->GetIsActive())continue;
		
		obj->Draw();

#ifdef _DEBUG
		Vec3 objPos = obj->GetRigidbody()->GetPos();

		DrawLine3D(objPos.VGet(), (objPos + obj->GetUpVec() * 6).VGet(),0xff0000);
		DrawLine3D(objPos.VGet(), (objPos + obj->GetFrontVec() * 6).VGet(), 0x00ff00);
		DrawLine3D(objPos.VGet(), (objPos + obj->GetSideVec() * 6).VGet(), 0x0000ff);
#endif

	}

	m_shadowHandles.clear();
	ModelManager::GetInstance().ClearShadowModel();
}

void MyEngine::Physics::Clear()
{
	m_collidables.clear();
	m_stageCollidables.clear();

	m_onCollideInfo.clear();
	m_newCollideInfo.clear();
	m_preCollideInfo.clear();
	m_newTirrigerInfo.clear();
	m_preTirrigerInfo.clear();
}

void MyEngine::Physics::Initialize(std::shared_ptr<Collidable> collidable)
{
	// 判定リストはペアになっているので半分の数だけ繰り返す
	for (auto& stage : m_stageCollidables)
	{

		for (auto& col : collidable->m_colliders)
		{
			//Stage同士なら無視
			if (collidable->GetTag() == ObjectTag::Stage)continue;
			//距離が離れすぎているオブジェクトも無視
			if ((collidable->GetRigidbody()->GetPos() - stage->GetRigidbody()->GetPos()).Length() > 500)continue;
			for (auto stageCol : stage->m_colliders)
			{
				auto& colA = stageCol;
				auto& colB = col;

				// 判定
				auto collideHitInfo = IsCollide(stage->m_rigid, collidable->m_rigid, colA, colB);
				// 当たっていなければ次の判定に
				if (!collideHitInfo.isHit) continue;

				// 通過オブジェクト確認
				auto throughA = stage->m_throughTags;
				auto throughB = collidable->m_throughTags;
				bool isThrough = std::find(throughA.begin(), throughA.end(), collidable->GetTag()) != throughA.end()
					|| std::find(throughB.begin(), throughB.end(), stage->GetTag()) != throughB.end();
				// isTriggerがtrueか通過オブジェクトなら通知だけ追加して次の判定に
				bool isTrigger = colA->col->m_isTrigger || colB->col->m_isTrigger || isThrough;
				if (isTrigger)
				{
					if (collidable->GetTag() == ObjectTag::Player)
					{
						printf("Player");
						if (colB->tag == ColideTag::Foot)
						{
							printf("の足");
						}
					}
					//重力はオブジェクトごとに一回のみ
					//重力の強さぶんベクトルに加算
					if (colB->tag != ColideTag::Body)continue;
					auto planet = dynamic_cast<Planet*>(stage.get());
					collidable->m_rigid->AddVelocity(planet->GravityEffect(collidable));
					collidable->m_rigid->SetNextPos(collidable->m_rigid->GetPos() + collidable->m_rigid->GetVelocity());
					continue;
				}
			}

		}


	}
}

void MyEngine::Physics::UpdatePlanetPhysics()
{
	// 判定リストはペアになっているので半分の数だけ繰り返す
	for (auto& stage : m_stageCollidables)
	{
		// それぞれが持つ判定全てを比較
		for (auto& object : m_collidables)
		{
			if (!object->GetIsActive())continue;
			for (auto& col : object->m_colliders)
			{
				//Stage同士なら無視
				if (object->GetTag() == ObjectTag::Stage)continue;
				//距離が離れすぎているオブジェクトも無視
				if ((object->GetRigidbody()->GetPos() - stage->GetRigidbody()->GetPos()).Length() > 500)continue;
				for (auto stageCol : stage->m_colliders)
				{
					auto& colA = stageCol;
					auto& colB = col;

					// 判定
					auto collideHitInfo = IsCollide(stage->m_rigid, object->m_rigid, colA, colB);

					if (!collideHitInfo.isHit) continue;
					
					auto planet = std::dynamic_pointer_cast<Planet>(stage);

					Vec3 planetPos = stage->GetRigidbody()->GetPos();
					Vec3 objectPos = object->GetRigidbody()->GetPos();
					/*Vec3 norm = object->GetRigidbody()->GetPos() - stage->GetRigidbody()->GetPos();
					norm.Normalize();*/
					// オブジェクトから惑星の中心へのベクトルを計算
					Vec3 direction = planetPos - objectPos;
					direction.Normalize();

					// 線分の始点と終点を設定
					Vec3 start = planetPos.VGet();
					Vec3 end = objectPos.VGet();

					// 衝突判定を実行
					MV1_COLL_RESULT_POLY collisionResult = MV1CollCheck_Line(
						planet->GetModelHandle(), // 惑星モデルのハンドル
						-1,                       // 全てのフレームを対象
						start.VGet(),                    // 線分の始点
						end .VGet()                      // 線分の終点
					);

					//体の判定のみ(一回)
					bool isHit = collisionResult.HitFlag == 1;
					if (isHit && colB->tag == ColideTag::Body) {
						//モデルと線分が衝突した地点
						Vec3 hitPosition = collisionResult.HitPosition;
						int shadowHandle = ModelManager::GetInstance().GetModelData(kShadowModelName,true);

						//惑星からオブジェクトまでの法線
						Vec3 norm = (end - start).GetNormalized();

						MATRIX scaleMat = MGetScale(VGet(0.05f, 0.0f, 0.05f)); // Y軸方向に0.1倍のスケーリング

						// 回転行列の作成（モデルの上方向ベクトルを法線ベクトルに合わせる）
						MATRIX rotMat = MGetRotVec2(Vec3::Up().VGet(), norm.VGet());

						// 平行移動行列の作成（モデルを衝突位置に移動）
						MATRIX transMat = MGetTranslate((hitPosition+norm).VGet());

						// 変換行列の合成（スケーリング → 回転 → 平行移動の順）
						MATRIX transformMat = MMult(scaleMat, rotMat);
						transformMat = MMult(transformMat, transMat);

						// モデルの行列を設定
						MV1SetMatrix(shadowHandle, transformMat);

						SetDrawBlendMode(DX_BLENDMODE_ALPHA, 50);
						// モデルを描画
						MV1DrawModel(shadowHandle);
						SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
					}



					// 通過オブジェクト確認
					auto throughA = stage->m_throughTags;
					auto throughB = object->m_throughTags;

					bool isThrough = std::find(throughA.begin(), throughA.end(), object->GetTag()) != throughA.end()
						|| std::find(throughB.begin(), throughB.end(), stage->GetTag()) != throughB.end();
					// isTriggerがtrueか通過オブジェクトなら通知だけ追加して次の判定に
					bool isTrigger = colA->col->m_isTrigger || colB->col->m_isTrigger || isThrough;


					//if (object->IsAntiGravity())continue;
					if (isTrigger)
					{
						if (object->GetTag() == ObjectTag::Player)
						{
							printf("Player");
							if (colB->tag == ColideTag::Foot)
							{
								printf("の足");
							}
						}
						//重力はオブジェクトごとに一回のみ
						//重力の強さぶんベクトルに加算
						if (colB->tag != ColideTag::Body)continue;
						auto planet = dynamic_cast<Planet*>(stage.get());
						object->m_rigid->AddVelocity(planet->GravityEffect(object));
						object->m_rigid->SetNextPos(object->m_rigid->GetPos() + object->m_rigid->GetVelocity());
						continue;
					}
					else
					{
						////摩擦力は触れている面積が多いほど強くなるため、そのオブジェクトの当たり判定が多く当たっているほどさらに上乗せする
						//colB->col->SetOnHitResult(true);
						//printf("Planetの地面と");
						//if (object->GetTag() == ObjectTag::Player)
						//{
						//	printf("Player");
						//	if (colB->tag == ColideTag::Foot)
						//	{
						//		printf("の足");
						//	}
						//}
						//else
						//{
						//	printf("なにか");
						//}
						//printf("が当たりました\n");
						//auto planet = dynamic_cast<Planet*>(stage.get());
						//object->m_rigid->SetVelocity(planet->FrictionEffect(object));

						//continue;
					}
				}

			}

		}
	}
}

void MyEngine::Physics::Friction()
{
	// 判定リストはペアになっているので半分の数だけ繰り返す
	for (auto& stage : m_stageCollidables)
	{
		// それぞれが持つ判定全てを比較
		for (auto& object : m_collidables)
		{
			if (!object->GetIsActive())continue;
			if (object->IsAntiGravity())continue;
			for (auto& col : object->m_colliders)
			{
				//Stage同士なら無視
				if (object->GetTag() == ObjectTag::Stage)continue;
				//距離が離れすぎているオブジェクトも無視
				if ((object->GetRigidbody()->GetPos() - stage->GetRigidbody()->GetPos()).Length() > 500)continue;
				for (auto stageCol : stage->m_colliders)
				{
					auto& colA = stageCol;
					auto& colB = col;

					// 判定
					auto collideHitInfo = IsCollide(stage->m_rigid, object->m_rigid, colA, colB);
					// 当たっていなければ次の判定に
					if (!collideHitInfo.isHit) continue;

					// 通過オブジェクト確認
					auto throughA = stage->m_throughTags;
					auto throughB = object->m_throughTags;
					bool isThrough = std::find(throughA.begin(), throughA.end(), object->GetTag()) != throughA.end()
						|| std::find(throughB.begin(), throughB.end(), stage->GetTag()) != throughB.end();
					// isTriggerがtrueか通過オブジェクトなら通知だけ追加して次の判定に
					bool isTrigger = colA->col->m_isTrigger || colB->col->m_isTrigger || isThrough;
					if (isTrigger)
					{

					}
					else
					{
						//摩擦力は触れている面積が多いほど強くなるため、そのオブジェクトの当たり判定が多く当たっているほどさらに上乗せする
						colB->col->SetOnHitResult(true);
						printf("Planetの地面と");
						if (object->GetTag() == ObjectTag::Player)
						{
							printf("Player");
							if (colB->tag == ColideTag::Foot)
							{
								printf("の足");
							}
						}
						else
						{
							printf("なにか");
						}
						printf("が当たりました\n");
						auto planet = dynamic_cast<Planet*>(stage.get());
						object->m_rigid->SetVelocity(planet->FrictionEffect(object));

						continue;
					}
				}

			}

		}
	}
}

/// <summary>
/// 物理からの移動を未来の座標に適用
/// </summary>
void MyEngine::Physics::MoveNextPos() const
{

	//当たり判定をする必要があるペア同士の当たり判定のリスト(位置補正リスト)を取得

	//上記リストから、どれとどれが当たっているかを出す(当たり判定ペアリスト)を取得


	//上記の結果から、位置補正を行う

	for (const auto& item : m_collidables)
	{
		auto& rigid = item->m_rigid;

		auto pos = rigid->GetPos();
		auto nextPos = pos + rigid->GetVelocity();

		//次フレームの位置を設定
		rigid->SetNextPos(nextPos);

#ifdef DEBUG
		auto& debug = DebugDraw::GetInstance();

		for (const auto& collider : item->m_colliders)
		{
			auto kind = collider->col->GetKind();
			if (kind == ColliderBase::Kind::Sphere)
			{
				auto sphereData = dynamic_pointer_cast<ColliderSphere>(collider->col);
				DebugDraw::SphereInfo preInfo;
				preInfo.center = pos;
				preInfo.radius = sphereData->radius;
				preInfo.color = DebugDraw::COL_BEFOR;
				debug.DrawSphere(preInfo);
				DebugDraw::SphereInfo newInfo;
				newInfo.center = nextPos;
				newInfo.radius = sphereData->radius;
				newInfo.color = DebugDraw::COL_NEXT;
				debug.DrawSphere(newInfo);
			}
		}
#endif
	}
}


std::vector<std::shared_ptr<Collidable>> Physics::GetCollisionList() const
{
	std::vector<std::shared_ptr<Collidable>> res;

	for (int i = 0; i < m_collidables.size(); ++i)
	{
		for (int j = i + 1; j < m_collidables.size(); ++j)
		{
			auto& obj1 = m_collidables[i];
			auto& obj2 = m_collidables[j];
			// 移動しないオブジェクト同士なら判定しない
			if (obj1->GetPriority() == Collidable::Priority::Static && obj2->GetPriority() == Collidable::Priority::Static) continue;

			// 一定範囲内にいないなら判定しない
			const auto& pos1 = obj1->m_rigid->GetPos();
			const auto& pos2 = obj2->m_rigid->GetPos();
			if ((pos1 - pos2).SqLength() < CHECK_COLLIDE_SQ_LENDGHT)
			{
				// 判定リストに追加
				res.push_back(obj1);
				res.push_back(obj2);
			}
		}
	}

	return res;
}


void MyEngine::Physics::CheckCollide()
{

	//当たり判定をする必要があるペア同士の当たり判定のリスト(位置補正リスト)を取得
	const auto& colVec = GetCollisionList();

	//上記リストから、どれとどれが当たっているかを出す(当たり判定ペアリスト)を取得


	//上記の結果から、位置補正を行う



	// 判定リストはペアになっているので半分の数だけ繰り返す
	int colNum = static_cast<int>(colVec.size() * 0.5f);
	for (int i = 0; i < colNum; ++i)
	{
		auto objA = colVec[i * 2];
		auto objB = colVec[i * 2 + 1];

		if (!objA->GetIsActive() || !objB->GetIsActive())continue;
		// それぞれが持つ判定全てを比較
		for (int i = 0; i < objA->m_colliders.size(); ++i)
		{
			for (int j = 0; j < objB->m_colliders.size(); ++j)
			{
				auto& colA = objA->m_colliders.at(i);
				auto& colB = objB->m_colliders.at(j);

				////2フレームの間なにもHITしていなかったとき、衝突していない
				//if (!colA->col->PreOnHit() && !colA->col->NowOnHit())
				//{
				//	colA->col->SetOnHitResult(false);
				//}
				//if (!colB->col->PreOnHit() && !colB->col->NowOnHit())
				//{
				//	colB->col->SetOnHitResult(false);
				//}

				//colA->col->SetPreOnHit(colA->col->NowOnHit());
				//colB->col->SetPreOnHit(colB->col->NowOnHit());

				colA->col->SetNowOnHit(false);
				colB->col->SetNowOnHit(false);

				// 判定
				auto collideHitInfo = IsCollide(objA->m_rigid, objB->m_rigid, colA, colB);
				colA->col->UpdateHit(colB->col, collideHitInfo.isHit);
				colB->col->UpdateHit(colA->col, collideHitInfo.isHit);
				// 当たっていなければ次の判定に
				if (!collideHitInfo.isHit) continue;


				// 通過オブジェクト確認
				auto throughA = objA->m_throughTags;
				auto throughB = objB->m_throughTags;
				bool isThrough = std::find(throughA.begin(), throughA.end(), objB->GetTag()) != throughA.end()
					|| std::find(throughB.begin(), throughB.end(), objA->GetTag()) != throughB.end();
				// isTriggerがtrueか通過オブジェクトなら通知だけ追加して次の判定に
				bool isTrigger = colA->col->m_isTrigger || colB->col->m_isTrigger || isThrough;
				if (isTrigger)
				{
					AddNewCollideInfo(objA, objB, colA->tag, colB->tag, m_newTirrigerInfo, collideHitInfo.hitPos);
					continue;
				}
				//Triggerじゃなければ今当たってるフラグを立てる

				colA->col->SetOnHitResult(true);
				colB->col->SetOnHitResult(true);

				printf((ObjectTag_String(objA->m_tag) + "の" + ColideTag_String(colA->tag) + "と" + ObjectTag_String(objB->m_tag) + "の" + ColideTag_String(colB->tag) + "がHIT\n").c_str());
				printf((ObjectTag_String(objB->m_tag) + "の" + ColideTag_String(colB->tag) + "と" + ObjectTag_String(objA->m_tag) + "の" + ColideTag_String(colA->tag) + "がHIT\n").c_str());


				// 通知を追加
				AddNewCollideInfo(objA, objB, colA->tag, colB->tag, m_newCollideInfo, collideHitInfo.hitPos);

				// 優先度に合わせて変数を変更
				auto primary = objA;
				auto secondary = objB;
				auto primaryCollider = colA;
				auto secondaryCollider = colB;

				if (objA->m_priority < objB->m_priority)
				{
					primary = objB;
					secondary = objA;
					primaryCollider = colB;
					secondaryCollider = colA;
				}

				// 優先度が変わらない場合
				else if (objA->m_priority == objB->m_priority)
				{
					// 速度の速いを方を優先度が高いことにする
					if (objA->m_rigid->GetVelocity().SqLength() < objB->m_rigid->GetVelocity().SqLength())
					{
						primary = objB;
						secondary = objA;
						primaryCollider = colB;
						secondaryCollider = colA;
					}
				}

				// 次の座標を修正する
				FixNextPos(primary->m_rigid, secondary->m_rigid, primaryCollider, secondaryCollider, collideHitInfo);
				//FixNextPos(secondary->m_rigid, primary->m_rigid, secondaryCollider, primaryCollider, collideHitInfo);
			}
		}
	}
}

MyEngine::Physics::CollideHitInfo Physics::IsCollide(const std::shared_ptr<Rigidbody> rigidA, const std::shared_ptr<Rigidbody> rigidB, const std::shared_ptr<Collidable::CollideInfo>& colliderA, const std::shared_ptr<Collidable::CollideInfo>& colliderB) const
{

	CollideHitInfo info;

	auto kindA = colliderA->col->GetKind();
	auto kindB = colliderB->col->GetKind();
	//kindA=球の当たり判定
	if (kindA == ColliderBase::Kind::Sphere)
	{
		//ReadMe::同じオブジェクト同士はまあいいとして、違うオブジェクト同士は関数化してください。
		//下にAとB入れ替えただけの同じコードが書かれているじゃないですか。同じオブジェクト同士も関数化してください。判定関数だけ持つクラス作ってもいいかも
		if (kindB == ColliderBase::Kind::Sphere)
		{
			auto sphereA = dynamic_pointer_cast<ColliderSphere>(colliderA->col);
			auto sphereB = dynamic_pointer_cast<ColliderSphere>(colliderB->col);

			auto aToB = (rigidB->GetNextPos() + colliderB->col->GetShift()) - (rigidA->GetNextPos() + colliderA->col->GetShift());
			float sumRadius = sphereA->radius + sphereB->radius;
			info.isHit = (aToB.SqLength() <= sumRadius * sumRadius);
		}
		if (kindA == ColliderBase::Kind::Sphere && kindB == ColliderBase::Kind::Box)
		{
			auto SphereA = dynamic_pointer_cast<ColliderSphere>(colliderA->col);
			auto BoxB = dynamic_pointer_cast<ColliderBox>(colliderB->col);

			auto spherePos = rigidA->GetNextPos() + colliderA->col->GetShift();
			auto boxPos = rigidB->GetNextPos() + colliderB->col->GetShift();

			// ボックスの中心から円の中心までのベクトルを作成
			auto boxToSphere = spherePos - boxPos;
			auto radius = SphereA->radius;
			// 球に近い場所を求める
			auto nearPos = GetNearestPtOnBox(spherePos, boxPos, BoxB->size, BoxB->rotation);
			// 最近接点と球の中心との長さで判定
			auto nearToSphere = spherePos - nearPos;
			if (nearToSphere.SqLength() < radius * radius)
			{
				info.isHit = true;
				info.hitPos = nearPos;
			}
		}
		if (kindB == ColliderBase::Kind::Polygons)
		{
			//2Dの線分と球の当たり判定で行う

			auto sphereA = dynamic_pointer_cast<ColliderSphere>(colliderA->col);
			auto polygonB = dynamic_pointer_cast<ColliderPolygonModel>(colliderB->col);
			auto spherePos = rigidA->GetNextPos() + colliderA->col->GetShift();
			float sphereRadius = sphereA->radius;

			Vec3 closestHitPos;   // 球から最も近いポリゴンの最近接点
			Vec3 closestNormal;   // 最も近いポリゴンの法線
			float closestDistance = 500; // 最小距離(はじめは馬鹿でか距離を入れておいて最小を更新していく)
			bool isCollision = false; // 衝突フラグ
			bool isInside = false;    // 球がポリゴン内部かどうか

			// ポリゴンとの衝突チェック
			for (auto& triangle : polygonB->GetTriangles())
			{
				// 球から三角形への最寄り点を求める
				Vec3 nearPoint = GetClosestPointOnTriangle(spherePos, triangle.vertex[0], triangle.vertex[1], triangle.vertex[2]);
				float distance = (nearPoint - spherePos).Length();

				// 最も近い点を更新
				if (distance < closestDistance)
				{
					closestDistance = distance;
					closestHitPos = nearPoint;
					closestNormal = triangle.Normal();
				}

				// 球と三角形の最近接点の距離が球の半径以下なら衝突
				if (distance <= sphereRadius)
				{
					isCollision = true;
				}
			}

			//レイキャスティング法
			// 球がポリゴン内部にあるか判定（レイの交差回数をカウント）
			if (!isCollision)
			{
				int intersectionCount = 0;
				Vec3 rayDir = Vec3(1, 0, 0); // 任意の方向のレイ（X軸正方向）

				for (auto& triangle : polygonB->GetTriangles())
				{
					if (RayIntersectsTriangle(spherePos, rayDir, triangle.vertex[0], triangle.vertex[1], triangle.vertex[2]))
					{
						intersectionCount++;
					}
				}

				// 交差回数が奇数なら内部と判断
				isInside = (intersectionCount % 2 == 1);
			}

			// 衝突結果を設定
			info.isHit = isCollision || isInside;  // 衝突または内部判定が真なら衝突
			DrawSphere3D(closestHitPos.VGet(), 7, 7, 0xff000, 0xffffff, false);
			info.hitPos = closestHitPos;  // 常に最も近い点を格納
			info.Norm = closestNormal; // 最近接点からsphereへの向き
		}
		if (kindB == ColliderBase::Kind::Line)
		{
			auto sphereA = dynamic_pointer_cast<ColliderSphere>(colliderA->col);
			auto LineB = dynamic_pointer_cast<ColliderLine3D>(colliderB->col);

			// 球の中心位置
			auto spherePos = rigidA->GetNextPos() + colliderA->col->GetShift();
			float sphereRadius = sphereA->radius;

			// 線分の始点と終点
			auto line = LineB->GetLine();
			Vec3 lineStart = line.start;
			Vec3 lineEnd = line.end;

			// 線分の方向ベクトルと長さ
			Vec3 lineDir = lineEnd - lineStart;
			float lineLength = lineDir.Length();
			lineDir = lineDir.GetNormalized();

			// 球の中心から線分の始点へのベクトル
			Vec3 L = spherePos - lineStart;

			// 線分上の最短点を求める（t: 線分上の割合 [0, 1]）
			float t = Dot(L, lineDir) / lineLength;
			t = std::clamp(t, 0.0f, 1.0f); // t を [0, 1] に制限

			// 線分上の最短点
			Vec3 closestPoint = lineStart + lineDir * (t * lineLength);

#ifdef DEBUG
			// デバッグ用に最短点を描画
			DrawSphere3D(closestPoint.VGet(), 5, 5, 0xff0000, 0xff0000, false);

#endif
			// 球の中心からその最短点までの距離
			float distance = (closestPoint - spherePos).Length();

			// 距離が球の半径以下なら交差している
			if (distance <= sphereRadius + 1)
			{
				info.hitPos = closestPoint;
				info.isHit = true;
			}
		}
	}


	//kindA=立方体の当たり判定
	if (kindA == ColliderBase::Kind::Box)
	{
		//立方体と球
		if (kindB == ColliderBase::Kind::Sphere)
		{
			auto BoxA = dynamic_pointer_cast<ColliderBox>(colliderA->col);
			auto SphareB = dynamic_pointer_cast<ColliderSphere>(colliderB->col);

			auto spherePos = rigidB->GetNextPos() + colliderB->col->GetShift();
			auto boxPos = rigidA->GetNextPos() + colliderA->col->GetShift();

			// ボックスの中心から円の中心までのベクトルを作成
			auto boxToSphere = spherePos - boxPos;
			auto radius = SphareB->radius;
			// 球に近い場所を求める
			auto nearPos = GetNearestPtOnBox(spherePos, boxPos, BoxA->size, BoxA->rotation);
			// 最近接点と球の中心との長さで判定
			auto nearToSphere = spherePos - nearPos;
			if (nearToSphere.SqLength() < radius * radius)
			{
				info.isHit = true;
				info.hitPos = nearPos;
			}
		}
		//立方体は球以外と判定しません
	}
	// kindA=ポリゴン集合体の当たり判定
	if (kindA == ColliderBase::Kind::Polygons && kindB == ColliderBase::Kind::Sphere)
	{
		auto polygonA = dynamic_pointer_cast<ColliderPolygonModel>(colliderA->col);
		auto sphereB = dynamic_pointer_cast<ColliderSphere>(colliderB->col);
		auto spherePos = rigidB->GetNextPos() + colliderB->col->GetShift();
		float sphereRadius = sphereB->radius;

		Vec3 closestHitPos;   // 球から最も近いポリゴンの最近接点
		Vec3 closestNormal;   // 最も近いポリゴンの法線
		float closestDistance = 500; // 最小距離(はじめは馬鹿でか距離を入れておいて最小を更新していく)
		bool isCollision = false; // 衝突フラグ
		bool isInside = false;    // 球がポリゴン内部かどうか

		// ポリゴンとの衝突チェック
		for (auto& triangle : polygonA->GetTriangles())
		{
			// 球から三角形への最寄り点を求める
			Vec3 nearPoint = GetClosestPointOnTriangle(spherePos, triangle.vertex[0], triangle.vertex[1], triangle.vertex[2]);
			float distance = (nearPoint - spherePos).Length();

			// 最も近い点を更新
			if (distance < closestDistance)
			{
				closestDistance = distance;
				closestHitPos = nearPoint;
				closestNormal = triangle.Normal();
			}

			// 球と三角形の最近接点の距離が球の半径以下なら衝突
			if (distance <= sphereRadius)
			{
				isCollision = true;
			}
		}

		//レイキャスティング法
		// 球がポリゴン内部にあるか判定（レイの交差回数をカウント）
		if (!isCollision)
		{
			int intersectionCount = 0;
			Vec3 rayDir = Vec3(1, 0, 0); // 任意の方向のレイ（X軸正方向）

			for (auto& triangle : polygonA->GetTriangles())
			{
				if (RayIntersectsTriangle(spherePos, rayDir, triangle.vertex[0], triangle.vertex[1], triangle.vertex[2]))
				{
					intersectionCount++;
				}
			}

			// 交差回数が奇数なら内部と判断
			isInside = (intersectionCount % 2 == 1);
		}

		// 衝突結果を設定
		info.isHit = isCollision || isInside;  // 衝突または内部判定が真なら衝突
		info.hitPos = closestHitPos;  // 常に最も近い点を格納
		DrawSphere3D(closestHitPos.VGet(), 7, 7, 0xff000, 0xffffff, false);
		info.Norm = closestNormal; //最近接点からsphereへの向き
	}
	if (kindA == ColliderBase::Kind::Line && kindB == ColliderBase::Kind::Sphere)
	{
		auto LineA = dynamic_pointer_cast<ColliderLine3D>(colliderA->col);
		auto sphereB = dynamic_pointer_cast<ColliderSphere>(colliderB->col);
		// 球の中心位置
		auto spherePos = rigidB->GetNextPos() + colliderB->col->GetShift();
		float sphereRadius = sphereB->radius;

		// 線分の始点と終点
		auto line = LineA->GetLine();
		Vec3 lineStart = line.start;
		Vec3 lineEnd = line.end;

		// 線分の方向ベクトルと長さ
		Vec3 lineDir = lineEnd - lineStart;
		float lineLength = lineDir.Length();
		lineDir = lineDir.GetNormalized();

		// 球の中心から線分の始点へのベクトル
		Vec3 L = spherePos - lineStart;

		// 線分上の最短点を求める（t: 線分上の割合 [0, 1]）
		float t = Dot(L, lineDir) / lineLength;
		t = std::clamp(t, 0.0f, 1.0f); // t を [0, 1] に制限

		// 線分上の最短点
		Vec3 closestPoint = lineStart + lineDir * (t * lineLength);

#ifdef DEBUG
		// デバッグ用に最短点を描画
		DrawSphere3D(closestPoint.VGet(), 5, 5, 0xff0000, 0xff0000, false);

#endif
		// 球の中心からその最短点までの距離
		float distance = (closestPoint - spherePos).Length();

		// 距離が球の半径以下なら交差している
		if (distance <= sphereRadius + 1)
		{
			info.hitPos = closestPoint;
			info.isHit = true;
		}
	}

	//kindA=ポリゴンの当たり判定
	return info;
}


void MyEngine::Physics::FixNextPos(const std::shared_ptr<Rigidbody> primaryRigid, std::shared_ptr<Rigidbody> secondaryRigid, const std::shared_ptr<Collidable::CollideInfo>& primaryCollider, const std::shared_ptr<Collidable::CollideInfo>& secondaryCollider, const CollideHitInfo info)
{
	Vec3 fixedPos = secondaryRigid->GetNextPos();

	auto primaryKind = primaryCollider->col->GetKind();
	auto secondaryKind = secondaryCollider->col->GetKind();

	//primaryKind=球の位置補正
	if (primaryKind == ColliderBase::Kind::Sphere)
	{
		if (secondaryKind == ColliderBase::Kind::Sphere)
		{
			auto primarySphere = dynamic_pointer_cast<ColliderSphere>(primaryCollider->col);
			auto secondarySphere = dynamic_pointer_cast<ColliderSphere>(secondaryCollider->col);

			// primaryからsecondaryへのベクトルを作成
			auto primaryToSecondary = (secondaryRigid->GetNextPos() + secondaryCollider->col->GetShift()) -
				(primaryRigid->GetNextPos() + primaryCollider->col->GetShift());
			// そのままだとちょうど当たる位置になるので少し余分に離す
			float  awayDist = (primarySphere->radius + secondarySphere->radius + 0.0001f);
			// 長さを調整
			primaryToSecondary = primaryToSecondary.GetNormalized() * awayDist;
			// primaryからベクトル方向に押す
			fixedPos = (primaryRigid->GetNextPos() - primaryCollider->col->GetShift()) + primaryToSecondary - secondaryCollider->col->GetShift();
		}
		if (secondaryKind == ColliderBase::Kind::Box)
		{
			auto dir = (primaryRigid->GetNextPos() - primaryCollider->col->GetShift() - secondaryCollider->col->GetShift()) - info.hitPos;
			dir.Normalize();
			auto sphereCol = dynamic_pointer_cast<ColliderSphere>(primaryCollider->col);
			DrawSphere3D(info.hitPos.VGet(), 6, 8, 0xffffff, 0xffffff, false);
			fixedPos = info.hitPos + dir * (sphereCol->radius + 0.0001f);
		}
		if (secondaryKind == ColliderBase::Kind::Polygons)
		{
			//今回のプロジェクトではポリゴンの当たり判定を持っているオブジェクトは動かさないので割愛
			assert(0 && "ポリゴンモデルが衝突相手として入っています");
		}
	}
	//primaryKind=立方体の位置補正

	if (primaryKind == ColliderBase::Kind::Box)
	{
		if (secondaryKind == ColliderBase::Kind::Sphere)
		{
			auto dir = (secondaryRigid->GetNextPos() + secondaryCollider->col->GetShift() - primaryCollider->col->GetShift()) - info.hitPos;
			dir.Normalize();
			auto sphereCol = dynamic_pointer_cast<ColliderSphere>(secondaryCollider->col);
			DrawSphere3D(info.hitPos.VGet(), 6, 8, 0xffffff, 0xffffff, false);
			fixedPos = info.hitPos + dir * (sphereCol->radius + 0.0001f);
		}
	}
	if (primaryKind == ColliderBase::Kind::Polygons) {
		if (secondaryKind == ColliderBase::Kind::Sphere) {

			auto secondarySphere = dynamic_pointer_cast<ColliderSphere>(secondaryCollider->col);
			auto primaryPolygon = dynamic_pointer_cast<ColliderPolygonModel>(primaryCollider->col);

			// 球の中心位置を計算
			auto spherePos = secondaryRigid->GetPos() + secondaryCollider->col->GetShift();

			// 衝突点と球の中心の距離（侵入距離）
			float penetrationDepth = secondarySphere->radius - Dot((spherePos - info.hitPos), info.Norm);

			// 侵入している場合のみ補正を適用
			if (penetrationDepth < 0.0f) {
				// 法線方向が球の中心に向かっているか確認（逆方向の場合は反転）
				bool Reverce = false;
				if (Dot(info.Norm, spherePos - info.hitPos) < 0.0f) {
					Reverce = true;
				}

				float adjustedPenetrationDepth = penetrationDepth;

				// 法線方向に補正距離分だけ移動
				if (Reverce)
				{
					fixedPos = info.hitPos + (info.Norm * -1) * (secondarySphere->GetRadius() + 0.001f) - secondaryCollider->col->GetShift();
				}
				else
				{
					fixedPos = info.hitPos + info.Norm * (secondarySphere->GetRadius() + 0.001f) - secondaryCollider->col->GetShift();
				}

			}
			else {
				// 侵入していない場合は補正不要
				fixedPos = spherePos;
			}
		}
	}

	secondaryRigid->SetNextPos(fixedPos);
}

void MyEngine::Physics::AddNewCollideInfo(const std::weak_ptr<Collidable>& objA, const std::weak_ptr<Collidable>& objB, ColideTag ownTag, ColideTag sendTag, SendCollideInfo& info, const Vec3& hitPos)
{
	// 既に追加されている通知リストにあれば追加しない
	for (auto& i : info)
	{
		if (i.own.lock() == objA.lock() && i.own.lock() == objB.lock()) return;
		if (i.send.lock() == objB.lock() && i.send.lock() == objA.lock()) return;
	}

	// ここまで来たらまだ通知リストに追加されていないため追加
	info.emplace_back(SendInfo{ objA, objB, ownTag, sendTag, hitPos });
}

void MyEngine::Physics::CheckSendOnCollideInfo(SendCollideInfo& preSendInfo, SendCollideInfo& newSendInfo, bool isTrigger)
{
	// 1つ前に通知リストが当たったか
	auto isPreExist = preSendInfo.size() != 0;

	for (auto& info : newSendInfo)
	{
		bool isEntry = false;

		// 1つ前に通知リストがあった場合
		if (isPreExist)
		{
			bool isEnter = true;
			// 1つ前の通知リストをすべて回す
			auto it = preSendInfo.begin();
			for (; it != preSendInfo.end(); ++it)
			{
				// 通知リストが存在した場合は当たった時の通知を呼ばないようにする
				if (it->own.lock() == info.own.lock() && it->send.lock() == info.send.lock()) isEnter = false;
				if (it->own.lock() == info.send.lock() && it->send.lock() == info.own.lock()) isEnter = false;
				if (!isEnter) break;
			}
			if (isEnter)
			{
				// 当たった時の通知を追加
				if (isTrigger) AddOnCollideInfo(info, OnCollideInfoKind::TriggerEnter);
				else           AddOnCollideInfo(info, OnCollideInfoKind::CollideEnter);
			}
			else
			{
				// 一つ前の通知リストから今回存在する通知のものを削除する
				preSendInfo.erase(it);
			}
		}
		// 1つ前に通知リストがなかった場合
		else
		{
			// 当たった時の通知を追加
			if (isTrigger) AddOnCollideInfo(info, OnCollideInfoKind::TriggerEnter);
			else           AddOnCollideInfo(info, OnCollideInfoKind::CollideEnter);
		}

		// 当たっている状態の通知を追加
		if (isTrigger) AddOnCollideInfo(info, OnCollideInfoKind::TriggerStay);
		else           AddOnCollideInfo(info, OnCollideInfoKind::CollideStay);
	}

	// 上で削除されずに残った1つ前の通知リストは今回抜けているため
	// 離れた時の通知を追加
	for (auto& info : preSendInfo)
	{
		if (isTrigger) AddOnCollideInfo(info, OnCollideInfoKind::TriggerExit);
		else           AddOnCollideInfo(info, OnCollideInfoKind::CollideExit);
	}
}
void Physics::AddOnCollideInfo(const SendInfo& info, OnCollideInfoKind kind)
{
	if (info.send.lock() == nullptr)
	{
		return;
	}
	m_onCollideInfo.emplace_back(OnCollideInfoData{ info.own, info.send, info.ownTag,info.sendTag, info.hitPos, kind });
	m_onCollideInfo.emplace_back(OnCollideInfoData{ info.send, info.own, info.sendTag,info.ownTag, info.hitPos, kind });
}

void MyEngine::Physics::OnCollideInfo(const std::weak_ptr<Collidable>& own, const std::weak_ptr<Collidable>& send, ColideTag ownTag, ColideTag sendTag, const Vec3& hitPos, OnCollideInfoKind kind)
{
	auto item = send;

	if (own.expired() || send.expired())return;

	if (kind == OnCollideInfoKind::CollideEnter)
	{
		own.lock()->OnCollideEnter(item.lock(), ownTag, sendTag);
	}
	else if (kind == OnCollideInfoKind::CollideStay)
	{
		own.lock()->OnCollideStay(item.lock(), ownTag, sendTag);
	}
	else if (kind == OnCollideInfoKind::CollideExit)
	{
		own.lock()->OnCollideExit(item.lock(), ownTag, sendTag);
	}
	else if (kind == OnCollideInfoKind::TriggerEnter)
	{
		own.lock()->OnTriggerEnter(item.lock(), ownTag, sendTag);
	}
	else if (kind == OnCollideInfoKind::TriggerStay)
	{
		own.lock()->OnTriggerStay(item.lock(), ownTag, sendTag);
	}
	else if (kind == OnCollideInfoKind::TriggerExit)
	{
		own.lock()->OnTriggerExit(item.lock(), ownTag, sendTag);
	}
}

/// <summary>
/// 最終的な未来の座標から現在の座標に適用
/// </summary>
void Physics::FixPos() const
{
	for (const auto& item : m_collidables)
	{
		auto& rigid = item->m_rigid;

		rigid->SetPos(rigid->GetNextPos());

#ifdef DEBUG
		auto& debug = DebugDraw::GetInstance();
		for (const auto& collider : item->m_colliders)
		{
			auto kind = collider->col->GetKind();
			if (kind == ColliderBase::Kind::Sphere)
			{
				auto sphereData = dynamic_pointer_cast<MyEngine::ColliderSphere>(collider->col);
				DebugDraw::SphereInfo info;
				info.center = rigid->GetPos();
				info.radius = sphereData->radius;
				info.color = DebugDraw::COL_AFFTER;
				debug.DrawSphere(info);
			}
		}
#endif
	}

}

bool MyEngine::Physics::IsPointOnEdge(const Vec3& point, float radius, const Vec3& v0, const Vec3& v1) const
{
	Vec3 edge = v1 - v0;
	Vec3 pointToV0 = point - v0;
	float edgeLength = edge.Length();
	float projection = Dot(pointToV0, edge) / edgeLength;

	if (projection < 0.0f || projection > edgeLength) {
		return false;  // 衝突点が辺の延長線上にない場合
	}

	Vec3 closestPoint = v0 + edge * (projection / edgeLength);
	float distance = (point - closestPoint).Length();

	return distance <= radius;  // 距離が球の半径以下なら衝突
}

Vec3 closestPointOnCube(const Vec3& cubeCenter, const Vec3& size, const Vec3& point) {
	Vec3 closest;

	//クランプ...範囲内に収める(a,min,max)
	closest.x = std::clamp(point.x, cubeCenter.x - size.x, cubeCenter.x + size.x);
	closest.y = std::clamp(point.y, cubeCenter.y - size.y, cubeCenter.y + size.y);
	closest.z = std::clamp(point.z, cubeCenter.z - size.z, cubeCenter.z + size.z);

	return closest;
}

Vec3 closestPointOnCubeAndSphere(const Vec3& cubeCenter, Vec3 size, const Vec3& sphereCenter, float sphereRadius) {
	// 立方体の最近接点
	Vec3 closestPoint = closestPointOnCube(cubeCenter, size, sphereCenter);

	// 球の中心から最近接点までのベクトル
	Vec3 direction = closestPoint - sphereCenter;

	// ベクトルの長さ
	float length = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

	// ベクトルを正規化
	if (length > 0) {
		direction = direction * (sphereRadius / length);
	}

	// 最近接点を計算
	return sphereCenter + direction;
}

