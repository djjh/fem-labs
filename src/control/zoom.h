#ifndef __ZOOM_H
#define __ZOOM_H

#include <linmath.h>

class Zoom
{
public:
    Zoom(float z = 1.f)
    {
        set_dragging(true);
        set_position(0.f, 0.f);
        set_dragging(false);
        set_position(0.f, 0.f);
        set_in_range(0.f, 0.f, 0.f, 0.f);
        _dzx = 1.f;
        _dzy = 1.f;
        _zx = z;
        _zy = z;
    }
    void set_dragging(bool dragging)
    {
        bool started_dragging = !_dragging && dragging;
        bool stopped_dragging = _dragging && !dragging;

        if (started_dragging)
        {
            // just started dragging
            _xi = _x;
            _yi = _y;
        }
        else if (stopped_dragging)
        {
            // just stopped dragging
            _zx *= _dzx;
            _zy *= _dzy;
            _dzx = 1.f;
            _dzy = 1.f;
        }

        _dragging = dragging;
    }
    void set_position(float x, float y)
    {
        _x = ((x - _in_range[0]) / (_in_range[1] - _in_range[0]));
        _y = ((y - _in_range[2]) / (_in_range[3] - _in_range[2]));

        if (_dragging)
        {
            _dzx = (_x - _xi) + 1.f;
            _dzy = (_y - _yi) + 1.f;
        }
    }
    void set_in_range(float xmin, float xmax, float ymin, float ymax)
    {
        _in_range[0] = xmin;
        _in_range[1] = xmax;
        _in_range[2] = ymin;
        _in_range[3] = ymax;
    }
    void set_out_range(float xmin, float xmax, float ymin, float ymax)
    {
        
    }
    float get_pixels_per_unit_x()
    {
        return _zx * _dzx;
    }
    float get_pixels_per_unit_y()
    {
        return _zy * _dzy;
    }
    void apply(mat4x4 dst)
    {
        mat4x4_scale_aniso(dst, dst, get_pixels_per_unit_x(), get_pixels_per_unit_y(), 1.f);
    }
private:
    bool _dragging;
    float _xi, _yi, _dzx, _dzy, _zx, _zy, _x, _y;
    float _in_range[4];
    float _out_range[4];
};

#endif  // __ZOOM_H