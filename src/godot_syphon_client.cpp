#include "godot_syphon_client.h"
#include "syphon_interface.h"
#include <OpenGL/gl.h>
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/texture2drd.hpp>
#include <godot_cpp/classes/viewport.hpp>

void GodotSyphonClient::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &GodotSyphonClient::print_type);
	godot::ClassDB::bind_method(D_METHOD("connect_to_server", "server_uuid"), &GodotSyphonClient::connect_to_server);
	godot::ClassDB::bind_method(D_METHOD("has_new_frame"), &GodotSyphonClient::client_has_new_frame);
	godot::ClassDB::bind_method(D_METHOD("connection_valid"), &GodotSyphonClient::client_is_connected);
	godot::ClassDB::bind_method(D_METHOD("get_frame_tex_id"), &GodotSyphonClient::get_frame_tex_id);
	godot::ClassDB::bind_method(D_METHOD("get_frame_width"), &GodotSyphonClient::get_frame_width);
	godot::ClassDB::bind_method(D_METHOD("get_frame_height"), &GodotSyphonClient::get_frame_height);
	godot::ClassDB::bind_static_method("GodotSyphonClient", D_METHOD("get_server_list"), &GodotSyphonClient::get_server_list);
}

void GodotSyphonClient::print_type(const Variant &p_variant) const {
	print_line(vformat("Type: %d", p_variant.get_type()));
}

void GodotSyphonClient::connect_to_server(const String &uuid) {
	RenderingServer *rendering_server = RenderingServer::get_singleton();
	RenderingDevice *rendering_device = rendering_server->get_rendering_device();

	auto names = GetServerNames();
	auto server_exists = names.has(uuid);
	if (!server_exists) {
		print_error("Cannot connect to server " + uuid + " because it does not exist!");
		return;
	} else {
		this->uuid = uuid;
		void *metal_device = (void *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_LOGICAL_DEVICE, RID(), 0);
		auto client = CreateSyphonClient(this->uuid.utf8(), metal_device);
		if (client != nullptr) {
			print_line("Connected to Syphon server: " + uuid);
			this->client_pointer = client;
		}
	}
}

bool GodotSyphonClient::client_is_connected() {
	if (this->client_pointer == nullptr) {
		return false;
	}

	return ClientIsValid(this->client_pointer);
}

bool GodotSyphonClient::client_has_new_frame() {
	if (this->client_pointer == nullptr) {
		return false;
	}

	return ClientHasFrame(this->client_pointer);
}

int64_t GodotSyphonClient::get_frame_tex_id() {
	if (this->client_pointer == nullptr) {
		return -1;
	}

	return ClientGetFrameTexture(this->client_pointer);
}

int GodotSyphonClient::get_frame_width() {
	if (this->client_pointer == nullptr) {
		return 0;
	}

	return ClientGetFrameWidth(this->client_pointer);
}

int GodotSyphonClient::get_frame_height() {
	if (this->client_pointer == nullptr) {
		return 0;
	}

	return ClientGetFrameHeight(this->client_pointer);
}

// Static helper function
Dictionary GodotSyphonClient::get_server_list() {
	return GetServerNames();
}