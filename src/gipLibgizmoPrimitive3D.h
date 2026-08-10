/*
 * gipLibgizmoPrimitive3D.h
 *
 * Small gMesh-backed helper used to draw ad hoc 3D line/triangle geometry
 * every frame (line loops, triangle fans, etc.), so LibGizmo's vendored
 * render layer (GizmoTransformRender.cpp) can draw through GlistEngine's
 * gRenderer instead of legacy fixed-function OpenGL, which GlistEngine's
 * core-profile GL/Vulkan context does not support.
 */

#ifndef SRC_GIPLIBGIZMOPRIMITIVE3D_H_
#define SRC_GIPLIBGIZMOPRIMITIVE3D_H_

#include "gMesh.h"
#include <vector>

class gipLibgizmoPrimitive3D : public gMesh {
public:
	gipLibgizmoPrimitive3D();
	virtual ~gipLibgizmoPrimitive3D();

	void draw(const std::vector<glm::vec3>& points, int drawMode, const glm::vec3& color, bool useAlphaBlending = false);
};

#endif /* SRC_GIPLIBGIZMOPRIMITIVE3D_H_ */
