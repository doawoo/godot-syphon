extends Node2D

var server:GodotSyphonServer = GodotSyphonServer.new()

func _ready() -> void:
	server.start_server("ExampleGodotServer")

func _process(delta: float) -> void:
	$Sprite2D.rotate(delta * 1.0)

func _physics_process(_delta: float) -> void:
	await RenderingServer.frame_post_draw
	var tex = get_viewport().get_texture()
	server.publish_frame(tex.get_rid(), tex.get_width(), tex.get_height())
