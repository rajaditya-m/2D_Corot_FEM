#pragma once 

#include <Eigen\Dense>
#include "boost\multi_array.hpp"
#include <vector>

typedef Eigen::Matrix<float,7,1> Vector7d;
typedef Eigen::Matrix<float,3,2> Matrix3x2;

typedef boost::multi_array<Eigen::Vector3d,2> Array2d_Vec3d;

typedef std::vector< std::vector<Eigen::Vector3d> > Vector2d_Vec3d;

typedef std::vector<Eigen::Vector2f> Vector_Vec2f;


typedef std::pair< Eigen::Vector3d, Eigen::Vector3d> Pair_Vec3d; 
typedef std::vector<Pair_Vec3d> Vector_PairVec3d;

typedef Eigen::Matrix<double,6,1> Vector6d;

typedef Eigen::Vector3d Voxel;

typedef enum RMode {WIREFRAME, SHADING, HMAP_VELOCITY,HMAP_ACCLRN,HMAP_SHEAR_FORCE,HMAP_BENDING_FORCE,HMAP_GRAVITY_FORCE,HMAP_DAMPING_FORCE} RenderMode;
typedef enum ScId {UNDEFINED,CLOTH,COLL_OBJ}  SceneObjectId;
typedef enum ScT {TWO_DIM,THREE_DIM} SceneType; 

typedef enum SpT {RADIAL,CROSS_SECTIONAL,CIRCUMFERAL} SpringType;