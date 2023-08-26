#include "Render/Threaded/RenderDeviceWorker.h"

void RenderDeviceWorker::RunOneThreadCommand()
{
	GfxCommandType cmd = _commandBuffer->ReadValueType<GfxCommandType>();
	switch (cmd)
	{
	case GfxCommandType::kGfxCmd_Clear:
	{
		_realDevice->Clear();
		_commandBuffer->ReadReleaseData();
		break;
	}
	/*
	case GfxCommandType::kGfxCmd_UseGPUProgram:
	{
		ThreadedGPUProgram* program = _commandBuffer->ReadValueType<ThreadedGPUProgram*>();
		_realDevice->UseGPUProgram(program->realProgram);
		_commandBuffer->ReadReleaseData();
		break;
	}
	case GfxCommandType::kGfxCmd_CreateGPUProgram:
	{
		auto data = _commandBuffer->ReadValueType<GfxCmdCreateGPUProgramData>();
		std::string vertexShader, fragmentShader;
		vertexShader.resize(data.vSize);
		fragmentShader.resize(data.fSize);
		_commandBuffer->ReadStreamingData((void*)vertexShader.c_str(), vertexShader.size());
		_commandBuffer->ReadStreamingData((void*)fragmentShader.c_str(), fragmentShader.size());
		data.program->realProgram = _realDevice->CreateGPUProgram(vertexShader, fragmentShader);
		break;
	}

	case GfxCommandType::kGfxCmd_DeleteGPUProgram:
	{
		ThreadedGPUProgram* program = _commandBuffer->ReadValueType<ThreadedGPUProgram*>();
		_realDevice->DeletGPUProgram(program->realProgram);
		delete program;
		_commandBuffer->ReadReleaseData();
		break;
	}
	case GfxCommandType::kGfxCmd_CreateTexture2D:
	{
		auto data = _commandBuffer->ReadValueType<GfxCmdCreateTextureData>();
		char* buff = new char[data.dataLen];
		_commandBuffer->ReadStreamingData(buff, data.dataLen);
		TextureData::Ptr textureData = std::make_shared<TextureData>(buff, data.width, data.height, data.dataLen);
		data.texture->realTexture = _realDevice->CreateTexture2D(textureData);
		break;
	}

	case GfxCommandType::kGfxCmd_DeleteTexture2D:
	{
		auto texture = _commandBuffer->ReadValueType<ThreadedTexture2D*>();
		_realDevice->DeleteTexture2D(texture->realTexture);
		_commandBuffer->ReadReleaseData();
		delete texture;
		break;
	}
	case GfxCommandType::kGfxCmd_UseTexture2D:
	{
		auto texture = _commandBuffer->ReadValueType<ThreadedTexture2D*>();
		auto index = _commandBuffer->ReadValueType<unsigned int>();
		_realDevice->UseTexture2D(texture->realTexture, index);
		_commandBuffer->ReadReleaseData();
		break;
	}
	*/
	case GfxCommandType::kGfxCmd_SetClearColor:
	{
		auto data = _commandBuffer->ReadValueType<Color>();
		_realDevice->SetClearColor(data.r, data.g, data.b, data.a);
		_commandBuffer->ReadReleaseData();
		break;
	}

	case GfxCommandType::kGfxCmd_DrawTriangle:
	{
		auto size = _commandBuffer->ReadValueType<unsigned int>();
		std::vector<Vector3> vertices(size / sizeof(Vector3));
		vertices.resize(size / sizeof(Vector3));
		_commandBuffer->ReadStreamingData((void*)&vertices[0], size);
		_realDevice->DrawTriangle(vertices);
		break;
	}

	case GfxCommandType::kGfxCmd_SetViewport:
	{
		Vector4 data = _commandBuffer->ReadValueType<Vector4>();
		_realDevice->SetViewPort(data.x, data.y, data.z, data.w);
		_commandBuffer->ReadReleaseData();
		break;
	}

	case GfxCommandType::kGfxCmd_Present:
	{
		_realDevice->Present();
		_commandBuffer->ReadReleaseData();
		SignalPresent();
		break;
	}

	case GfxCommandType::kGfxCmd_AcqiureThreadOwnerShip:
	{
		_realDevice->AcqiureThreadOwnerShip();
		_commandBuffer->ReadReleaseData();
		break;
	}
	case GfxCommandType::kGfxCmd_ReleaseThreadOwnership:
	{
		_realDevice->ReleaseThreadOwnership();
		_commandBuffer->ReadReleaseData();
		break;
	}
	/*
	case GfxCommandType::kGfxCmd_CreateVBO:
	{
		ThreadedVBO* threadedVbo = _commandBuffer->ReadValueType<ThreadedVBO*>();
		threadedVbo->realVbo = _realDevice->CreateVBO();
		_commandBuffer->ReadReleaseData();
		break;
	}
	case GfxCommandType::kGfxCmd_UpdateVBO:
	{
		GfxCmdUpdateVBOData data = _commandBuffer->ReadValueType<GfxCmdUpdateVBOData>();
		BeginProfile("kGfxCmd_UpdateVBO alloc");
		VBOData::Ptr vboData = std::make_shared<VBOData>(data.verticesCount, data.indicesCount);
		EndProfile();
		BeginProfile("kGfxCmd_UpdateVBO write");
		_commandBuffer->ReadStreamingData((void*)vboData->vertices, data.verticesCount * sizeof(VBOData::Vertex));
		_commandBuffer->ReadStreamingData((void*)vboData->indices, data.indicesCount * sizeof(unsigned short));
		EndProfile();
		_realDevice->UpdateVBO(data.vbo->realVbo, vboData);
		vboData.reset();
		break;
	}
	case GfxCommandType::kGfxCmd_DeleteVBO:
	{
		ThreadedVBO* threadedVbo = _commandBuffer->ReadValueType<ThreadedVBO*>();
		_realDevice->DeleteVBO(threadedVbo->realVbo);
		delete threadedVbo;
		_commandBuffer->ReadReleaseData();
		break;
	}
	case GfxCommandType::kGfxCmd_DrawVBO:
	{
		ThreadedVBO* threadedVbo = _commandBuffer->ReadValueType<ThreadedVBO*>();
		_realDevice->DrawVBO(threadedVbo->realVbo);
		_commandBuffer->ReadReleaseData();
		break;
	}
	case kGfxCmd_SetGPUProgramAsInt:
	{
		ThreadedGPUProgramParam* threadParam = _commandBuffer->ReadValueType<ThreadedGPUProgramParam*>();
		int value = _commandBuffer->ReadValueType<int>();
		_realDevice->SetGPUProgramParamAsInt(threadParam->realParam, value);
		_commandBuffer->ReadReleaseData();
		break;
	}
	case kGfxCmd_SetGPUProgramAsFloat:
	{
		ThreadedGPUProgramParam* threadParam = _commandBuffer->ReadValueType<ThreadedGPUProgramParam*>();
		float value = _commandBuffer->ReadValueType<float>();
		_realDevice->SetGPUProgramParamAsFloat(threadParam->realParam, value);
		_commandBuffer->ReadReleaseData();
		break;
	}

	case kGfxCmd_SetGPUProgramAsMat4:
	{
		ThreadedGPUProgramParam* threadParam = _commandBuffer->ReadValueType<ThreadedGPUProgramParam*>();
		auto value = _commandBuffer->ReadValueType<glm::mat4>();
		_realDevice->SetGPUProgramParamAsMat4(threadParam->realParam, value);
		_commandBuffer->ReadReleaseData();
		break;
	}

	case kGfxCmd_SetGPUProgramAsIntArray:
	{
		ThreadedGPUProgramParam* threadParam = _commandBuffer->ReadValueType<ThreadedGPUProgramParam*>();
		auto size = _commandBuffer->ReadValueType<unsigned int>();
		std::vector<int> values;
		values.resize(size / sizeof(int));
		_commandBuffer->ReadStreamingData(&values[0], size);
		_realDevice->SetGPUProgramParamAsIntArray(threadParam->realParam, values);
		break;
	}

	case kGfxCmd_SetGPUProgramAsFloatArray:
	{
		ThreadedGPUProgramParam* threadParam = _commandBuffer->ReadValueType<ThreadedGPUProgramParam*>();
		auto size = _commandBuffer->ReadValueType<unsigned int>();
		std::vector<float> values;
		values.resize(size / sizeof(float));
		_commandBuffer->ReadStreamingData(&values[0], size);
		_realDevice->SetGPUProgramParamAsFloatArray(threadParam->realParam, values);
		break;
	}

	case kGfxCmd_SetGPUProgramAsMat4Array:
	{
		ThreadedGPUProgramParam* threadParam = _commandBuffer->ReadValueType<ThreadedGPUProgramParam*>();
		auto size = _commandBuffer->ReadValueType<unsigned int>();
		std::vector<glm::mat4> values;
		values.resize(size / sizeof(glm::mat4));
		_commandBuffer->ReadStreamingData(&values[0], size);
		_realDevice->SetGPUProgramParamAsMat4Array(threadParam->realParam, values);
		break;
	}

	case kGfxCmd_InitThreadGPUProgramParam:
	{
		ThreadedGPUProgram* program = _commandBuffer->ReadValueType<ThreadedGPUProgram*>();
		ThreadedGPUProgramParam* threadParam = _commandBuffer->ReadValueType<ThreadedGPUProgramParam*>();
		auto size = _commandBuffer->ReadValueType<unsigned int>();
		std::string name;
		name.resize(size / sizeof(char));
		_commandBuffer->ReadStreamingData((void*)name.c_str(), size);
		threadParam->realParam = _realDevice->GetGPUProgramParam(program->realProgram, name);
		break;
	}
	*/
	default:
		assert(false);
		break;
	}
}