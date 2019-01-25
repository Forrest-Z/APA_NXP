/*****************************************************************************/
/* FILE NAME: algebraic_geometry.h                COPYRIGHT (c) Motovis 2019 */
/*                                                       All Rights Reserved */
/* DESCRIPTION: the algebraic geometry aclculate method  			         */
/*****************************************************************************/
/* REV      AUTHOR        DATE              DESCRIPTION OF CHANGE            */
/* ---   -----------    ----------------    ---------------------            */
/* 1.0	 Guohua Zhu     January 16 2019      Initial Version                 */
/*****************************************************************************/

#ifndef MATH_ALGEBRAIC_GEOMETRY_H_
#define MATH_ALGEBRAIC_GEOMETRY_H_

#include "derivative.h"
#include "property.h"
#include "vector_2d.h"
#include "math.h"
#include "chang_an_configure.h"
#include "common_configure.h"

typedef struct _Line
{
	Vector2d Point;
	float Angle;
}Line;

typedef struct _Circle
{
	Vector2d Center;
	float Radius;
}Circle;

class AlgebraicGeometry {
public:
	AlgebraicGeometry();
	virtual ~AlgebraicGeometry();

	//���η������
	float QuadraticEquation(float a,float b,float c);

	//��һ���ֱ�߷���
	float LinearAlgebra(Line l,float x);

	// ��֪Բ������Ͱ뾶���������Ļ���
	float ArcLength(Vector2d a,Vector2d b,float r);
	//��֪Բ�İ뾶������Բ��ֱ������Բ������λ��
	void Tangent_CCL(Line l,Circle cl,Circle *cr);
	// ��֪�߶κ�Բ�ģ�������Բ�İ뾶
	void Tangent_CL(Line l,Circle *c,Vector2d *p);
	// ��֪��Բ��λ�ã�������Բ����ֱ�ߵ��е�λ��
	void Tangent_CLC(Circle cl,Circle cr,Line *lm,Vector2d *ll,Vector2d *lr);
private:

};

#endif /* MATH_ALGEBRAIC_GEOMETRY_H_ */
