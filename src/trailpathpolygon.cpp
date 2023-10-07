#include "trailpathpolygon.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/visual_instance3d.hpp>
#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/variant/basis.hpp>
#include <godot_cpp/variant/plane.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/curve.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/math.hpp>

using namespace godot;

void TrailPathPolygon::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_path_node", "path"), &TrailPathPolygon::set_path_node);
	ClassDB::bind_method(D_METHOD("get_path_node"), &TrailPathPolygon::get_path_node);

    ClassDB::bind_method(D_METHOD("_path_exited"), &TrailPathPolygon::_path_exited);
	ClassDB::bind_method(D_METHOD("_path_changed"), &TrailPathPolygon::_path_changed);
    ClassDB::bind_method(D_METHOD("_update_shape"), &TrailPathPolygon::_update_shape);

    ClassDB::bind_method(D_METHOD("set_left_banking_curve", "curve"), &TrailPathPolygon::set_left_banking_curve);
	ClassDB::bind_method(D_METHOD("get_left_banking_curve"), &TrailPathPolygon::get_left_banking_curve);

    ClassDB::bind_method(D_METHOD("set_right_banking_curve", "curve"), &TrailPathPolygon::set_right_banking_curve);
	ClassDB::bind_method(D_METHOD("get_right_banking_curve"), &TrailPathPolygon::get_right_banking_curve);

    ClassDB::bind_method(D_METHOD("set_width_curve", "curve"), &TrailPathPolygon::set_width_curve);
	ClassDB::bind_method(D_METHOD("get_width_curve"), &TrailPathPolygon::get_width_curve);

    ClassDB::bind_method(D_METHOD("set_divide_size", "divide_size"), &TrailPathPolygon::set_divide_size);
	ClassDB::bind_method(D_METHOD("get_divide_size"), &TrailPathPolygon::get_divide_size);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "path_node", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Path3D"), "set_path_node", "get_path_node");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "left_banking_curve", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_left_banking_curve", "get_left_banking_curve");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "right_banking_curve", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_right_banking_curve", "get_right_banking_curve");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "width_curve", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_width_curve", "get_width_curve");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "divide_size"), "set_divide_size", "get_divide_size");
}

void TrailPathPolygon::set_left_banking_curve(Ref<Curve> p_curve) {
    if (_left_banking_curve != p_curve) {
		if (_left_banking_curve.is_valid()) {
			_left_banking_curve->disconnect("changed", Callable(this, "_path_changed"));
		}
		_left_banking_curve = p_curve;
		if (_left_banking_curve.is_valid()) {
			_left_banking_curve->connect("changed", Callable(this, "_path_changed"));
		}
		_path_changed();
	}
}

void TrailPathPolygon::set_right_banking_curve(Ref<Curve> p_curve) {
    if (_right_banking_curve != p_curve) {
		if (_right_banking_curve.is_valid()) {
			_right_banking_curve->disconnect("changed", Callable(this, "_path_changed"));
		}
		_right_banking_curve = p_curve;
		if (_right_banking_curve.is_valid()) {
			_right_banking_curve->connect("changed", Callable(this, "_path_changed"));
		}
		_path_changed();
	}
}

void TrailPathPolygon::set_width_curve(Ref<Curve> p_curve) {
    if (_width_curve != p_curve) {
		if (_width_curve.is_valid()) {
			_width_curve->disconnect("changed", Callable(this, "_path_changed"));
		}
		_width_curve = p_curve;
		if (_width_curve.is_valid()) {
			_width_curve->connect("changed", Callable(this, "_path_changed"));
		}
		_path_changed();
	}
}

void TrailPathPolygon::set_divide_size(float p_num) {
    // Must never be negative.
    _divide_size = MAX(0.01, p_num);
    _path_changed();
}

float TrailPathPolygon::get_divide_size() {
    return _divide_size;
}

Ref<Curve> TrailPathPolygon::get_width_curve() const {
	return _width_curve;
}

Ref<Curve> TrailPathPolygon::get_left_banking_curve() const {
	return _left_banking_curve;
}

Ref<Curve> TrailPathPolygon::get_right_banking_curve() const {
	return _right_banking_curve;
}

void TrailPathPolygon::set_path_node(const NodePath &p_path) {
    WARN_PRINT("setting path node");

    // Disconnect previous path.
    if (path) {
        WARN_PRINT("removing previous connections");
        if (path->is_connected("tree_exited", Callable(this, "_path_exited"))) {
            path->disconnect("tree_exited", Callable(this, "_path_exited"));
        } 

        if (path->is_connected("curve_changed", Callable(this, "_path_changed"))) {
            path->disconnect("curve_changed", Callable(this, "_path_changed"));
        }
    }

    // Set new path.
    path_node = p_path;
    path = Object::cast_to<Path3D>(get_node_or_null(path_node));

    WARN_PRINT(p_path);

    // Connect new path.
    if (path != NULL) {
        WARN_PRINT("setting new connections");
        path->connect("tree_exited", Callable(this, "_path_exited"), CONNECT_PERSIST);
        path->connect("curve_changed", Callable(this, "_path_changed"), CONNECT_PERSIST);
    }

    _path_changed();
}

NodePath TrailPathPolygon::get_path_node() const {
	return path_node;
}

TrailPathPolygon::~TrailPathPolygon() {
    // Add your cleanup here.
}

void TrailPathPolygon::_path_exited() {
    path = nullptr;
}

void TrailPathPolygon::_path_changed() {
    // csg_shape.cpp calls deferred here, so we probably need to as well.
    call_deferred(StringName("_update_shape"));
}

void TrailPathPolygon::_ready() {
    //set_path_node(path_node);
}

void TrailPathPolygon::_process(double delta) {
}

void TrailPathPolygon::_update_shape() {
    WARN_PRINT("updating shape");

    path = Object::cast_to<Path3D>(get_node_or_null(path_node));

    if(!path) {
        WARN_PRINT("No path, generating empty mesh");
        // Empty mesh.
       // Array array;
        //array.resize(Mesh::ARRAY_MAX);

        // TODO how to dot his properly?
        //array[Mesh::ARRAY_FLAG_USES_EMPTY_VERTEX_ARRAY] = true;

        // TODO reload from saved scene explodes - IMO its probably because of this ?
        //int idx = root_mesh->get_surface_count();
        //root_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, array);
       // set_base(root_mesh->get_rid());
        return;
    }

    WARN_PRINT("generating new mesh");

    set_base(RID());
	root_mesh.unref();
    root_mesh.instantiate();

    Ref<Curve3D> curve = path->get_curve();

    if (curve.is_null() || curve->get_point_count() < 2) {
        return;
    }

    // Since the curve length may not be perfectly divisible by _divide_size
    // We approximate it by turning it into an int (number of divisions)
    // and then dividing the curve length by this amount (to get a perfectly divisible offset).
    int extrusions = MAX((int) ceil(curve->get_baked_length() / _divide_size), 3);
    double bake_length = curve->get_baked_length() / (double) extrusions;

    PackedVector3Array left_verticies;
    PackedVector3Array right_verticies;
    PackedVector3Array left_verticies_bottom;
    PackedVector3Array right_verticies_bottom;

    left_verticies.resize(extrusions + 1);
    right_verticies.resize(extrusions + 1);
    left_verticies_bottom.resize(extrusions + 1);
    right_verticies_bottom.resize(extrusions + 1);

    int cumulative_vertex_index = 0;

    Vector3 forward = Vector3(0,0,0);

    // Find all the left/right points for the top of the mesh.
    for (int i = 0; i < (extrusions + 1); i++) {
        double main_curve_offset = i * bake_length;
        Vector3 t = curve->sample_baked(main_curve_offset);
    
        Vector3 up = Vector3(0,1,0);
        Plane p(up, t);

        // Project next onto plane to get a perpendicular vector.
        Vector3 t_next = p.project(curve->sample_baked(main_curve_offset + bake_length));
        
        // This stops the last extrusion from changing the forward vector
        if(i != extrusions) {
            forward = t.direction_to(t_next);
        }

        Vector3 left = forward.cross(up).normalized();

        Vector3 left_banking_offset = Vector3(0,0,0);
        Vector3 right_banking_offset = Vector3(0,0,0);

        // We never use 0, since 0 gives weird values with curves.
        float percentage = MAX(main_curve_offset / curve->get_baked_length(), 0.01);

        if (_left_banking_curve.is_valid()) {
            // Curve (note NOT Curve3D) is only between 0 and 1, so get the percentage along the main curve to sample.
            float curve_amount = _left_banking_curve->sample(percentage);
            left_banking_offset = Vector3(0, curve_amount, 0);
        }

         if (_right_banking_curve.is_valid()) {
            // Curve (note NOT Curve3D) is only between 0 and 1, so get the percentage along the main curve to sample.
            float curve_amount = _right_banking_curve->sample(percentage);
            right_banking_offset = Vector3(0, curve_amount, 0);
        }

        // Default width if no curve is set.
        float width = 3.0;

        // Sample width if a width curve is given.
        if (_width_curve.is_valid()) {
            width = _width_curve->sample(percentage);
        }

        left_verticies[i] = t - (left * width / 2.0) + left_banking_offset;
        right_verticies[i] = t + (left * width / 2.0) + right_banking_offset;

        // The bottom verticies don't move up/down with the banking
        float bottom_offset = 1.0; // TODO configurable.
        left_verticies_bottom[i] = t - (left * width / 2.0) - (up * bottom_offset);
        right_verticies_bottom[i] = t + (left * width / 2.0)  - (up * bottom_offset);
    }

    // Build these into triangles
    int verticies_per_interval = 18;
    
    PackedVector3Array verticies;
    PackedVector3Array normals;

    verticies.resize(extrusions * verticies_per_interval);
    normals.resize(extrusions * verticies_per_interval);

    for (int i = 0; i < (extrusions); i++) {
        // Top face top left triangle.
        /**
         *  1 - 2 (i + 1)
         *  | /
         *  3 (i)
        */
        verticies[cumulative_vertex_index] = left_verticies[i + 1];
        verticies[cumulative_vertex_index + 1] = right_verticies[i + 1];
        verticies[cumulative_vertex_index + 2] = left_verticies[i];

        // Make normal from a plane's normal.
        Plane p(verticies[cumulative_vertex_index], verticies[cumulative_vertex_index + 1], verticies[cumulative_vertex_index + 2]);
        normals[cumulative_vertex_index] = p.normal;
        normals[cumulative_vertex_index + 1] = p.normal;
        normals[cumulative_vertex_index + 2] = p.normal;

        cumulative_vertex_index += 3;
        
        // Top face bottom right triangle.
        /**
         *     1 (i + 1)
         *   / |
         * 3 - 2 (i)
        */
        verticies[cumulative_vertex_index] = right_verticies[i + 1];
        verticies[cumulative_vertex_index + 1] = right_verticies[i];
        verticies[cumulative_vertex_index + 2] = left_verticies[i];

        Plane p2(verticies[cumulative_vertex_index], verticies[cumulative_vertex_index + 1], verticies[cumulative_vertex_index + 2]);
        normals[cumulative_vertex_index] = p2.normal;
        normals[cumulative_vertex_index + 1] = p2.normal;
        normals[cumulative_vertex_index + 2] = p2.normal;

        cumulative_vertex_index += 3;

        // Left side top left triangle.
        /**   i + 1  i
         *     /    / 
         *    1 - 2 (top left)
         *    | /
         *    3 (bottom left)
        */
        verticies[cumulative_vertex_index] = left_verticies[i + 1];
        verticies[cumulative_vertex_index + 1] = left_verticies[i];
        verticies[cumulative_vertex_index + 2] = left_verticies_bottom[i + 1];

        Plane p3(verticies[cumulative_vertex_index], verticies[cumulative_vertex_index + 1], verticies[cumulative_vertex_index + 2]);
        normals[cumulative_vertex_index] = p3.normal;
        normals[cumulative_vertex_index + 1] = p3.normal;
        normals[cumulative_vertex_index + 2] = p3.normal;

        cumulative_vertex_index += 3;

         // Left side bottom right triangle.
        /**   i + 1  i
         *      /   / 
         *     :   1 (top left)
         *     : / |
         *     3 - 2 (bottom left)
        */
        verticies[cumulative_vertex_index] = left_verticies[i];
        verticies[cumulative_vertex_index + 1] = left_verticies_bottom[i];
        verticies[cumulative_vertex_index + 2] = left_verticies_bottom[i + 1];

        Plane p4(verticies[cumulative_vertex_index], verticies[cumulative_vertex_index + 1], verticies[cumulative_vertex_index + 2]);
        normals[cumulative_vertex_index] = p4.normal;
        normals[cumulative_vertex_index + 1] = p4.normal;
        normals[cumulative_vertex_index + 2] = p4.normal;

        cumulative_vertex_index += 3;

         // Right side top left triangle.
        /**  i   i + 1
         *     \   \ 
         *     1 - 2 (top right)
         *     | /
         *     3 (bottom right)
        */
        verticies[cumulative_vertex_index] = right_verticies[i];
        verticies[cumulative_vertex_index + 1] = right_verticies[i + 1];
        verticies[cumulative_vertex_index + 2] = right_verticies_bottom[i];

        Plane p5(verticies[cumulative_vertex_index], verticies[cumulative_vertex_index + 1], verticies[cumulative_vertex_index + 2]);
        normals[cumulative_vertex_index] = p5.normal;
        normals[cumulative_vertex_index + 1] = p5.normal;
        normals[cumulative_vertex_index + 2] = p5.normal;

        cumulative_vertex_index += 3;

        // Right side bottom right triangle.
        /**   i  i + 1
         *     \   \ 
         *     :   1 (top right)
         *     : / |
         *     3 - 2 (bottom right)
        */
        verticies[cumulative_vertex_index] = right_verticies[i + 1];
        verticies[cumulative_vertex_index + 1] = right_verticies_bottom[i + 1];
        verticies[cumulative_vertex_index + 2] = right_verticies_bottom[i];

        Plane p6(verticies[cumulative_vertex_index], verticies[cumulative_vertex_index + 1], verticies[cumulative_vertex_index + 2]);
        normals[cumulative_vertex_index] = p6.normal;
        normals[cumulative_vertex_index + 1] = p6.normal;
        normals[cumulative_vertex_index + 2] = p6.normal;

        cumulative_vertex_index += 3;
    }

    Array array;
    array.resize(Mesh::ARRAY_MAX);
    array[Mesh::ARRAY_VERTEX] = verticies;
    array[Mesh::ARRAY_NORMAL] = normals;
   // array[Mesh::ARRAY_TEX_UV] = uvs;

    int idx = root_mesh->get_surface_count();
    root_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, array);
    set_base(root_mesh->get_rid());
}

