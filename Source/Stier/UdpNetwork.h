// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine.h"
#include "Networking.h"
#include "Room.h"
#include "UdpNetwork.generated.h"

UCLASS()
class STIER_API AUdpNetwork : public AActor
{

	GENERATED_UCLASS_BODY()

		bool IsUDP;
	FString receivedData;
	FUdpSocketReceiver* UDPReceiver = nullptr;
	
public:
	UFUNCTION(BlueprintCallable, Category = UDPSender)
	bool SenderSendString(FString ToSend);

	UFUNCTION(BlueprintCallable, Category = UDPSender)
	bool StartUDPReceiver(const FString & YourChosenSocketName, const FString & TheIP, const int32 ThePort);
	void Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);
	/** Data has been received!! */
	UFUNCTION(BlueprintImplementableEvent)
		void BPEvent_DataReceived(const FString &receivedData);
	UFUNCTION(BlueprintImplementableEvent)
		void BPEvent_RoomReceived(const struct FRoom &receivedData);
	UFUNCTION(BlueprintImplementableEvent)
		void BPEvent_RoomDecompReceived(const FString &nom,const int32 & ips,const int32& playersMax,const int32& place);



	UFUNCTION(BlueprintCallable, Category = UDPSender)
		bool StartUDPSender(
			const FString& YourChosenSocketName,
			const FString& TheIP,
			const int32 ThePort
		);
	void WithRecvfrom();

public:
	TSharedPtr<FInternetAddr>	RemoteAddr;
	FSocket* receiverSocket;
	FSocket* SenderSocket;
	
	


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP Sender")
		bool ShowOnScreenDebugMessages;


	//ScreenMsg
	FORCEINLINE void ScreenMsg(const FString& Msg)
	{
		if (!ShowOnScreenDebugMessages) return;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *Msg);
	}
	FORCEINLINE void ScreenMsg(const FString& Msg, const float Value)
	{
		if (!ShowOnScreenDebugMessages) return;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s %f"), *Msg, Value));
	}
	FORCEINLINE void ScreenMsg(const FString& Msg, const FString& Msg2)
	{
		if (!ShowOnScreenDebugMessages) return;
		GEngine->AddOnScreenDebugMessage(-1, 500.f, FColor::Red, FString::Printf(TEXT("%s %s"), *Msg, *Msg2));
	}


public:

	/** Called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
