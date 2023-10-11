#include "bankedcurve3d.h"
#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/ref.hpp>

using namespace godot;

void BankedCurve3D::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_offset_height_values", "offset_height_values"), &BankedCurve3D::set_offset_height_values);
	ClassDB::bind_method(D_METHOD("get_offset_height_values"), &BankedCurve3D::get_offset_height_values);

    ClassDB::bind_method(D_METHOD("bake_curve"), &BankedCurve3D::bake_curve);
    ClassDB::bind_method(D_METHOD("duplicate_banked"), &BankedCurve3D::duplicate_banked);
    ClassDB::bind_method(D_METHOD("get_offset_height_values"), &BankedCurve3D::get_offset_height_values);
    ClassDB::bind_method(D_METHOD("set_offset_height_values"), &BankedCurve3D::set_offset_height_values);

    ClassDB::bind_method(D_METHOD("set_autobake"), &BankedCurve3D::set_autobake);
    ClassDB::bind_method(D_METHOD("get_autobake"), &BankedCurve3D::get_autobake);

    ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "offset_height_values"), "set_offset_height_values", "get_offset_height_values");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "autobake"), "set_autobake", "get_autobake");
}

void BankedCurve3D::set_offset_height_values(Dictionary v) {
    offset_height_values = v;

    // May want this off for performance reasons.
    if(autobake) {
        bake_curve();
    }
}

Dictionary BankedCurve3D::get_offset_height_values() {
    return offset_height_values;
}

void BankedCurve3D::set_autobake(bool v) {
    autobake = v;
}

bool BankedCurve3D::get_autobake() {
    return autobake;
}

float BankedCurve3D::get_bank_at_offset(float offset) {
    return banking_curve.sample(_offset_to_bank_curve_offset(offset));
}

float BankedCurve3D::_offset_to_bank_curve_offset(float offset) {
    // Curve only goes between 0 -> 1, which is just a percentage.
    return offset / get_baked_length();
}

void BankedCurve3D::bake_curve() {
    banking_curve.set_max_value(max_height);
    banking_curve.set_min_value(-max_height);

    banking_curve.clear_points();

    Array keys = offset_height_values.keys();
    Array values = offset_height_values.values();

    for(int i = 0; i < keys.size(); i++) {
        float offset = keys[i];
        float height = values[i];
        banking_curve.add_point(Vector2(_offset_to_bank_curve_offset(offset), height));
    }

    // Triggers changed signal, which triggers Path3D curve_changed.
    emit_changed();
}

Ref<BankedCurve3D> BankedCurve3D::duplicate_banked() {
    Ref<BankedCurve3D> c_new = duplicate();
    c_new->offset_height_values = offset_height_values.duplicate();
    c_new->max_height = max_height;

    return c_new;
}

BankedCurve3D::~BankedCurve3D() {
    // Add your cleanup here.
}