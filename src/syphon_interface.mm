#include "godot_cpp/core/print_string.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

#import <Syphon/SyphonMetalClient.h>
#import <Syphon/SyphonMetalServer.h>
#import <Syphon/SyphonServerDirectory.h>

id<MTLCommandQueue> globalCommandQueue = nil;

void *InitSyphonServer(const char *name, void *metal_device) {
	SyphonMetalServer *new_server = [SyphonMetalServer alloc];
	[new_server initWithName:[NSString stringWithUTF8String:name] device:(id<MTLDevice>)metal_device options:nil];
	return new_server;
}

void DeInitSyphonServer(void *server) {
	SyphonMetalServer *ptr = (SyphonMetalServer *)server;
	[ptr stop];
	[ptr dealloc];
}

void PublishFrame(void *server, int64_t metal_texture, int64_t width, int64_t height) {
	SyphonMetalServer *ptr = (SyphonMetalServer *)server;

	NSSize size;
	size.width = width;
	size.height = height;

	NSRect region;
	region.origin.x = 0;
	region.origin.y = 0;
	region.size = size;

	if (globalCommandQueue == nil) {
		globalCommandQueue = [ptr.device newCommandQueue];
	}

	id<MTLCommandBuffer> buffer = [globalCommandQueue commandBuffer];

	[ptr publishFrameTexture:(id<MTLTexture>)metal_texture onCommandBuffer:buffer imageRegion:region flipped:true];
	[buffer commit];
}

void InitNotificationCenter() {
	// Syphon requires that we have a NotificationCenter set up to list for other servers on the current machine
	[[NSNotificationCenter defaultCenter] addObserverForName:SyphonServerAnnounceNotification
													  object:nil
													   queue:nil
												  usingBlock:^(NSNotification *_Nonnull note){
												  }];
}

godot::Dictionary GetServerNames() {
	godot::Dictionary servers;
	for (NSDictionary *serverDescription in [[SyphonServerDirectory sharedDirectory] servers]) {
		NSString *appName = [serverDescription objectForKey:SyphonServerDescriptionAppNameKey];
		NSString *uuid = [serverDescription objectForKey:SyphonServerDescriptionUUIDKey];
		servers[uuid.UTF8String] = appName.UTF8String;
	}

	return servers;
}

void *CreateSyphonClient(const char *uuid, void *metal_device) {
	NSDictionary *foundServer;
	for (NSDictionary *serverDescription in [[SyphonServerDirectory sharedDirectory] servers]) {
		NSString *currUuid = [serverDescription objectForKey:SyphonServerDescriptionUUIDKey];
		if (currUuid.UTF8String == uuid) {
			foundServer = serverDescription;
			break;
		}
	}

	if (foundServer == nullptr) {
		godot::print_error("Could not create SyphonMetalClient! Couldn't find the server UUID in shared directory!");
		return nullptr;
	}

	SyphonMetalClient *client = [SyphonMetalClient alloc];
	[client initWithServerDescription:foundServer device:(id<MTLDevice>)metal_device options:nil newFrameHandler:nil];
	return client;
}

void StopClient(void *client_ptr) {
	SyphonMetalClient *client = (SyphonMetalClient *)client_ptr;
	[client stop];
	[client dealloc];
}

bool ClientHasFrame(void *client_ptr) {
	SyphonMetalClient *client = (SyphonMetalClient *)client_ptr;
	return [client hasNewFrame];
}

bool ClientIsValid(void *client_ptr) {
	SyphonMetalClient *client = (SyphonMetalClient *)client_ptr;
	return [client isValid];
}

int64_t ClientGetFrameTexture(void *client_ptr) {
	SyphonMetalClient *client = (SyphonMetalClient *)client_ptr;
	return (int64_t)[client newFrameImage];
}

int32_t ClientGetFrameWidth(void *client_ptr) {
	SyphonMetalClient *client = (SyphonMetalClient *)client_ptr;
	id<MTLTexture> img = [client newFrameImage];
	return img.width;
}

int32_t ClientGetFrameHeight(void *client_ptr) {
	SyphonMetalClient *client = (SyphonMetalClient *)client_ptr;
	id<MTLTexture> img = [client newFrameImage];
	return img.height;
}