#pragma once
#include <QOpenGLShaderProgram>

class Object
{
public:
	virtual void draw() = 0;
	virtual void setShader(QOpenGLShaderProgram* shader) = 0;
	virtual void initialize() = 0;
	QOpenGLShaderProgram* shader;
};

