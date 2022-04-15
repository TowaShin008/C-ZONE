#include "GameObject.h"

GameObject::GameObject()
{

}

GameObject::~GameObject()
{

}

void GameObject::Initialize()
{

}

void GameObject::Update(const Vector3& arg_incrementValue)
{

}

void GameObject::Draw()
{

}

bool GameObject::IsCollision(GameObject* arg_otherObject)
{
	GameObject* otherObject = arg_otherObject;

	if (isDeadFlag || otherObject->GetIsDeadFlag() || otherObject->GetCharacterType() == characterType) {
		return false;
	}

	if (Collision::CheckSphereToSphere({ otherObject->GetPosition(),otherObject->GetCollisionRadius() },
		{ position,collisionRadius}))
	{
		Damage();
		otherObject->Damage();
		return true;
	}

	return false;
}

void GameObject::Damage()
{
	if (invisibleTime < 1)
	{
		hp--;
		if (invisibleTime <= 0)
		{
			invisibleTime = INVISIBLEMAXTIME;
		}
	}
	if (hp < 1)
	{
		isDeadFlag = true;
	}

}

void GameObject::CriticalDamage()
{
	if (invisibleTime < 1)
	{
		hp -= 50;
		if (invisibleTime <= 0)
		{
			invisibleTime = INVISIBLEMAXTIME;
		}
	}
	if (hp < 1)
	{
		isDeadFlag = true;
	}
}
