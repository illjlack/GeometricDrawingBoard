#include "Object.h"


namespace My
{
	void Object::setShader(QOpenGLShaderProgram* shader)
	{
		this->shaderProgram = shader;
	}
}