#include "godot_syphon_server.h"
#include "syphon_interface.h"
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/viewport.hpp>

void GodotSyphonServer::_bind_methods() {
	godot::ClassDB::bind_method(D_METHOD("print_type", "variant"), &GodotSyphonServer::print_type);
	godot::ClassDB::bind_method(D_METHOD("start_server", "string"), &GodotSyphonServer::start_server);
	godot::ClassDB::bind_method(D_METHOD("stop_server"), &GodotSyphonServer::stop_server);
	godot::ClassDB::bind_method(D_METHOD("server_is_running"), &GodotSyphonServer::is_running);
	godot::ClassDB::bind_method(D_METHOD("publish_frame", "rid", "width", "height"), &GodotSyphonServer::publish_frame);
}

void GodotSyphonServer::print_type(const Variant &p_variant) const {
	print_line(vformat("Type: %d", p_variant.get_type()));
}

void GodotSyphonServer::start_server(const String &server_name) {
	if (this->server_pointer != nullptr) {
		print_error("Cannot start the Syphon server twice on the same instance!");
		return;
	}

	RenderingServer *rendering_server = RenderingServer::get_singleton();
	RenderingDevice *rendering_device = rendering_server->get_rendering_device();

	if (rendering_device) {
		void *metal_device = (void *)rendering_device->get_driver_resource(RenderingDevice::DRIVER_RESOURCE_LOGICAL_DEVICE, RID(), 0);
		this->server_pointer = InitSyphonServer(server_name.utf8(), metal_device);
		print_line("Init Syphon Server");
	} else {
		print_error("Could not fetch the current DisplayServer! Cannot start Syphon Server!");
	}
}

void GodotSyphonServer::stop_server() {
	if (this->server_pointer != nullptr) {
		DeInitSyphonServer(this->server_pointer);
	}
}

bool GodotSyphonServer::is_running() {
	return this->server_pointer != nullptr;
}

void GodotSyphonServer::publish_frame(const RID texture_rid, const int width, const int height) {
	// Cannot fetch the texture's ID if we don't have a rendering server!!
	if (server_pointer == nullptr) {
		print_error("Cannot publish texture, server not started!");
		return;
	}

	// Fetch the native ID from the Metal API
	auto render_server = RenderingServer::get_singleton();
	int64_t metal_texture = render_server->texture_get_native_handle(texture_rid);
	PublishFrame(this->server_pointer, metal_texture, width, height);
}
