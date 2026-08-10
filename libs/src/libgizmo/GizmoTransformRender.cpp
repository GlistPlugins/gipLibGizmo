///////////////////////////////////////////////////////////////////////////////////////////////////
// LibGizmo
// File Name :
// Creation : 10/01/2012
// Author : Cedric Guillemet
// Description : LibGizmo
//
///Copyright (C) 2012 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//of the Software, and to permit persons to whom the Software is furnished to do
///so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
// NOTE (gipLibgizmo): this file has been adapted to draw through GlistEngine's
// gRenderer (via gipLibgizmoPrimitive3D) instead of legacy fixed-function OpenGL
// (glBegin/glVertex/...), which GlistEngine's core-profile GL / Vulkan context
// does not support. Only the drawing backend changed; the geometry/math of every
// shape below is unchanged from upstream LibGizmo.
//

#include "stdafx.h"
#include "GizmoTransformRender.h"
#include "gipLibgizmoPrimitive3D.h"

namespace {
	gipLibgizmoPrimitive3D& gizmoPrimitive() {
		static gipLibgizmoPrimitive3D primitive;
		return primitive;
	}

	inline glm::vec3 toGlm(const tvector3& v) {
		return glm::vec3(v.x, v.y, v.z);
	}
}

void CGizmoTransformRender::DrawCircle(const tvector3 &orig,float r,float g,float b,const tvector3 &vtx,const tvector3 &vty)
{
	std::vector<glm::vec3> points;
	points.reserve(50);
	for (int i = 0; i < 50 ; i++)
	{
		tvector3 vt;
		vt = vtx * cos((2*ZPI/50)*i);
		vt += vty * sin((2*ZPI/50)*i);
		vt += orig;
		points.push_back(toGlm(vt));
	}
	gizmoPrimitive().draw(points, gMesh::DRAWMODE_LINELOOP, glm::vec3(r, g, b));
}


void CGizmoTransformRender::DrawCircleHalf(const tvector3 &orig,float r,float g,float b,const tvector3 &vtx,const tvector3 &vty,tplane &camPlan)
{
	std::vector<glm::vec3> points;
	for (int i = 0; i < 30 ; i++)
	{
		tvector3 vt;
		vt = vtx * cos((ZPI/30)*i);
		vt += vty * sin((ZPI/30)*i);
		vt +=orig;
		if (camPlan.DotNormal(vt))
			points.push_back(toGlm(vt));
	}
	gizmoPrimitive().draw(points, gMesh::DRAWMODE_LINESTRIP, glm::vec3(r, g, b));
}

void CGizmoTransformRender::DrawAxis(const tvector3 &orig, const tvector3 &axis, const tvector3 &vtx,const tvector3 &vty, float fct,float fct2,const tvector4 &col)
{
	glm::vec3 color(col.x, col.y, col.z);
	tvector3 tip(orig.x+axis.x, orig.y+axis.y, orig.z+axis.z);

	std::vector<glm::vec3> line = { toGlm(orig), toGlm(tip) };
	gizmoPrimitive().draw(line, gMesh::DRAWMODE_LINES, color);

	// NOTE: upstream emits 3 vertices per loop iteration (rim, rim, apex) into what
	// it labels a GL_TRIANGLE_FAN. That is not real fan winding (only vertex 0 would
	// stay pivot), but every 3 consecutive vertices already form one correct cone
	// triangle - so GL_TRIANGLES over the same vertex stream reproduces the same
	// visual shape.
	std::vector<glm::vec3> cone;
	cone.reserve(31 * 3);
	for (int i=0;i<=30;i++)
	{
		tvector3 pt;
		pt = vtx * cos(((2*ZPI)/30.0f)*i)*fct;
		pt+= vty * sin(((2*ZPI)/30.0f)*i)*fct;
		pt+=axis*fct2;
		pt+=orig;
		cone.push_back(toGlm(pt));

		pt = vtx * cos(((2*ZPI)/30.0f)*(i+1))*fct;
		pt+= vty * sin(((2*ZPI)/30.0f)*(i+1))*fct;
		pt+=axis*fct2;
		pt+=orig;
		cone.push_back(toGlm(pt));

		cone.push_back(toGlm(tip));
	}
	gizmoPrimitive().draw(cone, gMesh::DRAWMODE_TRIANGLES, color);
}

void CGizmoTransformRender::DrawCamem(const tvector3& orig,const tvector3& vtx,const tvector3& vty,float ng)
{
	std::vector<glm::vec3> fan;
	fan.reserve(52);
	fan.push_back(toGlm(orig));
	for (int i = 0 ; i <= 50 ; i++)
	{
		tvector3 vt;
		vt = vtx * cos(((ng)/50)*i);
		vt += vty * sin(((ng)/50)*i);
		vt+=orig;
		fan.push_back(toGlm(vt));
	}
	gizmoPrimitive().draw(fan, gMesh::DRAWMODE_TRIANGLEFAN, glm::vec3(1,1,0), true);

	std::vector<glm::vec3> outline;
	outline.reserve(52);
	outline.push_back(toGlm(orig));
	for (int i = 0 ; i <= 50 ; i++)
	{
		tvector3 vt;
		vt = vtx * cos(((ng)/50)*i);
		vt += vty * sin(((ng)/50)*i);
		vt+=orig;
		outline.push_back(toGlm(vt));
	}
	gizmoPrimitive().draw(outline, gMesh::DRAWMODE_LINELOOP, glm::vec3(1,1,0.2f));
}

void CGizmoTransformRender::DrawQuad(const tvector3& orig, float size, bool bSelected, const tvector3& axisU, const tvector3 &axisV)
{
	tvector3 pts[4];
	pts[0] = orig;
	pts[1] = orig + (axisU * size);
	pts[2] = orig + (axisU + axisV)*size;
	pts[3] = orig + (axisV * size);

	std::vector<glm::vec3> quad = { toGlm(pts[0]), toGlm(pts[1]), toGlm(pts[2]), toGlm(pts[3]) };

	glm::vec3 fillcolor = bSelected ? glm::vec3(1,1,1) : glm::vec3(1,1,0);
	gizmoPrimitive().draw(quad, gMesh::DRAWMODE_TRIANGLEFAN, fillcolor, true);

	glm::vec3 outlinecolor = bSelected ? glm::vec3(1,1,1) : glm::vec3(1,1,0.2f);
	gizmoPrimitive().draw(quad, gMesh::DRAWMODE_LINESTRIP, outlinecolor);
}


void CGizmoTransformRender::DrawTri(const tvector3& orig, float size, bool bSelected, const tvector3& axisU, const tvector3& axisV)
{
	tvector3 pts[3];
	pts[0] = orig;

	pts[1] = (axisU );
	pts[2] = (axisV );

	pts[1]*=size;
	pts[2]*=size;
	pts[1]+=orig;
	pts[2]+=orig;

	std::vector<glm::vec3> tri = { toGlm(pts[0]), toGlm(pts[1]), toGlm(pts[2]) };

	glm::vec3 fillcolor = bSelected ? glm::vec3(1,1,1) : glm::vec3(1,1,0);
	gizmoPrimitive().draw(tri, gMesh::DRAWMODE_TRIANGLES, fillcolor, true);

	glm::vec3 outlinecolor = bSelected ? glm::vec3(1,1,1) : glm::vec3(1,1,0.2f);
	gizmoPrimitive().draw(tri, gMesh::DRAWMODE_LINESTRIP, outlinecolor);
}
