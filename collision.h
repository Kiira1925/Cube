#pragma once
#include "vector.h"

//========================================
// �Փ˔���ɂ����ϐ���`
//========================================

// ����
struct Line {
    Point p;
    VECTOR3 v;
    Line() : p(0.0f, 0.0f, 0.0f), v(1.0f, 0.0f, 0.0f) {}
    Line(const Point& p, const VECTOR3& v) : p(p), v(v) {}
    ~Line() {}

    // �_��̍��W���擾
    // �x�N�g���Ɋ|���Z����W��
    Point getPoint(float t)const {
        return p + t * v;
    }
};

// ����
struct Segment : public Line {
    Segment() {}
    Segment(const Point& p, const VECTOR3& v) : Line(p, v) {}
    Segment(const Point& p1, const Point& p2) : Line(p1, p2 - p1) {}

    // �I�_���擾
    FLOAT3 getEndPoint() const {
        return p + v;
    }
};

// ��
struct Sphere {
    Point p;
    float r;
    Sphere() : p(0.0f, 0.0f, 0.0f), r(0.5f) {}
    Sphere(const Point& p, float r) : p(p), r(r) {}
    ~Sphere() {}
};

// �J�v�Z��
struct Capsule {
    Segment s;
    float r;
    Capsule() : r(0.5f) {}
    Capsule(const Segment& s, float r) : s(s), r(r) {}
    Capsule(const Point& p1, const Point& p2, float r) : s(p1, p2), r(r) {}
    ~Capsule() {}
};

// AABB

struct AABB {
    Point p;    // ���S�_
    FLOAT3 hl;  // �e���̕ӂ̒����̔���
    AABB() {}
    AABB(const Point& p, const FLOAT3& hl) :p(p), hl(hl) {}

    // �ӂ̒������擾
    float lenX() const { return hl.x * 2.0f; };
    float lenY() const { return hl.y * 2.0f; };
    float lenZ() const { return hl.z * 2.0f; };
    float len( int i)  {
        return *((&hl.x) + i)* 2.0f; };
};