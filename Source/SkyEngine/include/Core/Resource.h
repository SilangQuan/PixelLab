#pragma once

#include "EngineBase.h"
typedef int ResHandle;


class Resource
{
public:
	Resource();
	Resource(const std::string &name);
	virtual ~Resource();

	virtual void initDefault();
	virtual void release();
	virtual bool load(const char *data, int size);
	void unload();

	const std::string& getName() { return _name; }
	void setName(std::string name) { _name = name; }
	ResHandle getHandle() { return _handle; }
	bool isLoaded() { return _loaded; }

protected:
	std::string          _name;
	ResHandle            _handle;

	bool                 _loaded;

	friend class ResourceManager;
};