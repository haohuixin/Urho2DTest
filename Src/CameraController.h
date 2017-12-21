#pragma once
#include <Urho3D\Urho3DAll.h>
#include "Urho3DAliases.h"

class CameraController:public LogicComponent
{

	URHO3D_OBJECT(CameraController,LogicComponent)

public:
	CameraController(Context* context);
	virtual void Update(float timeStep);

	int keyUp_, keyDown_, keyLeft_, keyRight_ ,keyZoomIn_, keyZoomOut_;

	float moveSpeed_;

};

