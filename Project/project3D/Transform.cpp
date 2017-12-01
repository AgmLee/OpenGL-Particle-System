#include "Transform.h"
#include <iostream>
#include <glm\gtx\transform.hpp>
#include <glm\gtx\quaternion.hpp>

//Local Function Definitions
bool CheckForExistingChild(const std::vector<Transform*> children, const Transform* input);

//Constructors
Transform::Transform(const glm::vec3 & position, const glm::vec3 & scale, const glm::vec3 & rotation)
{
	m_positon = position;
	m_scale = scale;
	m_rotation = rotation;
	m_parent = nullptr;
	
	m_forward = glm::vec3(0, 0, 1);
	m_right = glm::vec3(1, 0, 0);
	m_up = glm::vec3(0, 1, 0);

	m_children.resize(0);
}
Transform::Transform(const Transform & other)
{
	m_positon = other.m_positon;
	m_rotation = other.m_rotation;
	m_scale = other.m_scale;
	m_parent = nullptr;

	m_forward = glm::vec3(0, 0, 1);
	m_right = glm::vec3(1, 0, 0);
	m_up = glm::vec3(0, 1, 0);

	m_children.resize(0);
}
Transform & Transform::operator=(const Transform & other)
{
	m_positon = other.m_positon;
	m_rotation = other.m_rotation;
	m_scale = other.m_scale;
	m_parent = nullptr;

	m_forward = glm::vec3(0, 0, 1);
	m_right = glm::vec3(1, 0, 0);
	m_up = glm::vec3(0, 1, 0);

	m_children.resize(0);
	return *this;
}

const glm::mat4 Transform::GetLocal() const
{
	return m_localTransform;
}
const glm::mat4 Transform::GetWorld() const
{
	return m_worldTransform;
}

Transform::ERROR_RETURNS Transform::SetParent(Transform * parent, bool checkForExistence)
{
	if (parent != nullptr)
	{
		if (checkForExistence)
		{
			if (CheckForExistingChild(m_children, parent))
			{
				std::cerr << "Transform parent is Child: " << this << std::endl;
				return MINOR_ERROR_RETURN;
			}
		}
		if (m_parent != nullptr)
			m_parent->RemoveChild(this);

		m_parent = parent;
		parent->AddChild(this);
		return NO_ERROR_RETURN;
	}
	std::cerr << "Transform parent is NULL: " << this << std::endl;
	return MAJOR_ERROR_RETURN;
}
Transform::ERROR_RETURNS Transform::RemoveParent(bool moveChildrenUp, bool checkForExistence)
{
	if (m_parent != nullptr) 
	{
		m_parent->RemoveChild(this);
		if (moveChildrenUp)
		{
			for (unsigned int i = 0; i < m_children.size(); i++)
			{
				m_children[i]->SetParent(m_parent, checkForExistence);
				m_children[i] = nullptr;
			}
			m_children.resize(0);
		}
		m_parent = nullptr;
		return NO_ERROR_RETURN;
	}

	std::cerr << "Transfrom parent doesn\'t exist: " << this << std::endl;
	return MAJOR_ERROR_RETURN;
}

Transform::ERROR_RETURNS Transform::AddChild(Transform * child, bool checkForExistence)
{
	if (child != nullptr)
	{
		if (child != m_parent)
		{
			if (checkForExistence)
			{
				if (CheckForExistingChild(m_children, child))
					return MINOR_ERROR_RETURN;
			}
			m_children.push_back(child);
			return NO_ERROR_RETURN;
		}
		std::cerr << "Transfrom child is Parent: " << this << std::endl;
		return MINOR_ERROR_RETURN;
	}
	std::cerr << "Transform child is NULL: " << this << std::endl;
	return MAJOR_ERROR_RETURN;
}
Transform::ERROR_RETURNS Transform::RemoveChild(Transform * child)
{
	if (child != nullptr)
	{
		for (unsigned int i = 0; i < m_children.size(); i++)
		{
			if (m_children[i] == child)
			{
				m_children.erase(m_children.begin() + i);
				m_children.shrink_to_fit();
				return NO_ERROR_RETURN;
			}
		}
		std::cerr << "Transform child isn\'t a child: " << this << std::endl;
		return MINOR_ERROR_RETURN;
	}
	std::cerr << "Transfrom child is NULL: " << this << std::endl;
	return MAJOR_ERROR_RETURN;
}

void Transform::UpdateTransform()
{
	{
		glm::mat4 posMat;
		posMat[3] = glm::vec4(m_positon, 1.0f);
		glm::mat4 scaMat;
		scaMat[0][0] = m_scale.x;
		scaMat[1][1] = m_scale.y;
		scaMat[2][2] = m_scale.z;
		glm::quat rotation = glm::quat(m_rotation);
		glm::mat4 rotMat = glm::toMat4(rotation);
		m_localTransform = posMat * scaMat * rotMat;
	}

	if (m_parent != nullptr)
	{
		m_worldTransform = m_localTransform * m_parent->GetWorld();
	}
	else
	{
		m_worldTransform = m_localTransform;
	}

	m_right = m_worldTransform[2].xyz;
	m_up = m_worldTransform[1].xyz;
	m_forward = m_worldTransform[0].xyz;

	for (unsigned int i = 0; i < m_children.size(); i++)
	{
		m_children[i]->UpdateTransform();
	}
}

Transform::~Transform()
{
	for (unsigned int i = 0; i < m_children.size(); i++) 
	{
		if (m_parent != nullptr)
			m_children[i]->SetParent(m_parent);
		m_children[i] = nullptr;
	}
	m_children.resize(0);
	if (m_parent != nullptr)
	{
		m_parent->RemoveChild(this);
		m_parent = nullptr;
	}
}

//Local Functions
bool CheckForExistingChild(const std::vector<Transform*> children, const Transform* input)
{
	for (unsigned int i = 0; i < children.size(); i++)
	{
		if (children[i] == input)
			return true;
	}
	return false;
}