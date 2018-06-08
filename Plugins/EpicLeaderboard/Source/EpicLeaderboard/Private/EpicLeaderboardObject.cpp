// Copyright 2018 EpicLeaderboard.com, All Rights Reserved.
#include "EpicLeaderboardObject.h"
#include "SecureHash.h"
#include "Runtime/JsonUtilities/Public/JsonUtilities.h"

#include <string>
#include <regex>

UEpicLeaderboardObject::UEpicLeaderboardObject(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer )
{
	LeaderboardEntries = TArray<FEpicLeaderboardEntry>();
}

FString UEpicLeaderboardObject::CleanupName(FString name)
{
	//name limited to 20 chars
	name = name.Left(20);

	std::string s(TCHAR_TO_UTF8(*name));

	//remove special characters
	std::regex e("[^A-Za-z0-9]");

	std::string clean = std::regex_replace(s, e, std::string(""));

	return FString(clean.c_str());
}

FString UEpicLeaderboardObject::SerializeMap(const TMap<FString, FString> &metadata)
{
	FString jsonStr;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&jsonStr);
	JsonWriter->WriteObjectStart();

	for (const auto& Entry : metadata)
	{
		JsonWriter->WriteValue(Entry.Key, Entry.Value);
	}

	JsonWriter->WriteObjectEnd();
	JsonWriter->Close();

	return jsonStr;
}

void UEpicLeaderboardObject::DeserializeMap(FString json, TMap<FString, FString> &map)
{
	map.Empty();

	TSharedPtr<FJsonValue> JsonParsed;
	TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(json);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed) && JsonParsed.IsValid())
	{
		for (const auto& JsonEntry : JsonParsed->AsObject()->Values)
		{
			map.Add(JsonEntry.Key, JsonEntry.Value->AsString());
		}
	}
}

void UEpicLeaderboardObject::GetLeaderboardEntries(FString PlayerName, bool AroundPlayer = false)
{
	//setup the request
	FString url = FString::Printf(TEXT("http://%s/api/getScores.php?accessID=%s&username=%s&around=%d"), TEXT("epicleaderboard.com"), *ID, *PlayerName, AroundPlayer);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UEpicLeaderboardObject::OnHighscoreResponseReceived);

	Request->SetURL(url);
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-EpicLeaderboard");
	Request->SetHeader("Content-Type", TEXT("application/x-www-form-urlencoded"));
	Request->SetHeader("Accept", TEXT("application/vnd.epicleaderboard.v2+json"));
	Request->ProcessRequest();
}

void UEpicLeaderboardObject::OnHighscoreResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		OnFailure.Broadcast(this);
		return;
	}

	FString data = Response->GetContentAsString();

	//parse json response
	TSharedPtr<FJsonValue> JsonParsed;
	TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(data);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed) && JsonParsed.IsValid())
	{
		//parse top score list 
		TSharedPtr<FJsonValue> *topScores = JsonParsed->AsObject()->Values.Find(TEXT("scores"));

		if (topScores != nullptr && topScores->IsValid())
		{
			//clear entries
			LeaderboardEntries.Empty();

			FJsonObjectConverter::JsonArrayToUStruct<FEpicLeaderboardEntry>(topScores->Get()->AsArray(), &LeaderboardEntries, 0, 0);
		
			//deserialize metadata
			for (auto& Entry : LeaderboardEntries)
			{
				DeserializeMap(Entry.meta, Entry.Metadata);
			}
		}

		//clear struct
		PlayerEntry = FEpicLeaderboardEntry();

		//parse player score
		TSharedPtr<FJsonValue> *playerScore = JsonParsed->AsObject()->Values.Find(TEXT("playerscore"));

		if (playerScore != nullptr && playerScore->IsValid())
		{
			FJsonObjectConverter::JsonValueToUProperty(*playerScore, UEpicLeaderboardObject::StaticClass()->FindPropertyByName(TEXT("PlayerEntry")), &PlayerEntry, 0, 0);
		
			//deserialize metadata
			DeserializeMap(PlayerEntry.meta, PlayerEntry.Metadata);
		}
	}

	OnSuccess.Broadcast(this);
}

void UEpicLeaderboardObject::SubmitEntry(FString PlayerName, float PlayerScore)
{
	SubmitScoreInternal(PlayerName, PlayerScore, "");
}

void UEpicLeaderboardObject::SubmitEntryWithMetadata(FString PlayerName, float PlayerScore, const TMap<FString, FString> &metadata)
{
	//generate the meta json
	FString metaJson = SerializeMap(metadata);

	SubmitScoreInternal(PlayerName, PlayerScore, metaJson);
}

void UEpicLeaderboardObject::SubmitScoreInternal(FString username, float score, FString Metadata)
{
	//cleanup the name
	username = CleanupName(username);

	//store last submitted username
	PlayerEntry.username = username;

	//generate the key
	FString hashSource = FString::Printf(TEXT("%s%.3f%s%s"), *username, score, *Key, *Metadata);
	FString generatedKey = FMD5::HashAnsiString(*hashSource);

	//setup the request
	FString url = FString::Printf(TEXT("http://%s/api/submitScore.php"), TEXT("epicleaderboard.com"));

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UEpicLeaderboardObject::OnScoreSubmitResponseReceived);

	Request->SetURL(url);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-EpicLeaderboard");
	Request->SetHeader("Content-Type", TEXT("application/x-www-form-urlencoded"));
	Request->SetHeader("Accept", TEXT("application/vnd.epicleaderboard.v2+json"));

	FString content = FString::Printf(TEXT("accessID=%s&username=%s&score=%.3f&meta=%s&hash=%s"), *ID, *username, score, *Metadata, *generatedKey);

	Request->SetContentAsString(content);

	Request->ProcessRequest();
}

void UEpicLeaderboardObject::OnScoreSubmitResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		OnFailure.Broadcast(this);
		return;
	}

	//score submitted now update the leaderboard
	GetLeaderboardEntries(PlayerEntry.username, true);
}

UEpicLeaderboardObject* UEpicLeaderboardObject::UEpicLeaderboardObject::GetEpicLeaderboard(FString ID, FString Key)
{
	UEpicLeaderboardObject* EpicLeaderboard = nullptr;

	EpicLeaderboard = NewObject<UEpicLeaderboardObject>();
	EpicLeaderboard->ID = ID;
	EpicLeaderboard->Key = Key;

	return EpicLeaderboard;
}