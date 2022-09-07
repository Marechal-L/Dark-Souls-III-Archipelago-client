#pragma once

#include "Core.h"
#include "GameHook.h"
#include "subprojects/apclientpp/apclient.hpp"
#include "subprojects/apclientpp/apuuid.hpp"

using nlohmann::json;

class CArchipelago {
public:
	BOOL Initialise(std::string URI);
	VOID say(std::string message);
	BOOLEAN isConnected();
	VOID update();
	VOID gameFinished();
	VOID sendDeathLink();
};
