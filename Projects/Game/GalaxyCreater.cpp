#include"Camera.h"

#include "GalaxyCreater.h"
#include"Player.h"
#include"SpherePlanet.h"
#include"SeekerLine.h"
#include"Physics.h"
#include"FullPowerDropItem.h"
#include"StickStarItem.h"
#include"Coin.h"
#include"FragmentOfStar.h"

#include"ModelManager.h"
#include"GraphManager.h"
#include"SoundManager.h"

#include"Takobo.h"
#include"Kuribo.h"
#include"BigKuribo.h"
#include"Gorori.h"
#include"Booster.h"
#include"Boss.h"
#include"DekaHead_Red.h"
#include"DekaHead_Blue.h"
#include"DekaHead_Green.h"
#include"DekaHead_Yellow.h"
#include"DekaHead_White.h"

#include<cassert>

namespace
{
	const char* kObjectAppearSEHandle = "ObjectAppear.mp3";
}

GalaxyCreater::GalaxyCreater()
{

}

GalaxyCreater::~GalaxyCreater()
{
}

GalaxyCreater& GalaxyCreater::GetInstance()
{
	static GalaxyCreater info;
	return info;
}

void GalaxyCreater::ObjectCreate(std::shared_ptr<Player> player)
{
	m_objectData.clear();

	std::string fileName = "Data/Info/data.loc";
	//開くファイルのハンドルを取得
	int handle = FileRead_open(fileName.c_str());

	//読み込むオブジェクト数が何個あるか取得
	int dataCnt = 0;
	FileRead_read(&dataCnt, sizeof(dataCnt), handle);
	//読み込むオブジェクト数分の配列に変更する
	m_objectData.resize(dataCnt);

	//配列の数分回す
	for (auto& loc : m_objectData)
	{
		//名前のバイト数を取得する
		byte nameCnt = 0;
		FileRead_read(&nameCnt, sizeof(nameCnt), handle);
		//名前のサイズを変更する
		loc.name.resize(nameCnt);
		//名前を取得する
		FileRead_read(loc.name.data(), sizeof(char) * static_cast<int>(loc.name.size()), handle);

		//タグのバイト数を取得する
		byte tagCnt = 0;
		FileRead_read(&tagCnt, sizeof(tagCnt), handle);
		//タグのサイズを変更する
		loc.tag.resize(tagCnt);
		//タグを取得する
		FileRead_read(loc.tag.data(), sizeof(char) * static_cast<int>(loc.tag.size()), handle);

		//座標を取得する
		FileRead_read(&loc.pos, sizeof(loc.pos), handle);
		//回転を取得する
		FileRead_read(&loc.rot, sizeof(loc.rot), handle);
		//大きさを取得する
		FileRead_read(&loc.scale, sizeof(loc.scale), handle);

		if (loc.tag == "Player")
		{
			player->GetRigidbody()->SetPos(loc.pos);
		}
		else if (loc.tag == "Coin")
		{
			auto coin = std::make_shared<Coin>(loc.pos,true);
			MyEngine::Physics::GetInstance().Entry(coin);
		}
		else if (loc.tag == "FullPowerDropItem")
		{
			auto fullPowerDropItem = std::make_shared<FullPowerDropItem>(loc.pos, true);
			MyEngine::Physics::GetInstance().Entry(fullPowerDropItem);
		}
		else if (loc.tag == "StickStarItem")
		{
			auto stickStarItem = std::make_shared<StickStarItem>(loc.pos, true);
			MyEngine::Physics::GetInstance().Entry(stickStarItem);
		}
		else if (loc.tag == "StarShard")
		{
			auto starShard = std::make_shared<FragmentOfStar>(loc.pos, true);
			MyEngine::Physics::GetInstance().Entry(starShard);
		}

		
	}
	FileRead_close(handle);

	for (auto& item : m_objectData)
	{
		printf(("Name:" + item.name + " Tag:" + item.tag + "\n").c_str());
		printf("Pos:%f,%f,%f\n", item.pos.x, item.pos.y, item.pos.z);
		printf("Rot:%f,%f,%f\n", item.rot.x, item.rot.y, item.rot.z);
		printf("scale:%f,%f,%f\n", item.scale.x, item.scale.y, item.scale.z);
	}
	m_createObjectSEHandle=SoundManager::GetInstance().GetSoundData(kObjectAppearSEHandle);
}

void GalaxyCreater::SeekerLineCreate()
{
	std::string fileName = "Data/Info/SeekerLine.loc";
	//開くファイルのハンドルを取得
	int handle = FileRead_open(fileName.c_str());


	//読み込むオブジェクト数が何個あるか取得
	int dataCnt = 0;
	FileRead_read(&dataCnt, sizeof(dataCnt), handle);
	LocationSeekerLine info;
	for (int i = 0; i < dataCnt; i++)
	{
		//名前のバイト数を取得する
		byte nameCnt = 0;
		FileRead_read(&nameCnt, sizeof(nameCnt), handle);
		//名前のサイズを変更する
		info.name.resize(nameCnt);
		//名前を取得する
		FileRead_read(info.name.data(), sizeof(char) * static_cast<int>(info.name.size()), handle);

		Vec3 pos;
		FileRead_read(&pos, sizeof(pos), handle);
		info.points.push_back(pos);

	}
	auto seekerLine = std::make_shared<SeekerLine>(info.points, info.color);
	MyEngine::Physics::GetInstance().Entry(seekerLine);
	FileRead_close(handle);

}

void GalaxyCreater::PlanetCreate()
{
	m_planetData.clear();
	m_planetModelData.clear();

	std::string fileName = "Data/Info/Planet.loc";
	//開くファイルのハンドルを取得
	int handle = FileRead_open(fileName.c_str());

	//読み込むオブジェクト数が何個あるか取得
	int dataCnt = 0;
	FileRead_read(&dataCnt, sizeof(dataCnt), handle);
	//読み込むオブジェクト数分の配列に変更する
	m_planetData.resize(dataCnt);
	
	//配列の数分回す
	for (auto& loc : m_planetData)
	{
		//名前のバイト数を取得する
		byte nameCnt = 0;
		FileRead_read(&nameCnt, sizeof(nameCnt), handle);
		//名前のサイズを変更する
		loc.name.resize(nameCnt);
		//名前を取得する
		FileRead_read(loc.name.data(), sizeof(char) * static_cast<int>(loc.name.size()), handle);
		//座標を取得する
		FileRead_read(&loc.pos, sizeof(loc.pos), handle);
		//色を取得する
		FileRead_read(&loc.color, sizeof(loc.color), handle);
		//重力を取得する
		FileRead_read(&loc.gravityPower, sizeof(loc.gravityPower), handle);
		//モデル名のバイト数を取得
		byte modelNameCnt = 0;
		FileRead_read(&modelNameCnt, sizeof(modelNameCnt), handle);
		loc.modelName.resize(modelNameCnt);
		//モデル名を取得する
		//取得したモデル名を利用し、後々ハンドルを受け取る
		FileRead_read(loc.modelName.data(), sizeof(char) * static_cast<int>(loc.modelName.size()), handle);
		//摩擦力を取得する
		FileRead_read(&loc.coefficientOfFriction, sizeof(loc.coefficientOfFriction), handle);
		//大きさを取得する
		FileRead_read(&loc.scale, sizeof(loc.scale), handle);


		m_planetModelData.push_back(ModelManager::GetInstance().GetModelData(loc.modelName.c_str()));
		auto planet = std::make_shared<SpherePlanet>(loc.pos, loc.color, loc.gravityPower, m_planetModelData.back(), loc.coefficientOfFriction, loc.scale);
		MyEngine::Physics::GetInstance().Entry(planet);
	}
	FileRead_close(handle);

}


void GalaxyCreater::TalkObjectCreate()
{
	std::list<TalkObject> objects;
	std::string fileName = "Data/Info/TalkObject.loc";
	//開くファイルのハンドルを取得
	int handle = FileRead_open(fileName.c_str());

	//読み込むオブジェクト数が何個あるか取得
	int dataCnt = 0;
	FileRead_read(&dataCnt, sizeof(dataCnt), handle);
	//読み込むオブジェクト数分の配列に変更する
	objects.resize(dataCnt);
	
	
	//配列の数分回す
	for (auto& loc : objects)
	{
		//名前のバイト数を取得する
		byte nameCnt = 0;
		FileRead_read(&nameCnt, sizeof(nameCnt), handle);
		//名前のサイズを変更する
		loc.name.resize(nameCnt);
		//名前を取得する
		FileRead_read(loc.name.data(), sizeof(char) * static_cast<int>(loc.name.size()), handle);
		//タグのバイト数を取得する
		byte tagCnt = 0;
		FileRead_read(&tagCnt, sizeof(tagCnt), handle);
		//タグのサイズを変更する
		loc.tag.resize(tagCnt);
		//タグを取得する
		FileRead_read(loc.tag.data(), sizeof(char) * static_cast<int>(loc.tag.size()), handle);
		//座標を取得する
		FileRead_read(&loc.pos, sizeof(loc.pos), handle);
		
		if (loc.tag == "DekaHead_Red")
		{
			auto talkObject=std::make_shared<DekaHead_Red>(loc.pos);
			MyEngine::Physics::GetInstance().Entry(talkObject);
		}
		if (loc.tag == "DekaHead_Blue")
		{
			auto talkObject = std::make_shared<DekaHead_Blue>(loc.pos);
			MyEngine::Physics::GetInstance().Entry(talkObject);
		}
		if (loc.tag == "DekaHead_Green")
		{
			auto talkObject = std::make_shared<DekaHead_Green>(loc.pos);
			MyEngine::Physics::GetInstance().Entry(talkObject);
		}
		if (loc.tag == "DekaHead_Yellow")
		{
			auto talkObject = std::make_shared<DekaHead_Yellow>(loc.pos);
			MyEngine::Physics::GetInstance().Entry(talkObject);
		}
		if (loc.tag == "DekaHead_White")
		{
			auto talkObject = std::make_shared<DekaHead_White>(loc.pos);
			MyEngine::Physics::GetInstance().Entry(talkObject);
		}
	}
	FileRead_close(handle);

}

std::vector<std::shared_ptr<Enemy>> GalaxyCreater::EnemyCreate(std::shared_ptr<Player>player)
{
	m_enemyData.clear();

	std::string fileName = "Data/Info/Enemy.loc";
	//開くファイルのハンドルを取得
	int handle = FileRead_open(fileName.c_str());

	//読み込むオブジェクト数が何個あるか取得
	int dataCnt = 0;
	FileRead_read(&dataCnt, sizeof(dataCnt), handle);
	//読み込むオブジェクト数分の配列に変更する
	m_enemyData.resize(dataCnt);
	std::vector<std::shared_ptr<Enemy>> enemies;
	//配列の数分回す
	for (auto& loc : m_enemyData)
	{
		//名前のバイト数を取得する
		byte nameCnt = 0;
		FileRead_read(&nameCnt, sizeof(nameCnt), handle);
		//名前のサイズを変更する
		loc.name.resize(nameCnt);
		//名前を取得する
		FileRead_read(loc.name.data(), sizeof(char) * static_cast<int>(loc.name.size()), handle);

		//タグのバイト数を取得する
		byte tagCnt = 0;
		FileRead_read(&tagCnt, sizeof(tagCnt), handle);
		//タグのサイズを変更する
		loc.tag.resize(tagCnt);
		//タグを取得する
		FileRead_read(loc.tag.data(), sizeof(char) * static_cast<int>(loc.tag.size()), handle);

		//座標を取得する
		FileRead_read(&loc.pos, sizeof(loc.pos), handle);

		if (loc.tag == "Takobo")
		{
			enemies.push_back(std::make_shared<Takobo>(loc.pos, player));
		}
		else if (loc.tag == "Kuribo")
		{
			enemies.push_back(std::make_shared<Kuribo>(loc.pos));
		}
		
		else if (loc.tag == "Boss")
		{
			enemies.push_back(std::make_shared<Boss>(loc.pos,player));
		}
		else if (loc.tag == "Gorori")
		{
			enemies.push_back(std::make_shared<Gorori>(loc.pos, player));
		}
		MyEngine::Physics::GetInstance().Entry(enemies.back());
	}
	FileRead_close(handle);
	return enemies;
}

std::vector<std::shared_ptr<MyEngine::Collidable>> GalaxyCreater::LockedObjectCreate()
{
	m_lockedObjects.clear();

	std::string fileName = "Data/Info/LockedObject.loc";
	//開くファイルのハンドルを取得
	int handle = FileRead_open(fileName.c_str());

	//読み込むオブジェクト数が何個あるか取得
	int dataCnt = 0;
	FileRead_read(&dataCnt, sizeof(dataCnt), handle);
	//読み込むオブジェクト数分の配列に変更する
	m_lockedData.resize(dataCnt);
	//配列の数分回す
	for (auto& loc : m_lockedData)
	{
		//名前のバイト数を取得する
		byte nameCnt = 0;
		FileRead_read(&nameCnt, sizeof(nameCnt), handle);
		//名前のサイズを変更する
		loc.name.resize(nameCnt);
		//名前を取得する
		FileRead_read(loc.name.data(), sizeof(char) * static_cast<int>(loc.name.size()), handle);

		//タグのバイト数を取得する
		byte tagCnt = 0;
		FileRead_read(&tagCnt, sizeof(tagCnt), handle);
		//タグのサイズを変更する
		loc.tag.resize(tagCnt);
		//タグを取得する
		FileRead_read(loc.tag.data(), sizeof(char) * static_cast<int>(loc.tag.size()), handle);

		//座標を取得する
		FileRead_read(&loc.pos, sizeof(loc.pos), handle);
		if (loc.tag == "Booster")
		{
			Vec3 dir;
			//オブジェクトを飛ばす向き
			FileRead_read(&dir, sizeof(dir), handle);

			std::string modelName;

			//モデル名のバイト数を取得する
			byte modelNameCnt = 0;
			FileRead_read(&modelNameCnt, sizeof(modelNameCnt), handle);
			//モデル名のサイズを変更する
			modelName.resize(modelNameCnt);
			//モデル名を取得する
			FileRead_read(modelName.data(), sizeof(char) * static_cast<int>(modelName.size()), handle);
			float power;
			//飛ばす力
			FileRead_read(&power, sizeof(power), handle);
			m_lockedObjects.push_back(std::make_shared<Booster>(loc.pos, dir, -1,power));
			
		}
		if (loc.tag == "SeekerLine")
		{
			//読み込むオブジェクト数が何個あるか取得
			int dataCnt = 0;
			FileRead_read(&dataCnt, sizeof(dataCnt), handle);
			LocationSeekerLine info;
			for (int i = 0; i < dataCnt; i++)
			{
				//名前のバイト数を取得する
				byte nameCnt = 0;
				FileRead_read(&nameCnt, sizeof(nameCnt), handle);
				//名前のサイズを変更する
				info.name.resize(nameCnt);
				//名前を取得する
				FileRead_read(info.name.data(), sizeof(char) * static_cast<int>(info.name.size()), handle);

				//位置を設定
				Vec3 pos;
				FileRead_read(&pos, sizeof(pos), handle);
				info.points.push_back(pos);

			}
			m_lockedObjects.push_back(std::make_shared<SeekerLine>(info.points, info.color));
			
		}
		MyEngine::Physics::GetInstance().Entry(m_lockedObjects.back());
	}

	FileRead_close(handle);
	return m_lockedObjects;
}

std::vector<std::shared_ptr<Enemy>> GalaxyCreater::KeyLockObjectCreate()
{
	m_keyLockObjectData.clear();

	std::string fileName = "Data/Info/KeyLockedObjects.loc";
	//開くファイルのハンドルを取得
	int handle = FileRead_open(fileName.c_str());

	//読み込むオブジェクト数が何個あるか取得
	int dataCnt = 0;
	FileRead_read(&dataCnt, sizeof(dataCnt), handle);
	//読み込むオブジェクト数分の配列に変更する
	m_keyLockObjectData.resize(dataCnt);

	std::vector<std::shared_ptr<Enemy>> enemies;
	//配列の数分回す
	for (auto& loc : m_keyLockObjectData)
	{
		//名前のバイト数を取得する
		byte nameCnt = 0;
		FileRead_read(&nameCnt, sizeof(nameCnt), handle);
		//名前のサイズを変更する
		loc.name.resize(nameCnt);
		//名前を取得する
		FileRead_read(loc.name.data(), sizeof(char) * static_cast<int>(loc.name.size()), handle);

		//タグのバイト数を取得する
		byte tagCnt = 0;
		FileRead_read(&tagCnt, sizeof(tagCnt), handle);
		//タグのサイズを変更する
		loc.tag.resize(tagCnt);
		//タグを取得する
		FileRead_read(loc.tag.data(), sizeof(char) * static_cast<int>(loc.tag.size()), handle);

		//座標を取得する
		FileRead_read(&loc.pos, sizeof(loc.pos), handle);
		
		//つながってるオブジェクトの番号を設定
		FileRead_read(&loc.connectObjectNumber, sizeof(loc.connectObjectNumber), handle);

		if (loc.tag == "BigKuribo")
		{
			enemies.push_back(std::make_shared<BigKuribo>(loc.pos,loc.connectObjectNumber));
			MyEngine::Physics::GetInstance().Entry(enemies.back());
		}
		
	}

	FileRead_close(handle);
	return enemies;

}

void GalaxyCreater::Clear()
{
	m_objectData.clear();
	m_enemyData.clear();
	m_lockedData.clear();
	m_lockedObjects.clear();
	m_planetData.clear();
}

std::shared_ptr<MyEngine::Collidable> GalaxyCreater::GetCollidable(int connectNumber)
{
	PlaySoundMem(m_createObjectSEHandle, DX_PLAYTYPE_BACK);
	auto obj = m_lockedObjects[connectNumber];
	obj->SetIsActive(true);
	MyEngine::Physics::GetInstance().Initialize(obj);
	Vec3 objPos = obj->GetRigidbody()->GetPos();
	Vec3 cameraPos = m_camera->GetPlayerCameraPoint();
	
	Vec3 cameraToObj = cameraPos - objPos;

	Vec3 center = (objPos + cameraPos) / 2;

	

	// **カメラをオブジェクトとの中間点に配置**
	Vec3 newCameraPos = (cameraPos + objPos) * 0.5f;

	// **カメラの向きベクトル**
	Vec3 cameraToObjDir = (objPos - newCameraPos).GetNormalized();

	// **オブジェクトの上方向を取得**
	Vec3 objUp = obj->GetUpVec();

	// **カメラの「右方向ベクトル」を計算**
	Vec3 rightVec = Cross(objUp, cameraToObjDir).GetNormalized();

	// **カメラの「上方向ベクトル」を計算**
	Vec3 upVec = Cross(cameraToObjDir, rightVec).GetNormalized();

	// **カメラを設定**
	m_camera->WatchThis(center, center+ upVec*cameraToObj.Length(), rightVec, 60.f);
	return obj;
}
