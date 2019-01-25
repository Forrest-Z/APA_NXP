/*****************************************************************************/
/* FILE NAME: algebraic_geometry.cpp              COPYRIGHT (c) Motovis 2019 */
/*                                                       All Rights Reserved */
/* DESCRIPTION: the algebraic geometry aclculate method  			         */
/*****************************************************************************/
/* REV      AUTHOR        DATE              DESCRIPTION OF CHANGE            */
/* ---   -----------    ----------------    ---------------------            */
/* 1.0	 Guohua Zhu     January 16 2019      Initial Version                 */
/*****************************************************************************/
#include "algebraic_geometry.h"

AlgebraicGeometry::AlgebraicGeometry() {


}

AlgebraicGeometry::~AlgebraicGeometry() {

}

//���η������
float AlgebraicGeometry::QuadraticEquation(float a,float b,float c)
{
	return 0.5 * (-b + sqrtf(b*b - 4*a*c)) / a;
}
//��һ���ֱ�߷���
float AlgebraicGeometry::LinearAlgebra(Line l,float x)
{
	return tanf(l.Angle)*(x - l.Point.getX()) + l.Point.getY();
}

float AlgebraicGeometry::ArcLength(Vector2d a,Vector2d b,float r)
{
	return 2 * asinf((a-b).Length()*0.5/r)*r;
}


//��֪Բ�İ뾶������Բ��ֱ������Բ������λ��
void AlgebraicGeometry::Tangent_CCL(Line l,Circle cl,Circle *cr)
{
	float a,b,c;
	float sec_v,tan_v;
	float Value_temp;
	sec_v = 1/cosf(l.Angle);
	tan_v = tanf(l.Angle);

	Value_temp = l.Point.getY() - cl.Center.getY() - tan_v*l.Point.getX() - sec_v*cr->Radius;

	a = 1 + tan_v * tan_v;
	b = 2*(tan_v * Value_temp - cl.Center.getX());
	c = powf(Value_temp,2) - powf(cl.Radius + cr->Radius ,2);

	cr->Center.X = QuadraticEquation(a,b,c);

	cr->Center.Y = LinearAlgebra(l,cr->Center.getX()) - sec_v*cr->Radius;
}

// ����ֱ������Բ�İ뾶
void AlgebraicGeometry::Tangent_CL(Line l,Circle *c,Vector2d *p)
{
	Vector2d temp_difference;
	*p = (c->Center-l.Point).Orthogonal(l.Angle) + l.Point;

	temp_difference = c->Center - l.Point;
	c->Radius = fabsf(sinf(l.Angle)*temp_difference.getX() - cosf(l.Angle)*temp_difference.getY());
}

// ��֪��Բ��λ�ã�������Բ����ֱ�ߵ��е�λ��
void AlgebraicGeometry::Tangent_CLC(Circle cl,Circle cr,Line *lm,Vector2d *ll,Vector2d *lr)
{
	float K1;
	float d_cl_lm;
	float alpha,beta;
	Vector2d v_ll_lm,v_lr_lm;

	K1 = cr.Radius / (cl.Radius + cr.Radius);
	lm->Point = (cl.Center - cr.Center)*K1 + cr.Center;

	d_cl_lm = (lm->Point - cl.Center).Length();
	alpha = (lm->Point - cl.Center).Angle();
	beta  = acosf(cl.Radius / d_cl_lm);
	lm->Angle = alpha - beta + PI_2;

	v_ll_lm = Vector2d(-cl.Radius * tanf(beta) ,0);
	v_lr_lm = Vector2d( cr.Radius * tanf(beta) ,0);

	*ll = lm->Point + v_ll_lm.rotate(lm->Angle);
	*lr = lm->Point + v_lr_lm.rotate(lm->Angle);
}
