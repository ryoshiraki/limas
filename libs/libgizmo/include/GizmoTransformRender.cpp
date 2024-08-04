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



//#include "stdafx.h"
#include "GizmoTransformRender.h"
//#ifdef MAC_OS
//#import <OpenGL/OpenGL.h>
//#else
//#include <GL/gl.h>
//#endif
#include "gl/Renderer.h"
#include "geom/GeomUtils.h"

void CGizmoTransformRender::DrawCircle(const tvector3& orig,float r,float g,float b,const tvector3& vtx,const tvector3& vty) {
  
  rs::gl::pushStyle();
  rs::gl::setBlendMode(rs::gl::BLEND_ALPHA);
  rs::gl::disableDepth();

  rs::gl::begin(GL_LINE_LOOP);
  glm::vec3 torig(orig.x, orig.y, orig.z );
  glm::vec3 tvtx(vtx.x, vtx.y, vtx.z );
  glm::vec3 tvty(vty.x, vty.y, vty.z );
  glm::vec4 tcolor(r,g,b,1);
  for (int i = 0; i < 50 ; i++) {
    glm::vec3 vt;
    vt = tvtx * cos((2*ZPI/50)*i);
    vt += tvty * sin((2*ZPI/50)*i);
    vt += torig;
    rs::gl::addColor(tcolor);
    rs::gl::addVertex(vt);
  }
  rs::gl::end();
  
  rs::gl::popStyle();
}

void CGizmoTransformRender::DrawCircleHalf(const tvector3& orig, float r, float g, float b,
                                           const tvector3& vtx, const tvector3& vty, tplane &camPlan) {
  rs::gl::pushStyle();
  rs::gl::setBlendMode(rs::gl::BLEND_ALPHA);
  rs::gl::disableDepth();

  rs::gl::begin(GL_LINE_STRIP);
  glm::vec4 tcolor(r,g,b,1);
  for (int i = 0; i < 30; i++) {
    tvector3 vt;
    vt = vtx * cos((ZPI/30)*i);
    vt += vty * sin((ZPI/30)*i);
    vt +=orig;
    if (camPlan.DotNormal(vt)) {
      rs::gl::addColor(tcolor);
      rs::gl::addVertex(vt.x, vt.y, vt.z);
    }
  }
  rs::gl::end();
  
  rs::gl::popStyle();
}

void CGizmoTransformRender::DrawAxis(const tvector3& orig, const tvector3& axis, const tvector3& vtx,const tvector3& vty,
                                     float fct, float fct2, const tvector4& col) {
  
  rs::gl::pushStyle();
  rs::gl::disableDepth();

  glm::vec4 tcolor(col.x, col.y, col.z, col.w);
  glm::vec3 start(orig.x, orig.y, orig.z);
  glm::vec3 end(orig.x+axis.x,orig.y+axis.y,orig.z+axis.z);
  
  glm::quat quat = rs::geom::getRotationBetweenVectors(glm::vec3(0, 1, 0),
                                                       glm::vec3(axis.x, axis.y, axis.z));
  glm::mat4 tmat = rs::math::toMat(quat);
  
  rs::gl::setFill(true);
  
  rs::gl::pushMatrix();
  rs::gl::translate(start);
  rs::gl::multMatrix(tmat);
  rs::gl::setColor(tcolor);
  rs::gl::drawCylinder(glm::vec3(0), fct2 * 0.5f, glm::distance(start, end));
  rs::gl::popMatrix();

  rs::gl::pushMatrix();
  rs::gl::translate(end);
  rs::gl::multMatrix(tmat);
  rs::gl::setColor(tcolor);
  rs::gl::drawCone(glm::vec3(0), fct2*3., fct2 * 6.13 );
  rs::gl::popMatrix();
  
  rs::gl::popStyle();
}

void CGizmoTransformRender::DrawCamem(const tvector3& orig, const tvector3& vtx, const tvector3& vty, float ng) {
  
  rs::gl::pushStyle();
  rs::gl::setBlendMode(rs::gl::BLEND_ALPHA);
  rs::gl::disableDepth();
  rs::gl::disableFaceCulling();
  
  glm::vec4 tcolor(1,1,0,0.5f);
  glm::vec3 torig(orig.x, orig.y, orig.z);
  glm::vec3 tvtx(vtx.x, vtx.y, vtx.z );
  glm::vec3 tvty(vty.x, vty.y, vty.z );
  
  rs::gl::begin(GL_TRIANGLE_FAN);
  rs::gl::addVertex(torig);
  for (int i = 0 ; i <= 50 ; i++) {
    glm::vec3 vt;
    vt = tvtx * cos(((ng)/50)*i);
    vt += tvty * sin(((ng)/50)*i);
    vt += torig;
    rs::gl::addColor(tcolor);
    rs::gl::addVertex(vt);
  }
  rs::gl::end();
  
  tcolor = glm::vec4(1,1,0.2f,1);
  rs::gl::begin(GL_LINE_LOOP);
  rs::gl::addVertex(torig);
  for (int i = 0 ; i <= 50; i++) {
    glm::vec3 vt;
    vt = tvtx * cos(((ng)/50)*i);
    vt += tvty * sin(((ng)/50)*i);
    vt += torig;
    rs::gl::addColor(tcolor);
    rs::gl::addVertex(vt);
  }
  rs::gl::end();
  
  rs::gl::popStyle();
}

void CGizmoTransformRender::DrawQuad(const tvector3& orig, float size, bool bSelected,
                                     const tvector3& axisU, const tvector3& axisV) {
  
  rs::gl::pushStyle();
  rs::gl::setBlendMode(rs::gl::BLEND_ALPHA);
  rs::gl::disableDepth();
  rs::gl::disableFaceCulling();

  glm::vec3 pts[4];
  pts[0] = glm::vec3(orig.x, orig.y, orig.z );
  pts[1] = pts[0] + (glm::vec3(axisU.x, axisU.y, axisU.z) * size);
  pts[2] = pts[0] + (glm::vec3(axisU.x, axisU.y, axisU.z) + glm::vec3(axisV.x, axisV.y, axisV.z))*size;
  pts[3] = pts[0] + (glm::vec3(axisV.x, axisV.y, axisV.z) * size);
  
  glm::vec4 tcolor(1,1,1,0.6f);
  if (!bSelected)
    tcolor = glm::vec4(1,1,0,0.5f);
  
  rs::gl::begin(GL_TRIANGLES);
  rs::gl::addVertex( pts[0] ); rs::gl::addColor(tcolor);
  rs::gl::addVertex( pts[1] ); rs::gl::addColor(tcolor);
  rs::gl::addVertex( pts[2] ); rs::gl::addColor(tcolor);
  rs::gl::addVertex( pts[0] ); rs::gl::addColor(tcolor);
  rs::gl::addVertex( pts[2] ); rs::gl::addColor(tcolor);
  rs::gl::addVertex( pts[3] ); rs::gl::addColor(tcolor);
  rs::gl::end();
  
  if (!bSelected)
    tcolor = glm::vec4(1,1,0.2f,1);
  else
    tcolor = glm::vec4(1,1,1,0.6f);
  
  rs::gl::begin(GL_LINE_STRIP);
  rs::gl::addVertex( pts[0] ); rs::gl::addColor(tcolor);
  rs::gl::addVertex( pts[1] ); rs::gl::addColor(tcolor);
  rs::gl::addVertex( pts[2] ); rs::gl::addColor(tcolor);
  rs::gl::addVertex( pts[3] ); rs::gl::addColor(tcolor);
  rs::gl::addVertex( pts[0] ); rs::gl::addColor(tcolor);
  rs::gl::end();
  
  rs::gl::popStyle();
}


void CGizmoTransformRender::DrawTri(const tvector3& orig, float size, bool b_selected,
                                    const tvector3& axisU, const tvector3& axisV) {

  rs::gl::pushStyle();
  rs::gl::setBlendMode(rs::gl::BLEND_ALPHA);
  rs::gl::disableDepth();
  rs::gl::disableFaceCulling();

  glm::vec4 tcolor;
  tvector3 pts[3];
  pts[0] = orig;
  
  pts[1] = (axisU);
  pts[2] = (axisV);
  
  pts[1]*=size;
  pts[2]*=size;
  pts[1]+=orig;
  pts[2]+=orig;
  
  if (!b_selected)
    tcolor = glm::vec4(1,1,0,0.5f);
  else
    tcolor = glm::vec4(1,1,1,0.6f);
  
  rs::gl::begin(GL_TRIANGLES);
  rs::gl::addVertex(pts[0].x, pts[0].y, pts[0].z); rs::gl::addColor(tcolor);
  rs::gl::addVertex(pts[1].x, pts[1].y, pts[1].z); rs::gl::addColor(tcolor);
  rs::gl::addVertex(pts[2].x, pts[2].y, pts[2].z); rs::gl::addColor(tcolor);
  rs::gl::end();

  if (!b_selected)
    tcolor = glm::vec4(1,1,0.2f,1);
  else
    tcolor = glm::vec4(1,1,1,0.6f);
  
  rs::gl::begin(GL_LINE_STRIP);
  rs::gl::addVertex(pts[0].x, pts[0].y, pts[0].z); rs::gl::addColor(tcolor);
  rs::gl::addVertex(pts[1].x, pts[1].y, pts[1].z); rs::gl::addColor(tcolor);
  rs::gl::addVertex(pts[2].x, pts[2].y, pts[2].z); rs::gl::addColor(tcolor);
  rs::gl::addVertex(pts[0].x, pts[0].y, pts[0].z); rs::gl::addColor(tcolor);
  rs::gl::end();
  
  rs::gl::popStyle();
}
