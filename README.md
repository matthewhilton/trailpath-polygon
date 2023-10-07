# Trail Path Polygon

A Godot addon that allows you to build trail paths. Connects to a Path3D node, similar to how `CSGPolygon3D` works. The trail paths have customisable widths, and left/right banking (raising one edge to form a slope).

![Example photo](trailpathpolygon.png)

## Features
- Specify path width via `Curve`.
- Specify left banking via `Curve`.
- Specify right banking via `Curve`.

## Version support

Godot 4.1.2 (for prebuilt, other versions require you to build it yourself)

# Usage

This addon is written in C++ using the GDExtension API. Therefore, you must use either the pre-built binaries, or build it yourself.

## Download pre-built

1. Click on `Actions` above
2. Choose the latest successful run
3. Under `Artifacts` download `trailpath-polygon`
4. Copy these your project, under `addons/trailpathpolygon/bin`
5. From the repository, copy the `trailpathpolygon.gdextension` file and put it in `addons/trailpathpolygon`

Godot should recognise the extension, and a new node `TrailPolygon3D` should now appear in the node selector.

## Build it yourself

1. Clone the repository
2. `scons platform=windows`
3. Copy the `bin` folder generated by `scons`, into your `addons/trailpathpolygon` folder.