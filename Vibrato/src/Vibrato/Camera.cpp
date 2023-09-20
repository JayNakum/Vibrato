#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Clef/Input/Input.h"

using namespace Clef;

namespace Vibrato
{
	Camera::Camera(float verticalFOV, float nearClip, float farClip)
		: m_verticalFOV(verticalFOV), m_nearClip(nearClip), m_farClip(farClip)
	{
		m_forwardDirection = glm::vec3(0, 0, -1);
		m_position = glm::vec3(0, 0, 6);
	}

	bool Camera::onUpdate(float ts)
	{
		glm::vec2 mousePos = Input::getMousePosition();
		glm::vec2 delta = (mousePos - m_lastMousePosition) * 0.002f;
		m_lastMousePosition = mousePos;

		if (!Input::isMouseButtonDown(MouseButton::Right))
		{
			Input::setCursorMode(CursorMode::Normal);
			return false;
		}

		Input::setCursorMode(CursorMode::Locked);

		bool moved = false;

		constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
		glm::vec3 rightDirection = glm::cross(m_forwardDirection, upDirection);

		float speed = 5.0f;

		// Movement
		if (Input::isKeyDown(KeyCode::W))
		{
			m_position += m_forwardDirection * speed * ts;
			moved = true;
		}
		else if (Input::isKeyDown(KeyCode::S))
		{
			m_position -= m_forwardDirection * speed * ts;
			moved = true;
		}
		if (Input::isKeyDown(KeyCode::A))
		{
			m_position -= rightDirection * speed * ts;
			moved = true;
		}
		else if (Input::isKeyDown(KeyCode::D))
		{
			m_position += rightDirection * speed * ts;
			moved = true;
		}
		if (Input::isKeyDown(KeyCode::Q))
		{
			m_position -= upDirection * speed * ts;
			moved = true;
		}
		else if (Input::isKeyDown(KeyCode::E))
		{
			m_position += upDirection * speed * ts;
			moved = true;
		}

		// Rotation
		if (delta.x != 0.0f || delta.y != 0.0f)
		{
			float pitchDelta = delta.y * getRotationSpeed();
			float yawDelta = delta.x * getRotationSpeed();

			glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
																							glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
			m_forwardDirection = glm::rotate(q, m_forwardDirection);

			moved = true;
		}

		if (moved)
		{
			recalculateView();
		}

		return moved;
	}

	void Camera::onResize(uint32_t width, uint32_t height)
	{
		if (width == m_viewportWidth && height == m_viewportHeight)
			return;

		m_viewportWidth = width;
		m_viewportHeight = height;

		recalculateProjection();
	}

	float Camera::getRotationSpeed()
	{
		return 0.3f;
	}

	void Camera::recalculateProjection()
	{
		m_projection = glm::perspectiveFov(glm::radians(m_verticalFOV), (float)m_viewportWidth, (float)m_viewportHeight, m_nearClip, m_farClip);
		m_inverseProjection = glm::inverse(m_projection);
	}

	void Camera::recalculateView()
	{
		m_view = glm::lookAt(m_position, m_position + m_forwardDirection, glm::vec3(0, 1, 0));
		m_inverseView = glm::inverse(m_view);
	}

	glm::vec3 Camera::getRayDirection(float x, float y) const
	{
		glm::vec2 coord = { (float)x / (float)m_viewportWidth, (float)y / (float)m_viewportHeight };
		coord = coord * 2.0f - 1.0f; // -1 -> 1

		glm::vec4 target = m_inverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
		glm::vec3 rayDirection = glm::vec3(m_inverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
		return rayDirection;
	}
}
