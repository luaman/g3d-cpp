/**
  @file QGWindow.h
  @author Corey Taylor, corey@acz.org
  @created 2004-03-07

*/

#include <qgl.h>

#include "..\Setup.h"
#include "QGWindow.h"
#include "RenderWindow.h"

QGWindow::QGWindow(GWindowSettings &settings)
{
	QGLFormat format;
	format.setDoubleBuffer(true);
	format.setRgba(true);
	format.setDirectRendering(true);
	if( settings.depthBits > 0 ) format.setDepth(true);
	if( settings.alphaBits > 0 ) format.setAlpha(true);
	if( settings.stereo ) format.setStereo(true);
	if( settings.stencilBits > 0 ) format.setStencil(true);
	glWindow = new RenderWindow(format, aeGlobals.mainWindow);
	glWindow->resize(800,600);
	glWindow->show();
}


QGWindow::~QGWindow()
{
	delete glWindow;
}


void QGWindow::getSettings(GWindowSettings &settings) const
{
}


int QGWindow::width() const
{
	return 800;
}


int QGWindow::height() const
{
	return 600;
}


Rect2D QGWindow::dimensions() const
{
	return Rect2D();
}


void QGWindow::setDimensions(const Rect2D &dims)
{
}


void QGWindow::setPosition(int x, int y) const
{
}


bool QGWindow::hasFocus() const
{
	return true;
}


std::string QGWindow::getAPIVersion() const
{
	return std::string("No name");
}


std::string QGWindow::getAPIName() const
{
	return std::string("No name");
}


void QGWindow::setGammaRamp(const Array< uint16 > &gammaRamp)
{
}


void QGWindow::setCaption(const std::string &caption)
{
}


int QGWindow::numJoysticks() const
{
	return 0;
}


Vector2 QGWindow::joystickPosition(int stickNum) const
{
	return Vector2(0, 0);
}


std::string QGWindow::caption()
{
	return std::string("no caption");
}


void QGWindow::setIcon(const GImage &image)
{
}


void QGWindow::swapGLBuffers()
{
	glWindow->swapBuffers();
}


void QGWindow::notifyResize(int w, int h)
{
}


void QGWindow::setRelativeMousePosition(double x, double y)
{
}


void QGWindow::setRelativeMousePosition(const Vector2 &p)
{
}


void QGWindow::getRelativeMouseState(Vector2 &position, uint8 &mouseButtons) const
{
}


void QGWindow::getRelativeMouseState(int &x, int &y, uint8 &mouseButtons) const
{
}


void QGWindow::getRelativeMouseState(double &x, double &y, uint8 &mouseButtons) const
{
}


void QGWindow::setMouseCapture(bool c)
{
}


bool QGWindow::mouseCapture() const
{
	return false;
}


void QGWindow::setMouseVisible(bool b)
{
}


bool QGWindow::mouseVisible() const
{
	return true;
}


