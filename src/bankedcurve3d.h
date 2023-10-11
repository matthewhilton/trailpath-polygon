#ifndef BANKEDCURVE3D_H
#define BANKEDCURVE3D_H

#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/curve.hpp>
#include <godot_cpp/variant/dictionary.hpp>

namespace godot {

class BankedCurve3D : public Curve3D {
    GDCLASS(BankedCurve3D, Curve3D)

private:
    Curve banking_curve = Curve();
    float max_height = 10;
    float _offset_to_bank_curve_offset(float offset);
protected:
    static void _bind_methods();

public:
    ~BankedCurve3D();

    bool autobake = true;
    
    void set_autobake(bool v);
    bool get_autobake();

    Dictionary offset_height_values = Dictionary();

    Dictionary get_offset_height_values();
    void set_offset_height_values(Dictionary v);

    void bake_curve();
    Ref<BankedCurve3D> duplicate_banked();

    float get_bank_at_offset(float offset);
};

}

#endif