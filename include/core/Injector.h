#pragma once

class Injector
{
public:
	Injector() {}

protected:
	virtual ~Injector() {}
	virtual bool inject() = 0;
	virtual void log() = 0;
};

