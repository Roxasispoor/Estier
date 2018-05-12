// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Room.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FRoom
{
	GENERATED_USTRUCT_BODY()

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
		int32 IPplayerActuels[20];
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
		int32 playerMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	int32 place;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room")
	uint8 Name[100];

	
	FRoom()
	{};
};
/*UFUNCTION(BlueprintCallable, Category = UDPSender)
uint8* GetName(FRoom & room) { return room.Name; };
UFUNCTION(BlueprintCallable, Category = UDPSender)
int32* GetIPplayerActuels(FRoom & room) { return room.IPplayerActuels; };
*/