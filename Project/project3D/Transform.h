#pragma once
#define GLM_SWIZZLE
#include <glm\glm.hpp>
#include <vector>



class Transform
{
public:
	Transform(const glm::vec3& position = { 0,0,0 }, const glm::vec3& scale = { 1,1,1 }, const glm::vec3& rotation = { 0,0,0 });
	Transform(const Transform& other);
	Transform& operator=(const Transform& other);

	const glm::mat4 GetLocal() const;
	const glm::mat4 GetWorld() const;

	//Return Enums
	enum ERROR_RETURNS
	{
		NO_ERROR_RETURN,		//No issues where found
		MAJOR_ERROR_RETURN,	//Major error (nullptr)
		MINOR_ERROR_RETURN,	//Minor error (transform exist already)

		RETURNS_MAX		//The number of different returns
	};
	/*Adds Child to the new parent, 
	removes Child from old parrent*/
	ERROR_RETURNS SetParent(Transform* parent, bool checkForExistence = false);
	/*Adds the transform to the array,
	if the not already there*/
	ERROR_RETURNS AddChild(Transform* child, bool checkForExistence = false);
	/*Removes the transform from the
	array of children*/
	ERROR_RETURNS RemoveChild(Transform* child);
	/*Removes the parent of this transform
	moveChildrenUP Moves children to the parent being removed*/
	ERROR_RETURNS RemoveParent(bool moveChildrenUp = true, bool checkForExistence = false);

	void UpdateTransform();

	virtual ~Transform();

	glm::vec3 m_positon;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;
	glm::vec3 m_forward;
	glm::vec3 m_up;
	glm::vec3 m_right;

protected:
	glm::mat4 m_localTransform;
	glm::mat4 m_worldTransform;

	std::vector<Transform*> m_children;
	Transform* m_parent;
};

