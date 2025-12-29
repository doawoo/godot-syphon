extends Node

var client: GodotSyphonClient = null;
@export var tex: Texture2DRD = null;

var check_interval = 2.0

func _ready() -> void:
	client = GodotSyphonClient.new()

func _process(_delta: float) -> void:
	if client != null and !client.connection_valid():
		if GodotSyphonClient.get_server_list().keys().size() > 0:
			var uuid = GodotSyphonClient.get_server_list().keys()[0]
			client.connect_to_server(uuid)
	elif client != null and client.connection_valid() and client.has_new_frame():
		if tex == null:
			var rid = client.get_frame_tex_id()
			doIt(rid)
		

func doIt(tex_id) -> void:
	var rd = RenderingServer.get_rendering_device()
	var width = client.get_frame_width()
	var height = client.get_frame_height()
	var bits = RenderingDevice.TEXTURE_USAGE_COLOR_ATTACHMENT_BIT | RenderingDevice.TEXTURE_USAGE_CAN_COPY_FROM_BIT | RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT
	var texRid = rd.texture_create_from_extension(RenderingDevice.TextureType.TEXTURE_TYPE_2D, RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM, RenderingDevice.TEXTURE_SAMPLES_1, bits, tex_id, width, height, 1, 1)
	tex = Texture2DRD.new()
	tex.texture_rd_rid = texRid
	$TextureRect.texture = tex