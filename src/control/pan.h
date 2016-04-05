#ifndef __PAN_H
#define __PAN_H

#include <linmath.h>

class Pan
{
public:
    Pan(float x = 0.f, float y = 0.f)
    {
        set_dragging(true);
        set_position(0.f, 0.f);
        set_dragging(false);
        set_position(0.f, 0.f);
        set_in_range(0.f, 0.f, 0.f, 0.f);
        set_out_range(0.f, 0.f, 0.f, 0.f);
        _dtx = 0.f;
        _dty = 0.f;
        _tx = x;
        _ty = y;
        _xscale = 1.f;
        _yscale = 1.f;
    }
    void set_dragging(bool dragging)
    {
        bool started_dragging = !_dragging && dragging;
        bool stopped_dragging = _dragging && !dragging;

        if (started_dragging)
        {
            _xi = _x;
            _yi = _y;
        }
        else if (stopped_dragging)
        {
            _tx += _dtx;
            _ty += _dty;
            _dtx = 0.f;
            _dty = 0.f;
        }

        _dragging = dragging;
    }
    void set_position(float x, float y)
    {
        _x = (_out_range[1] - _out_range[0]) * ((x - _in_range[0]) / (_in_range[1] - _in_range[0])) + _out_range[0];
        _y = (_out_range[3] - _out_range[2]) * ((y - _in_range[2]) / (_in_range[3] - _in_range[2])) + _out_range[2];

        if (_dragging)
        {
            _dtx = _x - _xi;
            _dty = _y - _yi;
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
        _out_range[0] = xmin;
        _out_range[1] = xmax;
        _out_range[2] = ymin;
        _out_range[3] = ymax;
    }
    void set_scale(float x, float y)
    {
        _xscale = x;
        _yscale = y;
    }
    void apply(mat4x4 dst)
    {
        mat4x4_translate_in_place(dst, _xscale * (_dtx + _tx), _yscale * (_dty + _ty), 0.f);
    }
private:
    bool _dragging;
    float _xi, _yi, _dtx, _dty, _tx, _ty, _x, _y, _xscale, _yscale;
    float _in_range[4];
    float _out_range[4];
};

#endif  // __PAN_H