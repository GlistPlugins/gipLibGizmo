/*
 * gipLibgizmo.cpp
 *
 *      Author: Mert Sanli
 */

#include "gipLibgizmo.h"
#include "gRenderObject.h"
#include "gRenderer.h"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <limits>


gipLibgizmo::gipLibgizmo() {
	gizmo = nullptr;
	editmatrix = nullptr;
	mode = MODE_MOVE;
	cachedviewmatrix = glm::mat4(1.0f);
	cachedprojectionmatrix = glm::mat4(1.0f);
	cachedviewportx = cachedviewporty = cachedviewportwidth = cachedviewportheight = 0;
	createGizmoForMode();
}

gipLibgizmo::~gipLibgizmo() {
	delete gizmo;
}

void gipLibgizmo::createGizmoForMode() {
	IGizmo::LOCATION previouslocation = gizmo ? gizmo->GetLocation() : IGizmo::LOCATE_WORLD;
	delete gizmo;

	switch (mode) {
		case MODE_ROTATE:
			gizmo = CreateRotateGizmo();
			break;
		case MODE_SCALE:
			gizmo = CreateScaleGizmo();
			break;
		case MODE_MOVE:
		default:
			gizmo = CreateMoveGizmo();
			break;
	}

	gizmo->SetLocation(previouslocation);
	if (editmatrix) {
		gizmo->SetEditMatrix(glm::value_ptr(*editmatrix));
	}
}

void gipLibgizmo::setMode(MODE newmode) {
	if (mode == newmode) {
		return;
	}
	mode = newmode;
	createGizmoForMode();
}

gipLibgizmo::MODE gipLibgizmo::getMode() {
	return mode;
}

void gipLibgizmo::setLocation(IGizmo::LOCATION location) {
	gizmo->SetLocation(location);
}

IGizmo::LOCATION gipLibgizmo::getLocation() {
	return gizmo->GetLocation();
}

void gipLibgizmo::setEditMatrix(glm::mat4* pMatrix) {
	editmatrix = pMatrix;
	gizmo->SetEditMatrix(pMatrix ? glm::value_ptr(*pMatrix) : nullptr);
}

glm::mat4* gipLibgizmo::getEditMatrix() {
	return editmatrix;
}

void gipLibgizmo::update() {
	// Cached unconditionally (even with nothing selected) because pickAt()
	// needs valid camera/viewport data too, and it runs from mousePressed()
	// which is dispatched between frames - by then gCamera::end() has already
	// restored the renderer's 2D default matrices, so it can't read
	// renderer->getViewMatrix()/getProjectionMatrix() live at that point.
	cachedviewmatrix = renderer->getViewMatrix();
	cachedprojectionmatrix = renderer->getProjectionMatrix();
	renderer->getViewport(cachedviewportx, cachedviewporty, cachedviewportwidth, cachedviewportheight);

	if (!editmatrix) {
		return;
	}

	// LibGizmo's tmatrix is reinterpreted directly from these float pointers
	// (see CGizmoTransform::SetCameraMatrix/SetEditMatrix in GizmoTransform.h)
	// and assumes the same 16-float layout as glm::mat4 (column-major) -
	// confirmed correct by visual testing (translate along all 3 axes tracks
	// the mouse correctly).
	gizmo->SetCameraMatrix(glm::value_ptr(cachedviewmatrix), glm::value_ptr(cachedprojectionmatrix));
	gizmo->SetScreenDimension(renderer->getWidth(), renderer->getHeight());
}

void gipLibgizmo::draw() {
	if (!editmatrix) {
		return;
	}
	gizmo->Draw();
	// gizmo drawing disables depth testing (see gipLibgizmoPrimitive3D::draw) so it
	// always renders on top - restore it for whatever draws after the gizmo this frame.
	renderer->enableDepthTest();
}

void gipLibgizmo::mousePressed(int x, int y, int button) {
	if (button != 0) {
		return;
	}
	bool capturedbygizmo = false;
	if (editmatrix) {
		capturedbygizmo = gizmo->OnMouseDown((unsigned int)x, (unsigned int)y);
	}
	if (!capturedbygizmo) {
		pickAt(x, y);
	}
}

void gipLibgizmo::mouseDragged(int x, int y, int button) {
	// NOTE: unlike mousePressed/mouseReleased, gAppManager passes a bitmask of
	// all currently-held buttons here (see gAppManager::onMouseMovedEvent -
	// mousebuttonstate is built as `1 << (button+1)` per button), not a plain
	// button index. Bit 1 (value 2) is the left button; gate on that instead
	// of comparing for equality with 0.
	if (!editmatrix || !(button & 2)) {
		return;
	}
	gizmo->OnMouseMove((unsigned int)x, (unsigned int)y);
}

void gipLibgizmo::mouseReleased(int x, int y, int button) {
	if (!editmatrix || button != 0) {
		return;
	}
	gizmo->OnMouseUp((unsigned int)x, (unsigned int)y);
}

void gipLibgizmo::addPickable(gBoundingBox* boundingBox, glm::mat4* transformMatrix) {
	pickables.push_back({boundingBox, transformMatrix});
}

void gipLibgizmo::removePickable(glm::mat4* transformMatrix) {
	for (size_t i = 0; i < pickables.size(); i++) {
		if (pickables[i].transformMatrix == transformMatrix) {
			pickables.erase(pickables.begin() + i);
			if (editmatrix == transformMatrix) {
				setEditMatrix(nullptr);
			}
			return;
		}
	}
}

void gipLibgizmo::clearPickables() {
	pickables.clear();
	setEditMatrix(nullptr);
}

void gipLibgizmo::pickAt(int x, int y) {
	glm::vec4 viewport(cachedviewportx, cachedviewporty, cachedviewportwidth, cachedviewportheight);

	// mouse y from gAppManager is top-down (0 at top of window), but
	// glm::unProject expects bottom-up OpenGL window coordinates.
	float flippedy = (float)cachedviewportheight - (float)y;
	glm::vec3 nearpoint = glm::unProject(glm::vec3((float)x, flippedy, 0.0f), cachedviewmatrix, cachedprojectionmatrix, viewport);
	glm::vec3 farpoint = glm::unProject(glm::vec3((float)x, flippedy, 1.0f), cachedviewmatrix, cachedprojectionmatrix, viewport);
	// NOTE: gRay's length (used by gBoundingBox::intersectsOBB/distanceOBB to
	// bound the hit test) is derived from the magnitude of the direction
	// vector passed here (see gRay::gRay/setDirection - length =
	// glm::length(directionVector)), not recomputed dynamically. Passing a
	// normalized direction would give the ray a length of 1 world unit,
	// which is far too short to reach any object beyond that distance from
	// the camera - so the direction must stay unnormalized (near-to-far span).
	gRay ray(nearpoint, farpoint - nearpoint);

	glm::mat4* nearesttransform = nullptr;
	float nearestdistance = std::numeric_limits<float>::max();
	for (gipLibgizmoPickable& pickable : pickables) {
		pickable.boundingBox->setTransformationMatrix(*pickable.transformMatrix);
		bool hit = pickable.boundingBox->intersectsOBB(&ray);
		float distance = pickable.boundingBox->distanceOBB(&ray);
		if (hit && distance < nearestdistance) {
			nearestdistance = distance;
			nearesttransform = pickable.transformMatrix;
		}
	}

	// A miss means "nothing new selected", not "deselect the current object" -
	// only replace editmatrix when something was actually hit.
	if (nearesttransform) {
		setEditMatrix(nearesttransform);
	}
}
