/*
 * gipLibgizmo.h
 *
 *      Author: Mert Sanli
 */

#ifndef SRC_GIPLIBGIZMO_H_
#define SRC_GIPLIBGIZMO_H_

#include "gBasePlugin.h"
#include "gBoundingBox.h"
#include "libgizmo/IGizmo.h"
#include "glm/glm.hpp"
#include <vector>


// A candidate object the plugin can select via pickAt()/mousePressed(). The
// host owns both pointers and must keep them alive/at the same address for as
// long as they're registered - the plugin only reads/updates the bounding
// box's transform and reads/writes through the matrix pointer once selected.
struct gipLibgizmoPickable {
	gBoundingBox* boundingBox;
	glm::mat4* transformMatrix;
};


class gipLibgizmo : public gBasePlugin {
public:
	enum MODE {
		MODE_MOVE,
		MODE_ROTATE,
		MODE_SCALE
	};

	gipLibgizmo();
	virtual ~gipLibgizmo();

	void update();
	// should be called by the host canvas/app after the scene (and camera) has
	// been drawn for this frame, so the gizmo renders as an overlay on top of it.
	void draw();

	void setMode(MODE mode);
	MODE getMode();

	void setLocation(IGizmo::LOCATION location);
	IGizmo::LOCATION getLocation();

	// the transform being manipulated: gizmo reads it every frame and writes
	// back into it directly (in place) while the user drags a handle.
	// pMatrix must stay alive and stay at the same address for as long as it's
	// set here - pass nullptr to detach (no object selected).
	void setEditMatrix(glm::mat4* pMatrix);
	glm::mat4* getEditMatrix();

	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	void mouseReleased(int x, int y, int button);

	// registers an object as pickable by clicking on it (see pickAt()). Does
	// not take ownership - the host must keep boundingBox/transformMatrix
	// alive as long as they're registered.
	void addPickable(gBoundingBox* boundingBox, glm::mat4* transformMatrix);
	void removePickable(glm::mat4* transformMatrix);
	void clearPickables();

	// casts a ray from the given screen position (current camera, current
	// frame) against all registered pickables and selects the nearest hit
	// (i.e. calls setEditMatrix() on it). If nothing is hit, the current
	// selection is left unchanged - a miss never deselects (see the NOTE in
	// pickAt()'s implementation for why). Called automatically from
	// mousePressed() whenever the click didn't land on an already-attached
	// gizmo handle, but can also be called directly if a host wants
	// pick-on-release or similar.
	void pickAt(int x, int y);

private:
	void createGizmoForMode();

	IGizmo* gizmo;
	MODE mode;
	glm::mat4* editmatrix;
	std::vector<gipLibgizmoPickable> pickables;

	// cached each frame in update() - gCamera::end() restores the renderer's
	// 2D default matrices, and mouse events (which is when pickAt() runs) are
	// dispatched between frames, after that restore already happened. So
	// pickAt() can't read renderer->getViewMatrix()/getProjectionMatrix()
	// live - it has to reuse whatever the 3D camera last pushed during draw().
	glm::mat4 cachedviewmatrix, cachedprojectionmatrix;
	int cachedviewportx, cachedviewporty, cachedviewportwidth, cachedviewportheight;
};

#endif /* SRC_GIPLIBGIZMO_H_ */
