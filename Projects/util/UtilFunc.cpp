#include "UtilFunc.h"
#include"DxLib.h"
#include<cmath>
void UtilFunc::DrawTriangle(const Vec3& center, const Vec3& normal, float size, unsigned int color, int GrHandle)
{
    SetUseBackCulling(DX_CULLING_NONE);
    // 法線ベクトルを基に、平面の基準となるベクトルを作成
    Vec3 up(0, 1, 0);
    if (abs(Dot(normal, up)) > 0.99f) {
        up = Vec3(1, 0, 0);
    }
    Vec3 right = Cross(normal, up).GetNormalized();
    Vec3 forward = Cross(right, normal).GetNormalized();

    // 平面の4つの頂点を計算
    Vec3 p1 = center + right * size - forward * size;
    Vec3 p2 = center - right * size - forward * size;
    Vec3 p3 = center - right * size + forward * size;
    Vec3 p4 = center + right * size + forward * size;

    // 頂点データの配列
    VERTEX3D vertices[4] = {
        { VGet(p1.x, p1.y, p1.z), VGet(normal.x, normal.y, normal.z), color },
        { VGet(p2.x, p2.y, p2.z), VGet(normal.x, normal.y, normal.z), color },
        { VGet(p3.x, p3.y, p3.z), VGet(normal.x, normal.y, normal.z), color },
        { VGet(p4.x, p4.y, p4.z), VGet(normal.x, normal.y, normal.z), color }
    };

    // 四角形の描画
    DrawPolygon3D(vertices, 1, GrHandle, false);
}

void UtilFunc::DrawCircle(const Vec3& center, const Vec3& normal, float size, unsigned int color, int GrHandle, int vertexCount)
{
    // 法線ベクトルを基に、平面の基準となるベクトルを作成
    Vec3 up(0, 1, 0);
    if (abs(Dot(normal, up)) > 0.99f) {
        up = Vec3(1, 0, 0);
    }
    Vec3 right = Cross(normal, up).GetNormalized();
    Vec3 forward = Cross(right, normal).GetNormalized();

    // 中心点を中心にした円を描画
    // 頂点を計算して、三角形の各面を描画
    for (int i = 0; i < vertexCount; ++i) {
        float angle1 = (2.0f * 3.14159f * i) / vertexCount;  // 角度計算
        float angle2 = (2.0f * 3.14159f * (i + 1)) / vertexCount;  // 次の頂点

        // 現在の頂点と次の頂点の位置を計算
        Vec3 p1 = center + (right * cos(angle1) + forward * sin(angle1)) * size;
        Vec3 p2 = center + (right * cos(angle2) + forward * sin(angle2)) * size;

        // 中心からの三角形を描画
        DrawTriangle(center, normal,size, color, GrHandle);
    }
}

