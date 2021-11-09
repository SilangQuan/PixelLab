#pragma once
#include "Base.h"
#include "Texture.h"


struct Particle {
	Vector3 Position, Velocity;
	Color Color;
	float Life;

	Particle()
		: Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


class ParticleSystem
{
    ParticleSystem();
    ~ParticleSystem();
    bool InitParticleSystem(const Vector3& Pos);
    void Render(int DeltaTimeMillis, const Matrix4x4& VP, const Vector3& CameraPos);
    void UpdateParticles(int DeltaTimeMillis);
    void RenderParticles(const Matrix4x4& VP, const Vector3& CameraPos);

private:
    bool m_isFirst;
    unsigned int m_currVB;
    unsigned int m_currTFB;
    GLuint m_particleBuffer[2];
    GLuint m_transformFeedback[2];
    //PSUpdateTechnique m_updateTechnique;
    //BillboardTechnique m_billboardTechnique;
    //RandomTexture m_randomTexture;
    Texture* m_pTexture;
    int m_time;
};

