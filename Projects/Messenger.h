#pragma once

#include <string>
class Messenger
{
public:
	Messenger();
	virtual ~Messenger();
	
	void Load();
	void Message(std::string chara,std::string messageID);

private:

};

