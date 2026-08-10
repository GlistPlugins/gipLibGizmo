/*
 * gipLibgizmoPrimitive3D.cpp
 */

#include "gipLibgizmoPrimitive3D.h"

gipLibgizmoPrimitive3D::gipLibgizmoPrimitive3D() {
	isprojection2d = false;
}

gipLibgizmoPrimitive3D::~gipLibgizmoPrimitive3D() {
}

void gipLibgizmoPrimitive3D::draw(const std::vector<glm::vec3>& points, int drawMode, const glm::vec3& color, bool useAlphaBlending) {
	if (points.empty()) {
		return;
	}

	std::vector<gVertex> vertices;
	vertices.reserve(points.size());
	for (const auto& point : points) {
		gVertex vertex;
		vertex.position = point;
		vertices.push_back(vertex);
	}

	setVertices(vertices);
	setAllVertexColor(color);
	setDrawMode(drawMode);

	renderer->disableDepthTest();
	// The gizmo is a flat-colored UI overlay, not a lit scene object - if the
	// host has a scene light enabled (for the object being edited), it would
	// otherwise shade the gizmo's red/green/blue axis colors too, washing
	// them out. Draw unlit regardless of the host's lighting state, and
	// restore that state afterward in case the host draws more after us.
	bool waslightingenabled = renderer->isLightingEnabled();
	renderer->disableLighting();
	if (useAlphaBlending) {
		renderer->enableAlphaBlending();
	}

	gMesh::draw();

	if (useAlphaBlending) {
		renderer->disableAlphaBlending();
	}
	if (waslightingenabled) {
		renderer->enableLighting();
	}
}
