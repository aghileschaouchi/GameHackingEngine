#include "../../include/core/Game.h"

ghe::Game& ghe::Game::operator=(const Game& other)
{
	if (this != &other)
	{
		m_baseAddress = other.m_baseAddress;
		m_hProcess = other.m_hProcess;
		m_pid = other.m_pid;
		m_hwnd = other.m_hwnd;
		m_processName = other.m_processName;
		m_baseModuleName = other.m_baseModuleName;
	}
	return *this;
}

ghe::Game& ghe::Game::operator=(Game&& other) noexcept
{
	if (this != &other)
	{
		m_baseAddress = std::move(other.m_baseAddress);
		m_hProcess = std::move(other.m_hProcess);
		m_pid = std::move(other.m_pid);
		m_hwnd = std::move(other.m_hwnd);
		m_processName = std::move(other.m_processName);
		m_baseModuleName = std::move(other.m_baseModuleName);
	}
	return *this;
}
