#include <stdint.h>

void* InitSyphonServer(const char* server_name, void* metal_device);
void DeInitSyphonServer(void* server);
void PublishFrame(void* server, int64_t metal_texture, int64_t width, int64_t height);
void InitNotificationCenter();
void* CreateSyphonClient(const char* uuid, void* context_prt);
bool ClientHasFrame(void* client_prt);
bool ClientIsValid(void* client_prt);
void StopClient(void* client_prt);
int64_t ClientGetFrameTexture(void *client_ptr);
int32_t ClientGetFrameWidth(void *client_ptr);
int32_t ClientGetFrameHeight(void *client_ptr);
godot::Dictionary GetServerNames();