#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <godot_cpp/classes/texture2drd.hpp>
#include <godot_cpp/classes/image_texture.hpp>

using namespace godot;

class GodotSyphonClient : public RefCounted {
	GDCLASS(GodotSyphonClient, RefCounted)

protected:
	static void _bind_methods();

private:
  String uuid;
	void* client_pointer = nullptr;

public:

	GodotSyphonClient() = default;
	~GodotSyphonClient() override = default;

	void print_type(const Variant &p_variant) const;
  void connect_to_server(const String &server_uuid);
	bool client_is_connected();
	bool client_has_new_frame();
	int64_t get_frame_tex_id();
	int get_frame_width();
	int get_frame_height();
  static Dictionary get_server_list();
};
