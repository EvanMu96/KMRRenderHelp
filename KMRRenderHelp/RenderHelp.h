// 
// 学习 skywind3000 RenderHelp 项目的个人项目
// Features:
// - 单个头文件的渲染器实现，没有任何依赖
// - 模型标准，计算精确，类 Direct3D 接口
// - 包含一套精简何理的矢量/矩阵库
// - 包含一套位图 Bitmap 库，方便画点/画线，加载纹理，保存渲染结果
// - 支持二次线性插值纹理采样器
// - 支持深度缓存
// - 支持多种数据类型的 varying
// - 支持顶点着色器 (Vertex Shader) 和像素着色器 (Pixel Shader)
// - 支持加载 24 位和 32 位的 bmp 图片纹理


#ifndef _RENDER_HELP_H_
#define _RENDER_HLEP_H_

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <vector>
#include <map>
#include <initializer_list>
#include <stdexcept>
#include <functional>
#include <ostream>
#include <sstream>
#include <iostream>

//---------------------------------------------------------------------
// 数学库：Vector 类型定义
//---------------------------------------------------------------------

// 通用 N 维，数据类型是 T 的 Vector
template <size_t N, typename T> struct Vector {
	T m[N];
	inline Vector() {
		for (size_t i = 0; i < N; i++) {
			m[i] = T();
		}
	}
	inline Vector(const T* ptr) {
		for (size_t i = 0; i < N; i++) {
			m[i] = ptr[i];
		}
	}
	inline Vector(const Vector<N, T>& u) {
		for (size_t i = 0; i < N; i++) {
			m[i] = u[i];
		}
	}
	inline Vector(const std::initializer_list<T>& u) {
		auto it = u.begin();
		for (size_t i = 0; i < N; i++) {
			m[i] = *it++;
		}
	}
	inline const T& operator[] (size_t i) const {
		assert(i < N);
		return m[i];
	}
	inline void load(const T* ptr) {
		for (size_t i = 0; i < N; i++) {
			m[i] = ptr[i];
		}
	}
	inline void save(const T* ptr) {
		for (size_t i = 0; i < N; i++) {
			ptr[i] = m[i];
		}
	}
};

// 特化 2-d Vector
template <typename T> struct Vector<2, T> {
	union
	{
		struct { T x, y; };
		struct { T u, v; };
		T m[2];
	};
	inline Vector() : x(T()), y(T()) {}
	inline Vector(T X, T Y) : x(X), y(Y) {}
	inline Vector(const Vector<2, T>& u) : x(u.x), y(u.y) {}
	inline Vector(const T* ptr) : x(ptr[0]), u(ptr[1]) {}
	inline const T& operator[] (size_t i) {
		assert(i < 2);
		return m[i];
	}
	inline void load(const T* ptr) {
		for (size_t i = 0; i < 2; i++) m[i] = ptr[i];
	}
	inline void save(const T* ptr) {
		for (size_t i = 0; i < 2; i++) ptr[i] = m[i];
	}
	inline Vector<2, T> xy() const { return *this; }
	inline Vector<3, T> xy1() const { return Vector<3, T>(x, y, 1); }
};

// 特化 3-d vector
template <typename T> struct Vector<3, T> {
	union 
	{
		struct { T x, y, z; };
		struct { T r, g, b; };
		T m[3];
	};
	inline Vector(): x(T()), y(T()), z(T()) {}
	inline Vector(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
	inline Vector(const Vector<3, T> &u): x(u.x), y(u.y), z(u.z) {}
	inline Vector(const T* ptr) : x(ptr[0]), y(ptr[1]), z(ptr[2]) {}
	inline const T& operator[] (size_t i) const {
		assert(i < 3);
		return m[i];
	}
	inline void load(const T *ptr) { for (size_t i = 0; i < 3; i++) m[i] = ptr[i]; }
	inline void save(T *ptr) { for (size_t i = 0; i < 3; i++) ptr[i] = m[i]; }
	inline Vector<2, T> xy() const { return Vector<2, T>(x, y); }
	inline Vector<3, T> xyz() const { return *this; }
	inline Vector<4, T> xyz1() const { return Vector<4, T>(x, y, z, 1); }
};

// 特化 4-d Vector
template <typename T> struct Vector<4, T> {
	union 
	{
		struct { T x, y, z, w;  };
		struct { T r, g, b, a; };
		T m[4];
	};
	inline Vector() : x(T()), y(T()), z(T()), w(T()) {}
	inline Vector(T X, T Y, T Z, T W): x(X), y(Y), z(Z), w(W) {}
	inline Vector(const Vector<4, T> &u): x(u.x), y(u.y), z(u.z), w(u.w) {}
	inline Vector(const T* ptr): x(ptr[0]), y(ptr[1]), z(ptr[2]), w(ptr[3]) {}
	inline const T& operator[] (size_t i) const {
		assert(i < 4);
		return m[i];
	}
	inline void load(const T *ptr) { for (size_t i = 0; i < 4; i++) m[i] = ptr[i]; }
	inline void save(T *ptr) { for (size_t i = 0; i < 4; i++) ptr[i] = m[i]; }
	inline Vector<2, T> xy() const { return Vector<2, T>(x, y); }
	inline Vector<3, T> xyz() const { return Vector<3, T>(x, y, z); }
	inline Vector<4, T> xyzw() const { return *this; }
};

//---------------------------------------------------------------------
// 数学库：Vector util 函数
//---------------------------------------------------------------------

template <size_t N, typename T> 
inline Vector<N, T> operator + (const Vector<N, T>& a) {
	return a;
}


template <size_t N, typename T> 
inline Vector<N, T> operator - (const Vector<N, T>& a) {
	Vector<N, T> b;
	for (size_t i = 0; i < N; i++) {
		b[i] = -a[i];
	}
	return b;
}

template <size_t N, typename T>
inline bool operator == (const Vector<N, T>& a, const Vector<N, T>& b) {
	for (size_t i = 0; i < N; i++) {
		if (a[i] != b[i])
			return false;
	}
	return true;
}

template <size_t N, typename T>
inline bool operator != (const Vector<N, T>& a, const Vector<N, T>& b) {
	return !(a == b);
}


// 数学库：Matrix 类型定义

// 数学库：Matrix util 函数

// 数学库：行列式和逆矩阵，光照计算有用

// util 函数

// 3d 数学运算

#endif // _RENDER_HELP_H_

