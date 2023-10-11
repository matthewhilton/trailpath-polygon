@tool
extends Node

func _ready():
	$Path3D.curve_changed.connect(_on_curve_change)

func _on_curve_change():
	print("Rebaking banking curve")
	var c: BankedCurve3D = $Path3D.curve
	c.bake_curve()
