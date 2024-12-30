#pragma once
#include <QOpenGLShaderProgram>

class Object
{
public:
	//virtual ~Object() = 0;

	virtual void draw() = 0;
	virtual void setShader(QOpenGLShaderProgram* shader);
	virtual void initialize() = 0;
	QOpenGLShaderProgram* shaderProgram;
};

