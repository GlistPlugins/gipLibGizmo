/*
* gCanvas.cpp
*
*  Created on: May 6, 2020
*      Author: Noyan Culum
*/


#include "gCanvas.h"
#include "gRenderer.h"


gCanvas::gCanvas(gApp* root) : gBaseCanvas(root) {
	this->root = root;
}

gCanvas::~gCanvas() {
}

void gCanvas::setup() {
	gizmobox.setPosition(0.0f, 0.25f, -3.0f);
	gizmobox.scale(0.5f);
	gizmoboxtexture.loadTexture("glistengine_logo.png");
	gizmobox.setTexture(&gizmoboxtexture);
	gizmoboxmatrix = gizmobox.getTransformationMatrix();
	gizmoboxinitialmatrix = gizmoboxmatrix;

	// not selected by default - click the box to select it via gipLibgizmo's pick function
	// NOTE: must be the INITIAL (mesh-local, untransformed) bounding box, not
	// getBoundingBox() - that one already has the box's current position/scale
	// baked in, which would double-apply the transform in pickAt()'s OBB test
	// (it re-applies transformMatrix on top) and shift the hit-test region
	// away from where the box actually is.
	gizmoboxboundingbox = gizmobox.getInitialBoundingBox();
	gizmo.addPickable(&gizmoboxboundingbox, &gizmoboxmatrix);

	// point light colocated with the camera (headlamp-style) - full diffuse
	// so the cube's faces actually shade differently depending on their angle
	// to the viewer (makes it read as a cube instead of a flat silhouette),
	// low ambient so unlit faces don't wash out to white, and no specular
	// since a small demo cube doesn't need shiny highlights.
	gizmolight.setType(gLight::LIGHTTYPE_POINT);
	gizmolight.setPosition(gizmocamera.getPosition());
	gizmolight.setAmbientColor(128, 128, 128);
	gizmolight.setDiffuseColor(255, 255, 255);
	gizmolight.setSpecularColor(0, 0, 0);

	hudfont.load(gGetFontsDir() + "FreeSans.ttf", 14);
}

void gCanvas::update() {
}

void gCanvas::draw() {
	enableDepthTest();
	gizmocamera.begin();

	// gizmo.update() must run after camera.begin() - it reads the view/projection
	// matrices that begin() just pushed into the renderer for this frame.
	gizmo.update();

	// keep the light glued to the camera in case it ever moves, and lights
	// must be enabled between camera.begin()/end() per gLight's contract.
	gizmolight.setPosition(gizmocamera.getPosition());
	gizmolight.enable();

	drawGizmoGrid();

	gizmobox.setTransformationMatrix(gizmoboxmatrix);
	gizmobox.draw();

	gizmo.draw();

	gizmolight.disable();
	gizmocamera.end();
	disableDepthTest();

	std::string modename = gizmo.getMode() == gipLibgizmo::MODE_MOVE ? "Move" :
	                        gizmo.getMode() == gipLibgizmo::MODE_ROTATE ? "Rotate" : "Scale";
	std::string spacename = gizmo.getLocation() == IGizmo::LOCATE_LOCAL ? "Local" : "World";

	hudfont.drawText("1: Move   2: Rotate   3: Scale   L: Toggle World/Local   R: Reset Box", 10, 20);
	hudfont.drawText("Mode: " + modename + "   Space: " + spacename, 10, 40);
}

void gCanvas::drawGizmoGrid() {
	const int halfsize = 10;
	const float spacing = 1.0f;

	setColor(80, 80, 80, 255);
	for (int i = -halfsize; i <= halfsize; i++) {
		gDrawLine(-halfsize * spacing, 0.0f, i * spacing, halfsize * spacing, 0.0f, i * spacing);
		gDrawLine(i * spacing, 0.0f, -halfsize * spacing, i * spacing, 0.0f, halfsize * spacing);
	}

	// origin axis indicators (X=red, Y=green, Z=blue)
	setColor(220, 60, 60, 255);
	gDrawLine(0, 0, 0, 2, 0, 0, 2.0f);
	setColor(60, 220, 60, 255);
	gDrawLine(0, 0, 0, 0, 2, 0, 2.0f);
	setColor(60, 60, 220, 255);
	gDrawLine(0, 0, 0, 0, 0, 2, 2.0f);

	setColor(255, 255, 255, 255);
}

void gCanvas::keyPressed(int key) {
	// gizmo mode switch: 1=move, 2=rotate, 3=scale
	if (key == '1') {
		gizmo.setMode(gipLibgizmo::MODE_MOVE);
	} else if (key == '2') {
		gizmo.setMode(gipLibgizmo::MODE_ROTATE);
	} else if (key == '3') {
		gizmo.setMode(gipLibgizmo::MODE_SCALE);
	} else if (key == 'l' || key == 'L') {
		// gizmo space toggle: world <-> local
		if (gizmo.getLocation() == IGizmo::LOCATE_WORLD) {
			gizmo.setLocation(IGizmo::LOCATE_LOCAL);
		} else {
			gizmo.setLocation(IGizmo::LOCATE_WORLD);
		}
	} else if (key == 'r' || key == 'R') {
		// convenience hotkey: reset the box back to its initial transform
		// without restarting the app.
		gizmoboxmatrix = gizmoboxinitialmatrix;
	}
}

void gCanvas::keyReleased(int key) {
//	gLogi("gCanvas") << "keyReleased:" << key;
}

void gCanvas::charPressed(unsigned int codepoint) {
//	gLogi("gCanvas") << "charPressed:" << gCodepointToStr(codepoint);
}

void gCanvas::mouseMoved(int x, int y) {
//	gLogi("gCanvas") << "mouseMoved" << ", x:" << x << ", y:" << y;
}

void gCanvas::mouseDragged(int x, int y, int button) {
//	gLogi("gCanvas") << "mouseDragged" << ", x:" << x << ", y:" << y << ", b:" << button;
}

void gCanvas::mousePressed(int x, int y, int button) {
//	gLogi("gCanvas") << "mousePressed" << ", x:" << x << ", y:" << y << ", b:" << button;
}

void gCanvas::mouseReleased(int x, int y, int button) {
//	gLogi("gCanvas") << "mouseReleased" << ", button:" << button;
}

void gCanvas::mouseScrolled(int x, int y) {
//	gLogi("gCanvas") << "mouseScrolled" << ", x:" << x << ", y:" << y;
}

void gCanvas::mouseEntered() {

}

void gCanvas::mouseExited() {

}

void gCanvas::windowResized(int w, int h) {

}

void gCanvas::showNotify() {

}

void gCanvas::hideNotify() {

}

