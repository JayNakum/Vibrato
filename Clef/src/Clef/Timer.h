#pragma once

#include <chrono>
#include <string>
#include <iostream>

namespace Clef
{
	class Timer
	{
	public:
		Timer()
		{
			reset();
		}

		void reset()
		{
			m_start = std::chrono::high_resolution_clock::now();
		}

		float elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_start).count() * 0.001f * 0.001f * 0.001f;
		}

		float elapsedMillis()
		{
			return elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
	};

	class ScopedTimer
	{
	public:
		ScopedTimer(const std::string& name)
			: m_name(name) {}
		~ScopedTimer()
		{
			float time = m_timer.elapsedMillis();
			std::cout << "[TIMER] " << m_name << " - " << time << "ms\n";
		}

	private:
		std::string m_name;
		Timer m_timer;
	};
}
