#ifndef TRAILPATHPOLYGON_H
#define TRAILPATHPOLYGON_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/visual_instance3d.hpp>
#include <godot_cpp/classes/geometry_instance3d.hpp>
#include <godot_cpp/classes/path3d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/curve.hpp>

namespace godot {

class TrailPathPolygon : public GeometryInstance3D {
    GDCLASS(TrailPathPolygon, GeometryInstance3D)

private:
    double time_passed;
    Ref<ArrayMesh> root_mesh;

    NodePath path_node;
    Path3D *path = nullptr;

    bool autoupdate = true;

    float _divide_size = 1.0;

    //Ref<Curve> _left_banking_curve;
    //Ref<Curve> _right_banking_curve;
    Ref<Curve> _width_curve;

    void set_path_node(const NodePath &p_path);
    NodePath get_path_node() const;

    void _path_changed();
	void _path_exited();
protected:
    static void _bind_methods();

public:
    ~TrailPathPolygon();

    void _ready();
    void _process(double delta);\

    void set_autoupdate(bool val);
    bool get_autoupdate();

    void set_divide_size(float p_num);
    float get_divide_size();

   // void set_left_banking_curve(Ref<Curve> p_curve);
	//Ref<Curve> get_left_banking_curve() const;

   // void set_right_banking_curve(Ref<Curve> p_curve);
	//Ref<Curve> get_right_banking_curve() const;

    void set_width_curve(Ref<Curve> p_curve);
	Ref<Curve> get_width_curve() const;

    void bake_mesh();
};

}

#endif