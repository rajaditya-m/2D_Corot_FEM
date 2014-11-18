#pragma once

#include <Eigen\Dense>

class Triangles
{
public:
	Triangles(void);
	Triangles(int _a, int _b, int _c);
	~Triangles(void);
	void add(int a,int b,int c);
	void add_textures(int a,int b,int c);
	void add_lmt_value(int x,int y);
	void addTriangleNormal(Eigen::Vector3d normal)			{ normal_ = normal;			}
	Eigen::Vector3d getTriangleNormal()						{ return normal_;			}
	int operator[](int x);

public:
	int a;
	int b;
	int c;
	int at;
	int bt;
	int ct;
	Eigen::Vector3d normal_;
	int lmt_x;
	int lmt_y;
};

