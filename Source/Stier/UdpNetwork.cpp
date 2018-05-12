#include "UdpNetwork.h"

AUdpNetwork::AUdpNetwork(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SenderSocket = NULL;

	ShowOnScreenDebugMessages = true;
}

void AUdpNetwork::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (SenderSocket)
	{
		SenderSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SenderSocket);
	}
	//Super::EndPlay(EndPlayReason);
	//~~~~~~~~~~~~~~~~


}


bool AUdpNetwork::StartUDPSender(
	const FString& YourChosenSocketName,
	const FString& TheIP,
	const int32 ThePort) {
	//Create Remote Address.
	RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bIsValid;
	RemoteAddr->SetIp(*TheIP, bIsValid);
	RemoteAddr->SetPort(ThePort);

	if (!bIsValid)
	{
		ScreenMsg("UDP Sender>> IP address was not valid!", TheIP);
		return false;
	}

	SenderSocket = FUdpSocketBuilder(*YourChosenSocketName)
		.AsReusable()
		.WithBroadcast()
		;



	//Set Send Buffer Size
	int32 SendSize = 2 * 1024 * 1024;
	SenderSocket->SetSendBufferSize(SendSize, SendSize);
	SenderSocket->SetReceiveBufferSize(SendSize, SendSize);

	UE_LOG(LogTemp, Log, TEXT("\n\n\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
	UE_LOG(LogTemp, Log, TEXT("***UDP**** Sender Initialized Successfully!!!"));
	UE_LOG(LogTemp, Log, TEXT("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n"));

	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	FTimerHandle TimerHandle;

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AUdpNetwork::WithRecvfrom, 0.01f, true, 1.0f);

	return true;
}

bool AUdpNetwork::SenderSendString(FString toSend)
{
	if (!SenderSocket)
	{
		ScreenMsg("No sender socket");
		return false;
	}
	//~~~~~~~~~~~~~~~~

	int32 BytesSent = 0;
	auto ansi_login = StringCast<ANSICHAR>(*toSend);
	//const TCHAR* wavLink = *toSend;
	SenderSocket->SendTo((uint8*)(&ansi_login), toSend.Len()+1, BytesSent, *RemoteAddr);

	if (BytesSent <= 0)
	{
		const FString Str = "Socket is valid but the receiver received 0 bytes, make sure it is listening properly!";
		UE_LOG(LogTemp, Error, TEXT("%s"), *Str);
		ScreenMsg(Str);
		return false;
	}
	ScreenMsg("Message Sent = ", toSend);

	ScreenMsg("UDP~ Send Succcess! Bytes Sent = ", BytesSent);



	return true;

}
bool AUdpNetwork::StartUDPReceiver(
	const FString& YourChosenSocketName,
	const FString& TheIP,
	const int32 ThePort
) {

	ScreenMsg("RECEIVER INIT");
	ScreenMsg("TAILLE room", sizeof(struct FRoom));
	//~~~

	FIPv4Address Addr;
	FIPv4Address::Parse(TheIP, Addr);

	//Create Socket
	FIPv4Endpoint Endpoint(Addr, ThePort);

	//BUFFER SIZE
	int32 BufferSize = 2 * 1024 * 1024;

	receiverSocket = FUdpSocketBuilder(*YourChosenSocketName)
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);
	;
	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	FTimerHandle TimerHandle;

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AUdpNetwork::WithRecvfrom, 0.01f, true, 1.0f);
	
		//UDPReceiver = new FUdpSocketReceiver(receiverSocket, ThreadWaitTime, TEXT("UDP RECEIVER"));
	
	//GetWorldTimerManager().SetTimer(this,&AUdpNetwork::WithRecvfrom, 0.01, true,0.01);
	
	//UDPReceiver->OnDataReceived().BindUObject(this, &AUdpNetwork::Recv);
	//UDPReceiver->Start();
	return true;
}


void AUdpNetwork::Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt)
{
	uint8 data[512];
	FMemory::Memzero(data, 512);
	
	FMemory::Memcpy(data, ArrayReaderPtr->GetData(), ArrayReaderPtr->Num());
	
	FString ReceivedData = ((const char*)data);

	ScreenMsg("As String!!!!! ~>", ReceivedData);
	/*
	TArray<uint8> ReceivedData;
	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	uint32 Size;
	if (receiverSocket->HasPendingData(Size))
	{
		ScreenMsg("We have data");

		ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));

		int32 Read = 0;
		receiverSocket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), Read, *targetAddr);
	}
	FString Rata = ((const char *)ReceivedData.GetData()); */
									//BP Event
	BPEvent_DataReceived(ReceivedData);
}

void AUdpNetwork::WithRecvfrom()
{
	uint8 receivedData[200];
	struct FRoom room;

	TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	uint32 Size;
	while (SenderSocket->HasPendingData(Size))
	{
		ScreenMsg("We have data",Size);

		
			int32 Read = 0;
			SenderSocket->RecvFrom(receivedData, (uint32)188, Read, *targetAddr);
			//ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));
			ScreenMsg("Read", Read);
			receivedData[Read] = '\0';
			FMemory::Memcpy(&room, receivedData,sizeof(receivedData));

			ScreenMsg("We have data", (char *)(receivedData));
			FString Rata = (char *)(room.Name);
			//FString Rata = BytesToString(room.Name, 100);
			BPEvent_RoomDecompReceived(Rata,room.IPplayerActuels[0],room.playerMax,room.place);
		
		}
	int BytesSent=0;
	//receiverSocket->SendTo((uint8*)("mabite"), 7 + 1, BytesSent, *targetAddr);


}
