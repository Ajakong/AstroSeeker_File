#include "ColliderPolygonModel.h"
#include"DxLib.h"

MyEngine::ColliderPolygonModel::ColliderPolygonModel():ColliderBase(Kind::Polygons)
{
	
}

MyEngine::ColliderPolygonModel::~ColliderPolygonModel()
{

}

void MyEngine::ColliderPolygonModel::Init(int modelHandle, float scale)
{
    // 初期位置から動かないため、一度だけ情報を保存
    m_data.modelHandle = modelHandle;

    // モデル全体のサイズを取得
    MV1_REF_POLYGONLIST refPolyList = MV1GetReferenceMesh(modelHandle, -1, TRUE);

    // モデルの全ポリゴン数を取得
    int polygonNum = refPolyList.PolygonNum;

    // モデルのワールド行列を取得
    MATRIX worldMatrix = MV1GetMatrix(modelHandle);
    VECTOR worldPosition = { worldMatrix.m[3][0], worldMatrix.m[3][1], worldMatrix.m[3][2] }; // 平行移動ベクトル

    //ポリゴンの先頭アドレス
    auto refPoly = refPolyList.Polygons;
    for (int i = 0; i < polygonNum; i++, refPoly++)
    {
        VECTOR vertexPositions[3];

        // 各ポリゴンの頂点座標を取得
        vertexPositions[0] = refPolyList.Vertexs[refPoly->VIndex[0]].Position;
        vertexPositions[1] = refPolyList.Vertexs[refPoly->VIndex[1]].Position;
        vertexPositions[2] = refPolyList.Vertexs[refPoly->VIndex[2]].Position;

        // 三角形情報を保存
        Triangle triangle = 
        {
           {vertexPositions[0],
            vertexPositions[1],
            vertexPositions[2]}
        };

        m_triangles.push_back(triangle);
    }
}

void MyEngine::ColliderPolygonModel::DebugDraw()
{
#ifdef DEBUG
    for (const auto& triangle : m_triangles) {
        // 三角形の3辺を描画
        DrawLine3D(triangle.vertex[0].VGet(), triangle.vertex[1].VGet(),0xffff00); // 辺 v0 → v1
        DrawLine3D(triangle.vertex[1].VGet(), triangle.vertex[2].VGet(), 0xffff00); // 辺 v1 → v2
        DrawLine3D(triangle.vertex[2].VGet(), triangle.vertex[0].VGet(), 0xffff00); // 辺 v2 → v0

        Vec3 center = (triangle.vertex[0] + triangle.vertex[1] + triangle.vertex[2]) / 3;
        DrawLine3D(center.VGet(), (center+triangle.Normal()).VGet(), 0xff0000); // 辺 v2 → v0
    }
#endif
}
