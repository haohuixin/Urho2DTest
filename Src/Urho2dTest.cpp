#include "Urho2dTest.h"

#include <Urho3D/Scene/SceneEvents.h>

#include <Urho3D/Core/CoreEvents.h>

#include <Urho3D/Scene/SceneEvents.h>

#include <Urho3D/Scene/SceneEvents.h>


URHO3D_DEFINE_APPLICATION_MAIN(Urho2DTest)

Urho2DTest::Urho2DTest(Context* context) :
	Application(context), yaw_(0.0f),
	pitch_(0.f),
	useMouseMode_(MM_ABSOLUTE),
	screenJoystickIndex_(M_MAX_UNSIGNED),
	screenJoystickSettingsIndex_(M_MAX_UNSIGNED),
	paused_(false)
{
	context->RegisterFactory<CameraController>();



}



void Urho2DTest::Setup()
{
	engineParameters_[EP_WINDOW_TITLE] = GetTypeName();
	engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetAppPreferencesDir("urho3d", "logs") + GetTypeName() + ".log";
	engineParameters_[EP_FULL_SCREEN] = false;
	engineParameters_[EP_HEADLESS] = false;
	engineParameters_[EP_SOUND] = false;
	engineParameters_[EP_WINDOW_HEIGHT] = 900;
	engineParameters_[EP_WINDOW_WIDTH] = 1440;

	// Construct a search path to find the resource prefix with two entries:
	// The first entry is an empty path which will be substituted with program/bin directory -- this entry is for binary when it is still in build tree
	// The second and third entries are possible relative paths from the installed program/bin directory to the asset directory -- these entries are for binary when it is in the Urho3D SDK installation location
	if (!engineParameters_.Contains(EP_RESOURCE_PREFIX_PATHS))
		engineParameters_[EP_RESOURCE_PREFIX_PATHS] = ";../../Resources;./Resources";


	
}

void Urho2DTest::Start()
{




	// Create console and debug HUD
	CreateConsoleAndDebugHud();
	

	InitMouseMode(MM_FREE);

	auto l10n = LOCALIZATION;

	l10n->LoadJSONFile("Lang.json");



	
	CreateUI();
	CreateScene();
	SetupViewport();
	
	// Subscribe key down event
	SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Urho2DTest, HandleKeyDown));
	// Subscribe key up event
	SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(Urho2DTest, HandleKeyUp));
	// Subscribe scene update event
	SubscribeToEvent(E_SCENEUPDATE, URHO3D_HANDLER(Urho2DTest, HandleSceneUpdate));

	SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Urho2DTest, HandleUpdate));

}

void Urho2DTest::Stop()
{
	engine_->DumpResources(true);
}

void Urho2DTest::InitMouseMode(MouseMode mode)
{

	useMouseMode_ = mode;

	Input* input = GetSubsystem<Input>();

	if (GetPlatform() != "Web")
	{
		if (useMouseMode_ == MM_FREE)
			input->SetMouseVisible(true);

		Console* console = GetSubsystem<Console>();
		if (useMouseMode_ != MM_ABSOLUTE)
		{
			input->SetMouseMode(useMouseMode_);
			if (console && console->IsVisible())
				input->SetMouseMode(MM_ABSOLUTE, true);
		}
	}
	else
	{
		input->SetMouseVisible(true);
		SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(Urho2DTest, HandleMouseModeRequest));
		SubscribeToEvent(E_MOUSEMODECHANGED, URHO3D_HANDLER(Urho2DTest, HandleMouseModeChange));
	}
}

void Urho2DTest::CreateConsoleAndDebugHud()
{

	// Get default style
	ResourceCache* cache = GetSubsystem<ResourceCache>();
	XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

	// Create console
	Console* console = engine_->CreateConsole();
	console->SetDefaultStyle(xmlFile);
	console->GetBackground()->SetOpacity(0.8f);

	// Create debug HUD.
	DebugHud* debugHud = engine_->CreateDebugHud();
	debugHud->SetDefaultStyle(xmlFile);
}

void Urho2DTest::HandleMouseModeRequest(StringHash eventType, VariantMap & eventData)
{

	Console* console = GetSubsystem<Console>();
	if (console && console->IsVisible())
		return;
	Input* input = GetSubsystem<Input>();
	if (useMouseMode_ == MM_ABSOLUTE)
		input->SetMouseVisible(false);
	else if (useMouseMode_ == MM_FREE)
		input->SetMouseVisible(true);
	input->SetMouseMode(useMouseMode_);





	
}

void Urho2DTest::HandleMouseModeChange(StringHash eventType, VariantMap & eventData)
{


	Input* input = GetSubsystem<Input>();
	bool mouseLocked = eventData[MouseModeChanged::P_MOUSELOCKED].GetBool();
	input->SetMouseVisible(!mouseLocked);
}

void Urho2DTest::HandleKeyDown(StringHash eventType, VariantMap & eventData)
{

	using namespace KeyDown;

	int key = eventData[P_KEY].GetInt();

	// Toggle console with F1
	if (key == KEY_F1)
		GetSubsystem<Console>()->Toggle();

	// Toggle debug HUD with F2
	else if (key == KEY_F2)
		GetSubsystem<DebugHud>()->ToggleAll();

	// Common rendering quality controls, only when UI has no focused element
	
		
	else if (!UI->GetFocusElement())
	{
		Renderer* renderer = GetSubsystem<Renderer>();

		// Preferences / Pause
		if (key == KEY_SELECT )
		{
			paused_ = !paused_;

			Input* input = GetSubsystem<Input>();
			if (screenJoystickSettingsIndex_ == M_MAX_UNSIGNED)
			{
				// Lazy initialization
				ResourceCache* cache = GetSubsystem<ResourceCache>();
				screenJoystickSettingsIndex_ = (unsigned)input->AddScreenJoystick(cache->GetResource<XMLFile>("UI/ScreenJoystickSettings_Samples.xml"), cache->GetResource<XMLFile>("UI/DefaultStyle.xml"));
			}
			else
				input->SetScreenJoystickVisible(screenJoystickSettingsIndex_, paused_);
		}

		// Texture quality
		else if (key == '1')
		{
			int quality = renderer->GetTextureQuality();
			++quality;
			if (quality > QUALITY_HIGH)
				quality = QUALITY_LOW;
			renderer->SetTextureQuality(quality);
		}

		// Material quality
		else if (key == '2')
		{
			int quality = renderer->GetMaterialQuality();
			++quality;
			if (quality > QUALITY_HIGH)
				quality = QUALITY_LOW;
			renderer->SetMaterialQuality(quality);
		}

		// Specular lighting
		else if (key == '3')
			renderer->SetSpecularLighting(!renderer->GetSpecularLighting());

		// Shadow rendering
		else if (key == '4')
			renderer->SetDrawShadows(!renderer->GetDrawShadows());

		// Shadow map resolution
		else if (key == '5')
		{
			int shadowMapSize = renderer->GetShadowMapSize();
			shadowMapSize *= 2;
			if (shadowMapSize > 2048)
				shadowMapSize = 512;
			renderer->SetShadowMapSize(shadowMapSize);
		}

		// Shadow depth and filtering quality
		else if (key == '6')
		{
			ShadowQuality quality = renderer->GetShadowQuality();
			quality = (ShadowQuality)(quality + 1);
			if (quality > SHADOWQUALITY_BLUR_VSM)
				quality = SHADOWQUALITY_SIMPLE_16BIT;
			renderer->SetShadowQuality(quality);
		}

		// Occlusion culling
		else if (key == '7')
		{
			bool occlusion = renderer->GetMaxOccluderTriangles() > 0;
			occlusion = !occlusion;
			renderer->SetMaxOccluderTriangles(occlusion ? 5000 : 0);
		}

		// Instancing
		else if (key == '8')
			renderer->SetDynamicInstancing(!renderer->GetDynamicInstancing());

		// Take screenshot
		//else if (key == '9')
		//{
		//	Graphics* graphics = GetSubsystem<Graphics>();
		//	Image screenshot(context_);
		//	graphics->TakeScreenShot(screenshot);
		//	// Here we save in the Data folder with date and time appended
		//	screenshot.SavePNG(GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Screenshot_" +
		//		Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png");
		//}

		
	}
}

void Urho2DTest::HandleKeyUp(StringHash eventType, VariantMap & eventData)
{


	using namespace KeyUp;

	int key = eventData[P_KEY].GetInt();

	// Close console (if open) or exit when ESC is pressed
	if (key == KEY_ESCAPE)
	{
		Console* console = GetSubsystem<Console>();
		if (console->IsVisible())
			console->SetVisible(false);
		else
		{
			if (GetPlatform() == "Web")
			{
				GetSubsystem<Input>()->SetMouseVisible(true);
				if (useMouseMode_ != MM_ABSOLUTE)
					GetSubsystem<Input>()->SetMouseMode(MM_FREE);
			}
			else
				engine_->Exit();
		}
	}
}

void Urho2DTest::HandleSceneUpdate(StringHash eventType, VariantMap & eventData)
{


	//if (touchEnabled_ && cameraNode_)
	//{
	//	Input* input = GetSubsystem<Input>();
	//	for (unsigned i = 0; i < input->GetNumTouches(); ++i)
	//	{
	//		TouchState* state = input->GetTouch(i);
	//		if (!state->touchedElement_)    // Touch on empty space
	//		{
	//			if (state->delta_.x_ || state->delta_.y_)
	//			{
	//				Camera* camera = cameraNode_->GetComponent<Camera>();
	//				if (!camera)
	//					return;

	//				Graphics* graphics = GetSubsystem<Graphics>();
	//				yaw_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.x_;
	//				pitch_ += TOUCH_SENSITIVITY * camera->GetFov() / graphics->GetHeight() * state->delta_.y_;

	//				// Construct new orientation for the camera scene node from yaw and pitch; roll is fixed to zero
	//				cameraNode_->SetRotation(Quaternion(pitch_, yaw_, 0.0f));
	//			}
	//			else
	//			{
	//				// Move the cursor to the touch position
	//				Cursor* cursor = GetSubsystem<UI>()->GetCursor();
	//				if (cursor && cursor->IsVisible())
	//					cursor->SetPosition(state->position_);
	//			}
	//		}
	//	}
	//}
}
void Urho2DTest::HandleUpdate(StringHash eventType, VariantMap & eventData)
{
	static float step = 1.f;
	static Sprite* spr = 0;
	
	if(!spr)
	spr = UI_ROOT->GetChildStaticCast<Sprite>(L"123");
	
	step += 0.005f * step;
	if (spr->GetRotation() > 360.f * 8)step = -1.f;
	else if (spr->GetRotation() < 0.f)step = 1.f;

	spr->SetRotation(spr->GetRotation() + step);

}
void Urho2DTest::CreateUI()
{

	auto root = UI_ROOT;

	

	auto style = CACHE->GetResource<XMLFile>("UI/DefaultStyle.xml");
	root->SetDefaultStyle(style);
	
	
	


	SharedPtr<Sprite> spr(new Sprite(context_));

	auto tex = CACHE->GetResource<Texture2D>("Textures/logo.png");
	spr->SetName(L"123");
	spr->SetTexture(tex);
	spr->SetSize(IntVector2(tex->GetWidth(), tex->GetHeight()));
	spr->SetAlignment(HA_RIGHT, VA_BOTTOM);
	spr->SetHotSpot(IntVector2(tex->GetWidth()/2, tex->GetHeight()/2));
	spr->SetPosition(Vector2(-tex->GetWidth() / 2 - 50, -tex->GetHeight() - 50));
	
	root->AddChild(spr);



	SharedPtr<Text> loading(new Text(context_));

	loading->SetText("Loading...");
	loading->SetName("Loading");

	auto font = CACHE->GetResource<Font>("c:/windows/Fonts/msyh.ttf");

	loading->SetFont(font, 40);
	loading->SetPosition(IntVector2(100, -100));
	loading->SetAlignment(HA_LEFT, VA_BOTTOM);
	loading->SetColor(Color::WHITE);
	loading->SetAutoLocalizable(true);


	root->AddChild(loading);


#ifdef TRANSPARENT	
#undef TRANSPARENT	
#endif


	SharedPtr<ValueAnimation> vAni(new ValueAnimation(context_));
	vAni->SetKeyFrame(0.f, Color::WHITE);
	vAni->SetKeyFrame(2.f, Color::TRANSPARENT);
	vAni->SetKeyFrame(4.f, Color::WHITE);
	vAni->SetKeyFrame(4.5f, Color::WHITE);
	loading->SetAttributeAnimation("Color", vAni);
	
	SharedPtr<ValueAnimation> vAni2(new ValueAnimation(context_));
	vAni2->SetKeyFrame(0.f, Color::BLUE);
	vAni2->SetKeyFrame(2.f, Color::TRANSPARENT);
	vAni2->SetKeyFrame(4.f, Color::BLUE);
	vAni2->SetKeyFrame(4.5f, Color::BLUE);
	loading->SetAttributeAnimation("Effect Color", vAni2);
	

	loading->SetTextEffect(TextEffect::TE_STROKE);
	loading->SetEffectStrokeThickness(5);
	loading->SetEffectRoundStroke(true);
	loading->SetEffectColor(Color::BLUE);
	

	auto loc = LOCALIZATION;

	for (int i = 0; i < loc->GetNumLanguages(); i++)
	{
		auto btn = root->CreateChild<Button>(loc->GetLanguage());
		
		btn->SetPosition(100, 100 * i);
		btn->SetAlignment(HA_CENTER, VA_CENTER);
		btn->SetStyleAuto();
		btn->SetSize(100, 50);
		btn->SetFocusMode(FocusMode::FM_NOTFOCUSABLE);
		SubscribeToEvent(btn, E_RELEASED, [=](StringHash type, VariantMap& args)
		{
			loc->SetLanguage(i);
		});


		auto txt = btn->CreateChild<Text>();
		txt->SetSize(100, 50);
		txt->SetStyleAuto();
		txt->SetFont(font);
		txt->SetAlignment(HA_CENTER, VA_CENTER);
		loc->SetLanguage(i);
		txt->SetText(loc->Get("Lang"));

		auto tool = btn->CreateChild<ToolTip>();
		tool->SetStyleAuto();
		tool->SetSize(100, 100);

	}


	auto downlist = root->CreateChild<DropDownList>();
	downlist->SetStyleAuto();
	downlist->SetSize(200, 50);
	downlist->SetAlignment(HA_CENTER, VA_CENTER);
	downlist->SetResizePopup(true);
	downlist->GetPopup()->SetMaxHeight(200);
	downlist->SetFocusMode(FocusMode::FM_NOTFOCUSABLE);
	downlist->SetPosition(-200, 0);

	auto resos = GRAPHICS->GetResolutions(0);
	
	for (unsigned int i = 0; i < resos.Size(); i++)
	{
		



		auto txt = root->CreateChild<Text>();
		txt->SetSize(100, 100);
		txt->SetStyleAuto();
		
		txt->SetAlignment(HA_CENTER, VA_CENTER);

		txt->SetText(String("Lang1345gfdgfgdgfdgffffffffffffff") + String(i));
		txt->SetStyle("EditorEnumAttributeText");
		txt->SetFont(font);
		downlist->AddItem(txt);

		txt->SetText(String(resos[i].x_) + String("X") + String(resos[i].y_) 
		+ String("X") + String(resos[i].z_));
		txt->SetVar("IntVector3",  resos[i]);
	}

	SubscribeToEvent(downlist, E_ITEMSELECTED, [=](StringHash type, VariantMap& args)
	{
		auto ele = static_cast<DropDownList*>(args[ItemSelected::P_ELEMENT].GetPtr());
		auto v = ele->GetSelectedItem()->GetVar("IntVector3").GetIntVector3();
		GRAPHICS->SetMode(v.x_, v.y_);
		

	});


	

}

void Urho2DTest::CreateScene()
{
	scene_ = new Scene(context_);
	scene_->CreateComponent<Octree>();

	// Create camera node
	cameraNode_ = scene_->CreateChild("Camera");
	// Set camera's position
	cameraNode_->SetPosition(Vector3(0.0f, 0.0f, -10.0f));

	Camera* camera = cameraNode_->CreateComponent<Camera>();
	camera->SetOrthographic(true);

	Graphics* graphics = GetSubsystem<Graphics>();
	camera->SetOrthoSize((float)graphics->GetHeight() * PIXEL_SIZE);

	cameraNode_->CreateComponent<CameraController>();


	auto node = scene_->CreateChild("Node2");
	
	node->CreateComponent<StaticSprite2D>()
		->SetSprite(CACHE->GetResource<Sprite2D>("Urho2D/Ball.png"));
	auto txt = node->CreateComponent<Text3D>();
	
	txt->SetText("Loading...");
	txt->SetFont(CACHE->GetResource<Font>("c:/windows/Fonts/msyh.ttf"), 20);
	
}

void Urho2DTest::SetupViewport()
{
	Renderer* renderer = GetSubsystem<Renderer>();

	// Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
	SharedPtr<Viewport> viewport(new Viewport(context_, scene_, cameraNode_->GetComponent<Camera>()));
	
	renderer->SetViewport(0, viewport);
}


