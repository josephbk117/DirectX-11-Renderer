#include "App.h"
#include "Imgui/imgui_impl_win32.h"
#include "Imgui/imgui_impl_dx11.h"
#include "Drawable/Model.h"
#include "Drawable/PhysicalSkybox.h"
#include "Drawable/Terrain.h"
#include "Image.h"

App::App()
	:
	wnd(1536, 864, "Dx11 Engine Test")
{
	//drawables.push_back(std::make_unique<Model>(wnd.Gfx(), "Resources\\Models\\TestScene.fbx"));
	//drawables.push_back(std::make_unique<Model>(wnd.Gfx(), "Resources\\Models\\nanosuit.obj"));
	//drawables.push_back(std::make_unique<Model>(wnd.Gfx(), "Resources\\Models\\BaseBrickCube.obj", 50.0f));
	//drawables.push_back(std::make_unique<Model>(wnd.Gfx(), "Resources\\Models\\Plane.obj"));
	models.push_back(std::make_unique<Model>(wnd.Gfx(), "Resources\\Models\\sponza.obj", 0.05f));

	Terrain::TerrainInitInfo terrainInfo;
	terrainInfo.baseMeshResolution = 32;
	terrainInfo.heightScale = 100.0f;
	terrainInfo.terrainUnitScale = 1000.0f;
	terrains.push_back(std::make_unique<Terrain>(wnd.Gfx(), "Resources\\Images\\testHeightMap3.png", terrainInfo));
	drawables.push_back(std::make_unique<PhysicalSkybox>(wnd.Gfx(), "Resources\\Models\\skyboxSphere.fbx"));

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 720.0f / 1280.0f, 0.5f, 50000.0f));
}

int App::Start()
{
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto ecode = Window::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}

		if (const auto e = wnd.kbd.ReadKey())
		{
			if (e->IsReleased() && e->GetCode() == VK_ESCAPE)
			{
				if (wnd.GetIsCursorEnabled())
				{
					wnd.DisableCursor();
				}
				else
				{
					wnd.EnableCursor();
				}
			}
		}

		if (!wnd.GetIsCursorEnabled())
		{
			const float moveInc = 0.03f;

			if (wnd.kbd.KeyIsPressed('W'))
			{
				wnd.cam.Translate({ 0.0f, 0.0f, moveInc });
			}
			if (wnd.kbd.KeyIsPressed('S'))
			{
				wnd.cam.Translate({ 0.0f, 0.0f, -moveInc });
			}
			if (wnd.kbd.KeyIsPressed('A'))
			{
				wnd.cam.Translate({ -moveInc, 0.0f, 0.0f });
			}
			if (wnd.kbd.KeyIsPressed('D'))
			{
				wnd.cam.Translate({ moveInc, 0.0f, 0.0f });
			}
			if (wnd.kbd.KeyIsPressed('Q'))
			{
				wnd.cam.Translate({ 0.0f, moveInc, 0.0f });
			}
			if (wnd.kbd.KeyIsPressed('E'))
			{
				wnd.cam.Translate({ 0.0f, -moveInc, 0.0f });
			}

			while (const auto delta = wnd.mouse.ReadRawDelta())
			{
				wnd.cam.Rotate(delta->x, delta->y);
			}
		}

		wnd.Gfx().SetCamera(wnd.cam.GetMatrix());
		wnd.Gfx().SetCameraPos(wnd.cam.GetPosition());

		DoFrame();
	}
}

App::~App()
{}

void App::DoFrame()
{
	auto dt = timer.Mark();
	wnd.Gfx().ClearBuffer(0.07f, 0.2f, 0.42f);

	for (auto& t : terrains)
	{
		t->Draw(wnd.Gfx());
	}

	for (auto& b : drawables)
	{
		b->Draw(wnd.Gfx());
	}

	for (auto& m : models)
	{
		m->Draw(wnd.Gfx());
	}


	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static bool showDemoWindow = false;

	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	models[0]->ShowWindow("DEBUG INFO");

	/*drawables[0]->ShowWindow("DEBUG INFO");
	drawables[1]->ShowWindow("DEBUG INFO 2");*/

	static_cast<Terrain*>(terrains[0].get())->ShowWindow("TERRAIN INFO");

	static_cast<PhysicalSkybox*>(drawables[0].get())->ShowWindow("SKY INFO");
	static_cast<PhysicalSkybox*>(drawables[0].get())->Update(wnd.Gfx());

	if (ImGui::Begin("INFO"))
	{
		ImGui::Text("Graphics device Info");

		Graphics::AdapterInfo info = wnd.Gfx().GetAdapterInfo(0);

		ImGui::Text("Vendor name : %s", info.desc.c_str());
		ImGui::Text("Vendor ID : %d", info.vendorId);
		ImGui::Text("Dedicated Video Memory : %d MB", info.dedicatedVideoMemory / 1000000);
		ImGui::Text("Dedicated System Memory : %d MB", info.dedicatedSystemMemory / 1000000);
		ImGui::Text("Shared System Memory : %d MB", info.sharedSystemMemory / 1000000);

		static unsigned int nativeScreenW = 0, nativeScreenH = 0;
		if (nativeScreenW == 0 || nativeScreenH == 0)
		{
			Window::GetNativeScreenResolution(nativeScreenW, nativeScreenH);
		}
		ImGui::Text("Native Screen Resolution : %d x %d", nativeScreenW, nativeScreenH);

		auto delta = *wnd.mouse.ReadRawDelta();
		ImGui::Text("Raw mouse input x : %d y : %d", delta.x, delta.y);
	}
	ImGui::End();


	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	wnd.Gfx().EndFrame();
}
