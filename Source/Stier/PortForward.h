// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../ThirdParty/miniUPnP/include/miniupnpc.h"
#include "../ThirdParty/miniUPnP/include/upnpcommands.h"
#include "../ThirdParty/miniUPnP/include/upnperrors.h"
#include "PortForward.generated.h"

UCLASS()
class STIER_API APortForward : public AActor
{

	GENERATED_BODY()
	
public:
	// COM interfaces
	IUPnPNAT* Nat;
	IStaticPortMappingCollection *Collection;
	IStaticPortMapping           *Mapping;
	// Methods
	APortForward();
	~APortForward();
	bool SetupForward(char protocol, int externalport, int internalport, read ipaddress, read name);
	bool Access();
	bool Add(char protocol, int externalport, int internalport, read ipaddress, read name);	// Sets default values for this actor's properties
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
