 /*
 * gCanvas.h
 *
 *  Created on: May 6, 2020
 *      Author: Noyan Culum
 */

#ifndef GCANVAS_H_
#define GCANVAS_H_

#include "gBaseCanvas.h"
#include "gApp.h"
#include "gCamera.h"
#include "gBox.h"
#include "gFont.h"
#include "gLight.h"
#include "gTexture.h"
#include "gipLibgizmo.h"


class gCanvas : public gBaseCanvas {
public:
	gCanvas(gApp* root);
	virtual ~gCanvas();

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void charPressed(unsigned int codepoint);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseScrolled(int x, int y);
	void mouseEntered();
	void mouseExited();
	void windowResized(int w, int h);

	void showNotify();
	void hideNotify();

private:
	void drawGizmoGrid();

	gApp* root;
	gFont hudfont;

	// gipLibgizmo test scene
	gCamera gizmocamera;
	gLight gizmolight;
	gBox gizmobox;
	gTexture gizmoboxtexture;
	glm::mat4 gizmoboxmatrix;
	glm::mat4 gizmoboxinitialmatrix;
	gBoundingBox gizmoboxboundingbox;
	gipLibgizmo gizmo;
};

#endif /* GCANVAS_H_ */
