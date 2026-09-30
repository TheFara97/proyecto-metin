import uiScriptLocale
window = {
	"name" : "LoginWindow",
	"sytle" : ("movable",),
	"x" : 0, "y" : 0,
	"width" : SCREEN_WIDTH, "height" : SCREEN_HEIGHT,
	"children" : (
		{
			"name" : "backGround",
			"type" : "expanded_image",
			"x" : 0, "y" : 0,
			"x_scale" : float(SCREEN_WIDTH) / 1366.0,
			"y_scale" : float(SCREEN_HEIGHT) / 768.0,
			"image": "d:/ymir work/arezzo/loadingwindow/loading_0.png",
		},
		{
			"name" : "logo",
			"type" : "image",
			"x" : 0, "y" : 180,
			"horizontal_align" : "center",
			"vertical_align" : "top",
			"image" : "d:/ymir work/arezzo/loginwindow/logo.png",
		},
	),
}
