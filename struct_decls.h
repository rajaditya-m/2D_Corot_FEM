#pragma once

typedef struct point_class
{
	float x;
	float y;
	float z;
}Point_Struct;

typedef struct tri_class
{
	int a;
	int b;
	int c;
}Triangle_Struct;

typedef struct text_class
{
	float u;
	float v;
}Texture_Struct;

typedef struct tri_class_en
{
	int a;
	int b;
	int c;

	int at;
	int bt;
	int ct;
}Triangle_Texture_Struct;

typedef struct spr_conn
{
	int spNum_A;
	int pointNum_A;

	int spNum_B;
	int pointNum_B;

	double stiffness;

} SpringConnections;