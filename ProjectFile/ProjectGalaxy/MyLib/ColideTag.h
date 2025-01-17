#pragma once
#include<string>
enum class ColideTag
{
	Head,
	Body,
	Fist,
	Foot,
	Spin,
	one,//汎用的なタグ1
	two,//汎用的なタグ2
	three//汎用的なタグ3
};

std::string ColideTag_String(ColideTag tag);
