#include "CameraController.h"



CameraController::CameraController(Context* context):
	LogicComponent(context),keyUp_(KEY_W), keyDown_(KEY_S), keyLeft_(KEY_A), keyRight_(KEY_D),moveSpeed_(4.f),
	keyZoomIn_(KEY_Q),keyZoomOut_(KEY_E)
{
	
}

void CameraController::Update(float timeStep)
{

	auto input = INPUT;
	auto node = GetNode();
	

	if (input->GetKeyDown(keyUp_))
		node->Translate2D(Vector2::UP * moveSpeed_ * timeStep);
	else if (input->GetKeyDown(keyDown_))
		node->Translate2D(Vector2::DOWN * moveSpeed_ * timeStep);
	if (input->GetKeyDown(keyLeft_))
		node->Translate2D(Vector2::LEFT * moveSpeed_ * timeStep);
	else if (input->GetKeyDown(keyRight_))
		node->Translate2D(Vector2::RIGHT * moveSpeed_ * timeStep);
	if (input->GetKeyDown(keyZoomIn_))
	{
		auto camera = node->GetComponent<Camera>();
		camera->SetZoom(camera->GetZoom()*.99f);
	}
	else if (input->GetKeyDown(keyZoomOut_))
	{
		auto camera = node->GetComponent<Camera>();
		camera->SetZoom(camera->GetZoom()*1.01f);
	}

	if(input->GetKeyDown(KEY_Z))

		node->SetRotation2D(node->GetRotation2D() + 1.f);
	else if (input->GetKeyDown(KEY_C))

		node->SetRotation2D(node->GetRotation2D() + -1.f);
	if (input->GetKeyDown(KEY_X))

		node->SetRotation2D(0.f);
}
