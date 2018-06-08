// Copyright 2018 EpicLeaderboard.com, All Rights Reserved.
#include "IEpicLeaderboard.h"
#include "EpicLeaderboardObject.h"

class FEpicLeaderboard : public IEpicLeaderboard
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FEpicLeaderboard, EpicLeaderboard )

void FEpicLeaderboard::StartupModule()
{
	UEpicLeaderboardObject::StaticClass();
}

void FEpicLeaderboard::ShutdownModule()
{
}