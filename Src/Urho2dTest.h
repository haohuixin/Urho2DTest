#pragma once


#include <Urho3D\Urho3DAll.h>
#include "Urho3DAliases.h"
#include "CameraController.h"

class Urho2DTest :public Application
{

	URHO3D_OBJECT(Urho2DTest,Application)

public:
	Urho2DTest(Context* context);
	virtual void Setup();
	virtual void Start();
	virtual void Stop();


protected:

	virtual String GetScreenJoystickPatchString()const { return String::EMPTY; }

	void InitMouseMode(MouseMode mode);

	SharedPtr<Scene> scene_;
	SharedPtr<Node> cameraNode_;

	float yaw_;
	float pitch_;

	MouseMode useMouseMode_;

private:
	void CreateConsoleAndDebugHud();
	void HandleMouseModeRequest(StringHash eventType, VariantMap& eventData);
	/// Handle request for mouse mode change on web platform.
	void HandleMouseModeChange(StringHash eventType, VariantMap& eventData);
	/// Handle key down event to process key controls common to all samples.
	void HandleKeyDown(StringHash eventType, VariantMap& eventData);
	/// Handle key up event to process key controls common to all samples.
	void HandleKeyUp(StringHash eventType, VariantMap& eventData);
	/// Handle scene update event to control camera's pitch and yaw for all samples.
	void HandleSceneUpdate(StringHash eventType, VariantMap& eventData);

	void HandleUpdate(StringHash eventType, VariantMap& eventData);
	/// Handle touch begin event to initialize touch input on desktop platform.
	void CreateUI();

	void CreateScene();

	void SetupViewport();
	/// Screen joystick index for navigational controls (mobile platforms only).
	unsigned screenJoystickIndex_;
	/// Screen joystick index for settings (mobile platforms only).
	unsigned screenJoystickSettingsIndex_;
	/// Pause flag.
	bool paused_;
};

