#include "Effect.h"

Effekseer::Matrix43 Effect::GetEffMatrix(DxLib::MATRIX mat)
{
	Effekseer::Matrix43 res;
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 3; ++x)
		{
			res.Value[y][x] = mat.m[y][x];
		}
	}
	return res;
}
