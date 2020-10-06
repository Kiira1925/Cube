#pragma once
#include <DirectXMath.h>

#define _OX_EPSILON_ 0.000001f // 誤差

struct FLOAT3 {
    float x, y, z;

    FLOAT3() {}
    FLOAT3(float x, float y, float z) : x(x) , y(y),z(z){}
    ~FLOAT3() {}

    // オペレーター
    // [+]
    FLOAT3 operator +(const FLOAT3& r) const {
        return FLOAT3(x + r.x, y + r.y, z + r.z);
    }

    // [-]
    FLOAT3 operator -(const FLOAT3& r) const {
        return FLOAT3(x - r.x, y - r.y, z - r.z);
    }

    FLOAT3 operator -() const {
        return FLOAT3(x * -1.0f, y * -1.0f, z * -1.0f);
    }

    // [*]
    FLOAT3 operator *(const FLOAT3& r) const {
        return FLOAT3(x * r.x, y * r.y, z * r.z);
    }

    // [/]
    FLOAT3 operator /(const FLOAT3& r) const {
        return FLOAT3(x / r.x, y / r.y, z / r.z);
    }

    // [* float]
    FLOAT3 operator *(float r) const {
        return FLOAT3(x * r, y * r, z * r);
    }

    // [/ float]
    FLOAT3 operator /(float r)const {
        return FLOAT3(x / r, y / r, z / r);
    }

    // [FLOAT3 = float * FLOAT3]
    friend FLOAT3 operator *(float l, const FLOAT3& r){
        return FLOAT3(r.x * l, r.y * l, r.z * l);
    }

    // [FLOAT3 = float / FLOAT3]
    friend FLOAT3 operator /(float l, const FLOAT3& r) {
        return FLOAT3(r.x / l, r.y / l, r.z / l);
    }

    // dot 内積
    float dot(const FLOAT3& r) const {
        return (x * r.x) + (y * r.y) + (z * r.z);
    }

    // cross 外積
    FLOAT3 cross(const FLOAT3& r)const {
        return FLOAT3((y * r.z) - (z * r.y), (z * r.x) - (x * r.z), (x * r.y) - (y * r.x));
    }

    // length 長さの半分
    float length() const {
        return sqrtf(lengthSq());
    }
    
    // lengthSq 長さ
    float lengthSq() const {
        return (x * x) + (y * y) + (z * z);
    }

    // 正規化
    void norm() {
        const float len = length();
        if (len > 0.0f) {
            x /= len;
            y /= len;
            z /= len;
        }
    }

    // 正規化した値を受け取る
    FLOAT3 getNorm() const {
        const float len = length();
        if (len > 0.0f) {
            return FLOAT3(x / len, y / len, z / len);
        }
        else return FLOAT3(0.0f, 0.0f, 0.0f);
    }
};

// 点(位置)
typedef FLOAT3 Point;

// ベクトル(方向)
struct VECTOR3 : public FLOAT3 {
    VECTOR3() {}
    VECTOR3( float x, float y, float z) : FLOAT3(x, y, z) {}
    VECTOR3(const FLOAT3& r) : FLOAT3(r) {}
    ~VECTOR3() {}

    VECTOR3& operator =(const FLOAT3& r) {
        x = r.x;
        y = r.y;
        z = r.z;
        return *this;
    }

    // 標準化(正規化)
    void norm() {
        const float len = length();
        if (len > 0.0f) {
            x /= len;
            y /= len;
            z /= len;
        }
    }

    // 垂直関係にあるか？
    bool isVertical( const VECTOR3& r) const {
        float d = dot(r);
        return (-_OX_EPSILON_ < d&& d < _OX_EPSILON_);
    }

    // 平行関係にあるか？
    bool isParallel(const VECTOR3& r) const {
        float d = cross(r).lengthSq();
        return (-_OX_EPSILON_ < d&& d < _OX_EPSILON_);
    }

    //　鋭角関係にあるか？
    bool isSharpAngle(const VECTOR3& r)const {
        return (dot(r) >= 0.0f);
    }

};


class VECTOR2 : public DirectX::XMFLOAT2
{
public:
    VECTOR2() : DirectX::XMFLOAT2(0, 0) {}
    VECTOR2(float x, float y) : DirectX::XMFLOAT2(x, y) {}
    VECTOR2(const VECTOR2& v) { x = v.x; y = v.y; }
    ~VECTOR2() {}

    VECTOR2& operator=(const VECTOR2&);
    VECTOR2& operator+=(const VECTOR2&);
    VECTOR2& operator-=(const VECTOR2&);
    VECTOR2& operator*=(float);
    VECTOR2& operator/=(float);

    VECTOR2 operator+() const;
    VECTOR2 operator-() const;

    VECTOR2 operator+(const VECTOR2&) const;
    VECTOR2 operator-(const VECTOR2&) const;
    VECTOR2 operator*(float) const;
    friend VECTOR2 operator*(float, const VECTOR2&);
    VECTOR2 operator/(float) const;

    bool operator == (const VECTOR2&) const;
    bool operator != (const VECTOR2&) const;
};

//------< プロトタイプ宣言 >-----------------------------------------------------

float vec2LengthSq(const VECTOR2&);
float vec2Length(const VECTOR2&);
VECTOR2* vec2Normalize(const VECTOR2&, VECTOR2*);

// VECTOR3型
//class VECTOR3 : public DirectX::XMFLOAT3 {
//public:
//    VECTOR3() : DirectX::XMFLOAT3(0, 0, 0) {}
//    VECTOR3(float x, float y, float z) : DirectX::XMFLOAT3(x, y, z) {}
//    VECTOR3(const VECTOR3& v) { x = v.x; y = v.y; z = v.z; }
//    ~VECTOR3() {}
//
//    VECTOR3& operator=(const VECTOR3&);
//    VECTOR3& operator+=(const VECTOR3&);
//    VECTOR3& operator-=(const VECTOR3&);
//    VECTOR3& operator*=(float);
//    VECTOR3& operator/=(float);
//
//    VECTOR3 operator+() const;
//    VECTOR3 operator-() const;
//
//    VECTOR3 operator+(const VECTOR3&) const;
//    VECTOR3 operator-(const VECTOR3&) const;
//    VECTOR3 operator*(float) const;
//    friend VECTOR3 operator*(float, const VECTOR3&);
//    VECTOR3 operator/(float) const;
//
//    bool operator == (const VECTOR3&) const;
//    bool operator != (const VECTOR3&) const;
//
//    VECTOR3(const VECTOR2& v);
//    // VECTOR3にVECTOR2を代入
//    VECTOR3& operator=(const VECTOR2&);
//};
//float Vec3Length(const VECTOR3&);
//void  Vec3Nomalize(VECTOR3&);
//float Vec3Dot(VECTOR3*, VECTOR3*);
//void  Vec3Cross(VECTOR3*,VECTOR3*,VECTOR3*);
//******************************************************************************

class VECTOR4 : public DirectX::XMFLOAT4
{
public:
    VECTOR4() : DirectX::XMFLOAT4(0, 0, 0, 0) {}
    VECTOR4(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w) {}
    ~VECTOR4() {}
};
