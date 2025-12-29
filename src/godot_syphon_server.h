#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <godot_cpp/classes/texture2d.hpp>

using namespace godot;

class GodotSyphonServer : public RefCounted {
	GDCLASS(GodotSyphonServer, RefCounted)

protected:
	static void _bind_methods();

private:
	void* server_pointer = nullptr;
	void* mtl_cmd_buffer = nullptr;

public:
	GodotSyphonServer() = default;
	~GodotSyphonServer() override = default;

	void print_type(const Variant &p_variant) const;
	
	void start_server(const String &p_variant);
	void stop_server();
	bool is_running();
	void publish_frame(const RID texRID, int width, int height);
};
