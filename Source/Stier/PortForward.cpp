// Fill out your copyright notice in the Description page of Project Settings.

#include "PortForward.h"

APortForward::APortForward() {

	// Set the COM interface pointers to NULL so we'll know if we've initialized them
	Nat = NULL;
	Collection = NULL;
	Mapping = NULL;
}

// Delete this PortForward object
APortForward::~APortForward() {

	// Release the COM interfaces that we got access to
	if (Mapping) { Mapping->Release();    Mapping = NULL; }
	if (Collection) { Collection->Release(); Collection = NULL; }
	if (Nat) { Nat->Release();        Nat = NULL; }
}

// Takes a protocol 't' for TCP or 'u' for UDP, the port to forward externally and internally, and a program name up to 11 letters a-z and A-Z only
bool APortForward::SetupForward(char protocol, int externalport, int internalport, read ipaddress, read name) {

	// Connect to the COM object, and have it begin talking UPnP to the router
	if (!Access()) return false;

	// Remove any forward the router has for the given protocol and port
	Remove(protocol, externalport); // Even if remove fails, we still want to try to add

									// Add a fresh port mapping with the right name and a checked box
	if (!Add(protocol, externalport, internalport, ipaddress, name)) return false;

	// Everything worked
	return true;
}

// Get access to the COM objects
// Returns true if it works, false if there was an error
bool APortForward::Access() {

	// Initialize COM itself so this thread can use it
	HRESULT result = CoInitialize(NULL); // Must be NULL
	if (FAILED(result)) Report("PortForward::Access CoInitialize failed");
	if (FAILED(result)) return false;

	// Access the IUPnPNAT COM interface, has Windows send UPnP messages to the NAT router
	result = CoCreateInstance(__uuidof(UPnPNAT), NULL, CLSCTX_ALL, __uuidof(IUPnPNAT), (void **)&Nat);
	if (FAILED(result)) Report("PortForward::Access CoCreateInstance IUPnPNAT failed");
	else if (!Nat)      Report("PortForward::Access CoCreateInstance succeeded, but left Nat null");
	if (FAILED(result) || !Nat) return false;

	// Get the collection of forwarded ports from it, has Windows send UPnP messages to the NAT router
	result = Nat->get_StaticPortMappingCollection(&Collection); // Won't work if the NAT has UPnP turned off
	if (FAILED(result))   Report("PortForward::Access Nat->get_StaticPortMappingCollection failed");
	else if (!Collection) Report("PortForward::Access Nat->get_StaticPortMappingCollection succeeded, but left Collection null");
	if (FAILED(result) || !Collection) return false; // Frequently, result is S_OK but Collection is null

													 // Everything worked
	return true;
}

// Takes a protocol 't' for TCP or 'u' for UDP, the ports to forward externally and internally, and the program name and description
// Talks UPnP to the router to setup port forwarding
// Returns false if there was an error
bool APortForward::Add(char protocol, int externalport, int internalport, read ipaddress, read name) {

	// Make a local BSTR with the protocol described in text
	Beaster p;
	if (protocol == 't') p.Set("TCP");
	else                 p.Set("UDP");

	// Express the name and description as BSTRs
	Beaster i(ipaddress);
	Beaster n(name);

	// Have Windows send UPnP messages to the NAT router to get it to forward a port
	if (Mapping) { Mapping->Release(); Mapping = NULL; } // Reuse the mapping interface pointer
	HRESULT result = Collection->Add( // Create a new port mapping, and add it to the collection
		externalport, // The port to forward
		p.B,          // The protocol as the text "TCP" or "UDP" in a BSTR
		internalport, // This computer's internal LAN port to forward to, like 192.168.1.100:internalport
		i.B,          // Internal IP address to forward to, like "192.168.1.100"
		true,         // True to start forwarding now
		n.B,          // Description text the router can show in its Web configuration interface
		&Mapping);    // Access to the IStaticPortMapping interface, if this works
	if (FAILED(result)) Report("PortForward::Add Collection->Add failed");
	else if (!Mapping)  Report("PortForward::Add Collection->Add succeeded, but left Mapping null");
	if (FAILED(result) || !Mapping) return false;
	return true;
}


