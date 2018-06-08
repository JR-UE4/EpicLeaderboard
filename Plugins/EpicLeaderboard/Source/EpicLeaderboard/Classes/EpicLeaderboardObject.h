// Copyright 2018 EpicLeaderboard.com, All Rights Reserved.
#pragma once

#include "Runtime/Online/HTTP/Public/Http.h"

#include "EpicLeaderboardObject.generated.h"

USTRUCT(BlueprintType)
struct FEpicLeaderboardEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int rank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString country;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FString> Metadata;

	UPROPERTY()
	FString meta;
};
 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEpicLeaderboardResponse, UEpicLeaderboardObject*, Leaderboard);

UCLASS(BlueprintType)
class UEpicLeaderboardObject : public UObject
{
	GENERATED_BODY()

public:
	UEpicLeaderboardObject(const FObjectInitializer& ObjectInitializer);


	//Utilities
	UFUNCTION(BlueprintCallable, Category = "EpicLeaderboard|Utilities")
	static FString CleanupName(FString name);

	//Json serialization
	FString SerializeMap(const TMap<FString, FString> &metadata);
	void DeserializeMap(FString json, TMap<FString, FString> &map);

	//Delegates
	UPROPERTY(BlueprintAssignable, Category = "EpicLeaderboard|Delegates")
	FEpicLeaderboardResponse OnSuccess;

	UPROPERTY(BlueprintAssignable, Category = "EpicLeaderboard|Delegates")
	FEpicLeaderboardResponse OnFailure;


	//Properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EpicLeaderboard|Properties")
	FString ID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EpicLeaderboard|Properties")
	FString Key;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EpicLeaderboard|Properties")
	TArray<FEpicLeaderboardEntry> LeaderboardEntries;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "EpicLeaderboard|Properties")
	FEpicLeaderboardEntry PlayerEntry;


	/** Fetches entries for this leaderboard
	* @param PlayerName - The player for which to return scores for.
	* @param AroundPlayer - Return scores centered around player instead of the top scores.
	*/
	UFUNCTION(BlueprintCallable, Category = "EpicLeaderboard|Leaderboard")
	void GetLeaderboardEntries(FString PlayerName, bool AroundPlayer);

	/** Submit a leaderboard entry
	* @param PlayerName - The player for which this score should be submitted.
	* @param PlayerScore - The score to be submitted.
	*/
	UFUNCTION(BlueprintCallable, Category = "EpicLeaderboard|Leaderboard")
	void SubmitEntry(FString PlayerName, float PlayerScore);

	/** Submit a leaderboard entry with meta information attached
	* @param PlayerName - The player for which this score should be submitted.
	* @param PlayerScore - The score to be submitted.
	* @param Metadata - The meta information to be submitted.
	*/
	UFUNCTION(BlueprintCallable, Category = "EpicLeaderboard|Leaderboard")
	void SubmitEntryWithMetadata(FString PlayerName, float PlayerScore, const TMap<FString, FString> &Metadata);

	/** Return the leaderboard for this ID
	* @param LeaderboardID - The LeaderboardID from http://EpicLeaderboard.com.
	* @param LeaderboardKey - The LeaderboardKey from http://EpicLeaderboard.com.
	* @return The EpicLeaderboardObject for this LeaderboardID.
	*/
	UFUNCTION(BlueprintCallable, Category = "EpicLeaderboard|Leaderboard")
	static UEpicLeaderboardObject* GetEpicLeaderboard(FString LeaderboardID, FString LeaderboardKey);

private:

	void SubmitScoreInternal(FString username, float score, FString metadata);

	//callbacks
	void OnHighscoreResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnScoreSubmitResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
