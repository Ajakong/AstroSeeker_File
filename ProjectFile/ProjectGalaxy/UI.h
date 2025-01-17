#pragma once

class UI
{
	
public:
	struct UIinfo
	{
		float x;
		float y;
		float width;
		float height;
	};
	UI();
	virtual ~UI();

	void Init();
	void Update();
	void Draw(float m_hp);


private:

	int m_uiAssetHandle;


};

