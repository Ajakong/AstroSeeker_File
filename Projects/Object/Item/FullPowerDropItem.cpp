#include "FullPowerDropItem.h"
#include"Quaternion.h"

FullPowerDropItem::FullPowerDropItem(Vec3 pos, bool antiGravity): Item(pos,ObjectTag::FullPowerDropItem,antiGravity)
{
}

FullPowerDropItem::~FullPowerDropItem()
{
}

void FullPowerDropItem::Draw()
{
	Vec3 zero = { 0,0,0 };
	m_rigid->SetVelocity(Vec3(0, 0, 0));
	Vec3 offSetVec = GetCameraRightVector();
	offSetVec -= GetCameraUpVector();
	offSetVec *= 0.9f;
	Quaternion myQ;
	m_angle += 0.05f;

	Vec3 front = GetCameraFrontVector();
	for (int i = 0; i < 3; i++)
	{
		myQ.SetQuaternion(offSetVec);
		myQ.SetMove(DX_PI_F * 2 / 3 * i + m_angle, front);
		Vec3 offSet = myQ.Move(offSetVec, zero);
		DrawSphere3D((m_rigid->GetPos() + offSet).VGet(), 1.0, 8, 0x0000ff, 0x0000ff, false);
	}

	DrawSphere3D(m_rigid->GetPos().VGet(), m_col->radius, 8, 0xaa0000, 0xff0000, false);
}
