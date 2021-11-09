#include "Render\DeferredSceneRenderer.h"
#include "../../SkyEngine/include/SkyEngine.h"
#include "../../SkyEngine/include/Render/RenderContext.h"
#include "../../SkyEngine/include/Render/Graphics.h"
#include "../../SkyEngine/include/Render/GBuffer.h"

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 



DeferredSceneRenderer::DeferredSceneRenderer()
{
	renderContext = new RenderContext();
}

DeferredSceneRenderer::~DeferredSceneRenderer()
{

}

bool DeferredSceneRenderer::Initialize(int inWindowWidth, int inWindowHeight)
{
	mViewWidth = inWindowWidth;
	mViewHeight = inWindowHeight;

	mGBuffer = new GBuffer();

	if (!mGBuffer->Init(inWindowWidth, inWindowHeight)) {
		return false;
	}

	CreateSphere();
	glCheckError();
	mCurrentContentType = RenderContentType::Result;
	return true;
}

void DeferredSceneRenderer::ResizeGL(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}

	mViewWidth = width;
	mViewHeight = height;

	//Reset View
	glViewport(0, 0, (GLint)width, (GLint)height);
	//Choose the Matrix mode
	glMatrixMode(GL_PROJECTION);
	//reset projection
	glLoadIdentity();
	//set perspection
	gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1, 1000.0);
	//choose Matrix mode
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();
}

void DeferredSceneRenderer::DrawMesh(Mesh* mesh, Matrix4x4* modelMatrix, Material* mat, Camera* cam)
{
	Graphics::DrawMesh(mesh, modelMatrix, mat, cam, renderContext);
}


void DeferredSceneRenderer::CreateSphere() {
	int stacks = 20;
	int slices = 20;
	std::vector<float> positions;
	std::vector<GLuint> indices;

	// loop through stacks.
	for (int i = 0; i <= stacks; ++i) {

		float V = (float)i / (float)stacks;
		float phi = V * PI;

		// loop through the slices.
		for (int j = 0; j <= slices; ++j) {

			float U = (float)j / (float)slices;
			float theta = U * (PI * 2);

			// use spherical coordinates to calculate the positions.
			float x = cos(theta) * sin(phi);
			float y = cos(phi);
			float z = sin(theta) * sin(phi);

			positions.push_back(x);
			positions.push_back(y);
			positions.push_back(z);
		}
	}

	// Calc The Index Positions
	for (int i = 0; i < slices * stacks + slices; ++i) {
		indices.push_back(GLuint(i));
		indices.push_back(GLuint(i + slices + 1));
		indices.push_back(GLuint(i + slices));

		indices.push_back(GLuint(i + slices + 1));
		indices.push_back(GLuint(i));
		indices.push_back(GLuint(i + 1));
	}
	glGenVertexArrays(1, &mLightSphereVAO);

	// upload geometry to GPU.
	glGenBuffers(1, &mSpherePositionVbo);
	glBindBuffer(GL_ARRAY_BUFFER, mSpherePositionVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * positions.size(), positions.data(), GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);
//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

	glGenBuffers(1, &mSphereIndexVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mSphereIndexVbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	//
	//glEnableVertexAttribArray(0);
	//
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

	mSphereIndexCount = indices.size();
}


void DeferredSceneRenderer::Render(Scene* scene, Camera* camera)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	// setup GL state.
	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
	glDisable(GL_BLEND);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	renderContext->projectionMatrix = camera->projectionMaxtrix;
	renderContext->viewMatrix = camera->GetViewMatrix();
	renderContext->viewProjectionMatrix = camera->GetViewMatrix() * camera->GetProjectionMatrix();
	renderContext->viewPos = camera->transform.position;

	if (scene != NULL)
	{
		GeoPass(scene);
		//RenderGameObject(scene->GetRoot(), renderContext);
	}
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // stop writing to gbuffer.
	////
	//// Now comes the Deferred shading!
	////
	glViewport(0, 0, mViewWidth, mViewHeight);
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (mCurrentContentType != Result)
	{
		DirLightPass(camera);
	}
	else
	{
		DirLightPass(camera);
		PointLightPass(camera);
	}
}

void DeferredSceneRenderer::SetShaderProgram(ShaderProgram* geoPass, ShaderProgram* dirLightingPass, ShaderProgram* pointLightingPass)
{
	mGeometryPass = geoPass;
	mDirLightingPass = dirLightingPass;
	mPointLightingPass = pointLightingPass;

}


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}


void DeferredSceneRenderer::RenderPointLight(float radius, const Vector3& position, const Color& color) {
	glUniform1f(glGetUniformLocation(mPointLightingPass->GetProgramID(), "uLightRadius"), radius);
	glUniform3f(glGetUniformLocation(mPointLightingPass->GetProgramID(), "uLightPosition"), position.x, position.y, position.z);
	glUniform3f(glGetUniformLocation(mPointLightingPass->GetProgramID(), "uLightColor"), color.r, color.g, color.b);
	glDrawElements(GL_TRIANGLES, mSphereIndexCount, GL_UNSIGNED_INT, 0);
}



void DeferredSceneRenderer::GeoPass(Scene* scene)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mGBuffer->GetFBO());
	glUseProgram(mGeometryPass->GetProgramID());
	//glUniformMatrix4fv(glGetUniformLocation(mGeometryPass->GetProgramID(), "model"), 1, GL_FALSE, (GLfloat *)Matrix4x4::identity.get());
	glUniformMatrix4fv(glGetUniformLocation(mGeometryPass->GetProgramID(), "view"), 1, GL_FALSE, (GLfloat*)renderContext->viewMatrix.get());
	glUniformMatrix4fv(glGetUniformLocation(mGeometryPass->GetProgramID(), "projection"), 1, GL_FALSE, (GLfloat*)renderContext->projectionMatrix.get());
	glViewport(0, 0, mViewWidth, mViewHeight);
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderGameObjectGeo(scene->GetRoot(), renderContext);
}

void DeferredSceneRenderer::DirLightPass(Camera* camera)
{
	glUseProgram(mDirLightingPass->GetProgramID());

	Light* pDirLight = GetLightManager().GetDirectionLight();

	glUniform3f(glGetUniformLocation(mDirLightingPass->GetProgramID(), "uLightDir"), pDirLight->direction.x, pDirLight->direction.y, pDirLight->direction.z);
	glUniform3f(glGetUniformLocation(mDirLightingPass->GetProgramID(), "uLightColor"), pDirLight->color.r, pDirLight->color.g, pDirLight->color.b);

	mGBuffer->SetupDeferredShader(mDirLightingPass->GetProgramID(), camera->transform.position);
	renderQuad();
}



void DeferredSceneRenderer::PointLightPass(Camera* camera)
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glFrontFace(GL_CW);

	glUseProgram(mPointLightingPass->GetProgramID());
	mGBuffer->SetupDeferredShader(mPointLightingPass->GetProgramID(), camera->transform.position);

	glUniformMatrix4fv(glGetUniformLocation(mPointLightingPass->GetProgramID(), "model"), 1, GL_FALSE, (GLfloat*)Matrix4x4::identity.get());
	glUniformMatrix4fv(glGetUniformLocation(mPointLightingPass->GetProgramID(), "view"), 1, GL_FALSE, (GLfloat*)renderContext->viewMatrix.get());
	glUniformMatrix4fv(glGetUniformLocation(mPointLightingPass->GetProgramID(), "projection"), 1, GL_FALSE, (GLfloat*)renderContext->projectionMatrix.get());


	// We render every point light as a light sphere. And this light sphere is added onto the framebuffer
	// with additive alpha blending.
	glBindVertexArray(mLightSphereVAO);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mSpherePositionVbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mSphereIndexVbo);


	vector<Light*> pLight = GetLightManager().GetPointLights();

	for (vector<Light*>::const_iterator lightIter = pLight.begin(); lightIter != pLight.end(); lightIter++)
	{
		if ((*lightIter)->type != kLightPoint)
		{
			qDebug() << "Cannot Set DirectionLight. Wrong light type. ";
			return;
		}
		RenderPointLight((*lightIter)->range, (*lightIter)->GetPosition(), (*lightIter)->color);

	}


	//RenderPointLight(6.0f, Vector3(0, 0, -5.5f), Vector3(1.0f, 0.0f, 0.0f));
	//RenderPointLight(4.0f, Vector3(3, 0, -5.5f), Vector3(1.0f, 1.0f, 0.0f));
}

void DeferredSceneRenderer::RenderGameObjectGeo(GameObject* gameObject, RenderContext* renderContext)
{
	//Get a RenderingComponent from gameObject and invoke render() on it
	MeshRenderer* rc = gameObject->GetComponent<MeshRenderer>();

	if (rc != 0)
	{
		renderContext->modelMatrix = gameObject->GetTransform()->GetLocalToWorldMatrix();
		//rc->GetMaterial()->Bind();
		rc->GetMaterial()->Bind(renderContext);
		//glUniformMatrix4fv(glGetUniformLocation(mGeometryPass->GetProgramID(), "model"), 1, GL_FALSE, (GLfloat *)renderContext->modelMatrix.get());

		GLuint vao = rc->GetMesh()->GetVAO();
		glBindVertexArray(vao);

		//DrawElements
		if (rc->GetMesh()->GetIndices().size() > 0)
		{
			glDrawElements(
				GL_TRIANGLES,
				rc->GetMesh()->GetIndices().size(),
				GL_UNSIGNED_INT,
				0);
		}
		else
		{
			int size = rc->GetMesh()->GetPositions().size();
			glDrawArrays(GL_TRIANGLES, 0, size);
		}

		//Unbind the Vertex Array
		glBindVertexArray(0);
	}

	//For all of gameObject's children, call render() on them
	uint32 numChildren = gameObject->GetNumChildren();
	for (uint32 i = 0; i < numChildren; i++)
	{
		RenderGameObjectGeo(gameObject->GetChild(i), renderContext);
	}
}


void DeferredSceneRenderer::RenderGameObject(GameObject* gameObject, RenderContext* renderContext)
{
	//Get a RenderingComponent from gameObject and invoke render() on it
	MeshRenderer* rc = gameObject->GetComponent<MeshRenderer>();

	if (rc != 0)
	{
		rc->Render(renderContext);
	}

	//For all of gameObject's children, call render() on them
	uint32 numChildren = gameObject->GetNumChildren();
	for (uint32 i = 0; i < numChildren; i++)
	{
		RenderGameObject(gameObject->GetChild(i), renderContext);
	}
}
void DeferredSceneRenderer::SwitchRenderType(RenderContentType type)
{
	mCurrentContentType = type;
	switch (type)
	{
	case RenderContentType::Albedo:
		glUniform1f(glGetUniformLocation(mDirLightingPass->GetProgramID(), "uRenderType"), 1.0f);
		return;
	case RenderContentType::Normal:
		glUniform1f(glGetUniformLocation(mDirLightingPass->GetProgramID(), "uRenderType"), 2.0f);
		return;

	case RenderContentType::PosWorld:
		glUniform1f(glGetUniformLocation(mDirLightingPass->GetProgramID(), "uRenderType"), 3.0f);
		return;
	case RenderContentType::Depth:
		glUniform1f(glGetUniformLocation(mDirLightingPass->GetProgramID(), "uRenderType"), 4.0f);
		return;
	case RenderContentType::Result:
		glUniform1f(glGetUniformLocation(mDirLightingPass->GetProgramID(), "uRenderType"), 0.0f);
		return;
	}
}
