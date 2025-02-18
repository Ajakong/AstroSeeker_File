#include "ColideTag.h"


std::string ColideTag_String(ColideTag tag)
{
	switch (tag)
	{
	case ColideTag::Head: return "Head";
		break;
	case ColideTag::Body:return "Body";
		break;
	case ColideTag::Fist:return "Fist";
		break;
	case ColideTag::Foot:return "Foot";
		break;
	case ColideTag::Spin:return "Spin";
		break;
	case ColideTag::one:return "one";
		break;
	case ColideTag::two:return "two";
		break;
	case ColideTag::three:return "three";
		break;
	default:
		break; return "このタグはColideTag_Stringに定義されていません";
	}
	return "エラー : 関数Colidetag_Stringを確認してください";
}
