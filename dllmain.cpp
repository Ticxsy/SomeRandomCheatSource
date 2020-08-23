#include "stdafx.h"
#include "xor.hpp"
#include <cinttypes>
#include <math.h>

#define M_PI 3.14159265358979323846264338327950288419716939937510

ID3D11Device *device = nullptr;
ID3D11DeviceContext *immediateContext = nullptr;
ID3D11RenderTargetView *renderTargetView = nullptr;

HRESULT(*ResizeOriginal)(IDXGISwapChain *swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) = nullptr;

uint64_t base_address, uworld, ulevel, game_instance, player_controller, localplayer, actors, pawn, Rootcomp, PlayerState, localmesh;

PVOID jmp_rbx_0 = NULL;

DWORD_PTR w2sAddress;
DWORD_PTR UworldAddress;
DWORD_PTR vischeckAddress;
DWORD_PTR GetObjectNameAddr;

static int VisDist = 350;
int revise = 0;

int width;
int height;
static int BotID = 0;

BYTE* DiscordGetAsyncKeyStatePattern;
typedef HRESULT(__stdcall* DiscordGetAsyncKeyState_t) (int vKey);

HWND hWnd;

int actor_count = 0;
Vector3 Localpos;

bool in_menu = true;
bool menu = true;

bool Esp = false;
bool Box = false;
bool Snapline = false;
bool skeleton = false;
bool fovchanger = false;
static int camfov = 50;
bool vischeck = false;
bool getids = false;

int MyTeamId;

template<typename T>
T ReadMemory(DWORD_PTR address, const T& def = T())
{
	return *(T*)address;
}

struct FQuat
{
	float x;
	float y;
	float z;
	float w;
};

struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};

D3DXMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}

FTransform GetBoneIndex(DWORD_PTR mesh, int index)
{
	es:
	DWORD_PTR bonearray = ReadMemory<DWORD_PTR>(mesh + 0x410);

	if (IsBadReadPtr((DWORD_PTR*)bonearray, 8))
		goto es;

	return ReadMemory<FTransform>(bonearray + (index * 0x30));
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = ReadMemory<FTransform>(mesh + 0x1C0);

	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

__declspec(dllexport) HRESULT ResizeHook(IDXGISwapChain *swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
	ImGui_ImplDX11_Shutdown();
	renderTargetView->Release();
	immediateContext->Release();
	device->Release();
	device = nullptr;

	return ResizeOriginal(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
}

ImGuiWindow &BeginScene() {
	ImGui_ImplDX11_NewFrame();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin("##scene", nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);

	auto &io = ImGui::GetIO();
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);

	return *ImGui::GetCurrentWindow();
}

VOID EndScene(ImGuiWindow &window) {
	window.DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::Render();
}

std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

void DrawLine(int x1, int y1, int x2, int y2, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickness);
}

void DrawNewText(int x, int y, RGBA* color, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}

WNDPROC oriWndProc = NULL;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam) && menu == true)
	{
		return true;
	}
	return CallWindowProc(oriWndProc, hWnd, msg, wParam, lParam);
}

void Menu()
{
	static bool pressed = false;

	if (GetKeyState(VK_INSERT) & 0x8000)
		pressed = true;

	else if (!(GetKeyState(VK_INSERT) & 0x8000) && pressed) {
		menu = !menu;
		pressed = false;
	}

	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	if (menu)
	{
		io.MouseDrawCursor = true;
	}
	else
	{
		io.MouseDrawCursor = false;
	}

	if (menu)
	{
		ImGui::Begin(_xor_("Jordefin Private Internal").c_str(), &menu, ImVec2(420, 165), 1.0f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Checkbox(_xor_("Esp Enable").c_str(), &Esp);
		ImGui::Checkbox(_xor_("Box Esp").c_str(), &Box);
		ImGui::Checkbox(_xor_("Line Esp").c_str(), &Snapline);
		ImGui::Checkbox(_xor_("Skeleton Esp").c_str(), &skeleton);
		ImGui::SliderInt(_xor_("Max Visuals Distance").c_str(), &VisDist, 50, 500);
		ImGui::Checkbox(_xor_("Draw Actor id").c_str(), &getids);
		ImGui::InputInt(_xor_("Set Bot id").c_str(), &BotID);
		ImGui::End();
	}
}

void updateaddr()
{
	hWnd = FindWindow(0, _xor_(L"Fortnite  ").c_str());

	RECT rect;
	if (GetWindowRect(hWnd, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}

	uworld = ReadMemory<uint64_t>(UworldAddress);

	if (IsBadReadPtr((uint64_t*)uworld, 8))
		return;

	game_instance = ReadMemory<uint64_t>(uworld + 0x170);

	if (IsBadReadPtr((uint64_t*)game_instance, 8))
		return;

	uint64_t localplayers = ReadMemory<uint64_t>(game_instance + 0x38);

	if (IsBadReadPtr((uint64_t*)localplayers, 8))
		return;

	localplayer = ReadMemory<uint64_t>(localplayers + 0x0);

	if (IsBadReadPtr((uint64_t*)localplayer, 8))
		return;

	ulevel = ReadMemory<uint64_t>(uworld + 0x30);

	if (IsBadReadPtr((uint64_t*)ulevel, 8))
		return;

	actors = ReadMemory<uint64_t>(ulevel + 0x98);

	if (IsBadReadPtr((uint64_t*)actors, 8))
		return;

	actor_count = ReadMemory<int>(ulevel + 0xA0);

	player_controller = ReadMemory<uint64_t>(localplayer + 0x30);

	if (IsBadReadPtr((uint64_t*)player_controller, 8))
		return;

	if (actor_count < 300)
	{
		in_menu = true;
		return;
	}
	else
	{
		in_menu = false;
	}

	if (!in_menu)
	{
		pawn = ReadMemory<uint64_t>(player_controller + 0x298);

		if (IsBadReadPtr((uint64_t*)pawn, 8))
			return;

		Rootcomp = ReadMemory<uint64_t>(pawn + 0x130);

		if (IsBadReadPtr((uint64_t*)Rootcomp, 8))
			return;

		Localpos = ReadMemory<Vector3>(Rootcomp + 0x11C);
	}
}

void WorldToScreen(Vector3 world, Vector3* out) {
	if (jmp_rbx_0 == NULL)
	{
		jmp_rbx_0 = (PVOID)FindPattern((BYTE*)base_address, 0x10000000, (BYTE*)_xor_("\xFF\x23").c_str(), _xor_("xx").c_str(), 0);
	}
	auto W2S = reinterpret_cast<char(__fastcall*)(uint64_t controller, Vector3 world, Vector3 * out, char)>(w2sAddress);

	spoof_call(jmp_rbx_0, W2S, player_controller, world, out, (char)0);
}

bool isVis(Vector3 LocalHeadWorldPos, ULONG64 LocalPawn, ULONG64 ActorEnemy)
{
	if (jmp_rbx_0 == NULL)
	{
		jmp_rbx_0 = (PVOID)FindPattern((BYTE*)base_address, 0x10000000, (BYTE*)_xor_("\xFF\x23").c_str(), _xor_("xx").c_str(), 0);
	}

	typedef bool(__fastcall* CheckLineOfSightToActor_t)(Vector3* Origin, ULONG64 Actor, ULONG64 Unk);
	CheckLineOfSightToActor_t CheckLineOfSightToActor = (CheckLineOfSightToActor_t)(vischeckAddress);

	if (jmp_rbx_0 == NULL)
	{
		return false;
	}
	else
	{
		bool isVisible = spoof_call(jmp_rbx_0, CheckLineOfSightToActor, &LocalHeadWorldPos, ActorEnemy, LocalPawn);
		return isVisible;
	}
}

void WriteAngles(float TargetX, float TargetY)
{
	float x = TargetX / 6.666666666666667f;
	float y = TargetY / 6.666666666666667f;
	y = -(y);

	DWORD_PTR angle = player_controller + 0x418;
	*(float*)angle = y;

	DWORD_PTR anglex = player_controller + 0x418 + 0x4;
	*(float*)anglex = x;
}

FString GetObjName(UINT64* obj)
{
	return reinterpret_cast<FString(__fastcall*)(UINT64*)>(GetObjectNameAddr)(obj);
}

bool IsPlayer(uint64_t actor)
{
	auto ObjectName = GetObjName((uint64_t*)actor);

	size_t found1 = ObjectName.ToString().find(_xor_("PlayerPawn_Athena_C").c_str());

	if (found1 != std::string::npos)
		return true;

	size_t found2 = ObjectName.ToString().find(_xor_("BP_PlayerPawn_Athena_Phoebe_C").c_str());

	if (found2 != std::string::npos)
		return true;

	return false;
}

using f_present = HRESULT(__stdcall*)(IDXGISwapChain * pthis, UINT sync_interval, UINT flags);
f_present o_present = nullptr;

HRESULT __stdcall hk_present(IDXGISwapChain* pSwapChain, UINT sync_interval, UINT flags)
{
	if (!device) {
		pSwapChain->GetDevice(__uuidof(device), reinterpret_cast<PVOID*>(&device));
		device->GetImmediateContext(&immediateContext);

		ID3D11Texture2D* renderTarget = nullptr;
		pSwapChain->GetBuffer(0, __uuidof(renderTarget), reinterpret_cast<PVOID*>(&renderTarget));
		device->CreateRenderTargetView(renderTarget, nullptr, &renderTargetView);
		renderTarget->Release();

		ImGui_ImplDX11_Init(FindWindow(0, _xor_(L"Fortnite  ").c_str()), device, immediateContext);
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	immediateContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
	auto& window = BeginScene();

	updateaddr();

	if (!in_menu)
	{
		if (Esp)
		{
			if (pawn)
			{
				for (int i = 0; i < actor_count; i++)
				{
					uint64_t CurrentActor = ReadMemory<uint64_t>(actors + i * 0x8);

					if (IsBadReadPtr((uint64_t*)CurrentActor, 8))
						continue;

					uint64_t CurrentActorRootComponent = ReadMemory<uint64_t>(CurrentActor + 0x130);

					if (IsBadReadPtr((uint64_t*)CurrentActorRootComponent, 8))
						continue;

					int curactorid = ReadMemory<int>(CurrentActor + 0x18);

					if (getids)
					{
						Vector3 actorposw2s;
						WorldToScreen(ReadMemory<Vector3>(CurrentActorRootComponent + 0x11C), &actorposw2s);

						CHAR id[50];
						sprintf_s(id, _xor_("[%.d]").c_str(), curactorid);

						DrawNewText(actorposw2s.x, actorposw2s.y, &Col.red, id);
					}

					if (IsPlayer(CurrentActor))
					{
						/*uint64_t ActorPlayerState = ReadMemory<uint64_t>(CurrentActor + 0x238);

						if (ActorPlayerState == (uint64_t)nullptr || ActorPlayerState == -1 || ActorPlayerState == NULL)
							continue;

						if (IsBadReadPtr((uint64_t*)ActorPlayerState, 8))
							continue;

						int ActorTeamId = ReadMemory<int>(ActorPlayerState + 0xE50);*/

						//if (MyTeamId != ActorTeamId)
						//{
						if (CurrentActor == pawn)
							continue;

						uint64_t currentactormesh = ReadMemory<uint64_t>(CurrentActor + 0x278);

						if (IsBadReadPtr((uint64_t*)currentactormesh, 8))
							continue;

						Vector3 Headpos = GetBoneWithRotation(currentactormesh, 66);
						float distance = Localpos.Distance(Headpos) / 100.f;

						Vector3 bottom;
						Vector3 Headbox;
						Vector3 Aimpos;

						WorldToScreen(GetBoneWithRotation(currentactormesh, 0), &bottom);
						WorldToScreen(Vector3(Headpos.x, Headpos.y, Headpos.z + 15), &Headbox);
						WorldToScreen(Vector3(Headpos.x, Headpos.y, Headpos.z + 10), &Aimpos);

						if (bottom.x < 2.f)
							continue;

						if (bottom.y < 2.f)
							continue;

						float test = (float)width - 1;

						if (bottom.x > test)
							continue;

						float boxsize = (float)(bottom.y - Headbox.y);
						float boxwidth = boxsize / 3.0f;

						float dwpleftx = (float)bottom.x - boxwidth / 2.0f;
						float dwplefty = (float)bottom.y;

						if (Esp)
						{
							if (distance < VisDist)
							{
								char cName[64];
								sprintf_s(cName, _xor_("Player [%.fm]").c_str(), distance);
								DrawNewText(dwpleftx, dwplefty, &Col.white_, cName);
							}
						}

						if (Box)
						{
							if (distance < VisDist)
							{
								if (isVis(Localpos, pawn, CurrentActor))
								{
									ImGui::GetOverlayDrawList()->AddRect(ImVec2(dwpleftx, dwplefty), ImVec2(Headbox.x + boxwidth, Headbox.y), IM_COL32(255, 0, 0, 220));
								}
								else
								{
									ImGui::GetOverlayDrawList()->AddRect(ImVec2(dwpleftx, dwplefty), ImVec2(Headbox.x + boxwidth, Headbox.y), IM_COL32(0, 0, 255, 220));
								}
							}
						}

						if (Snapline)
						{
							if (distance < VisDist)
							{
								if (isVis(Localpos, pawn, CurrentActor))
								{
									DrawLine(width / 2, height, bottom.x, bottom.y, &Col.red, 1.5);
								}
								else
								{
									DrawLine(width / 2, height, bottom.x, bottom.y, &Col.blue, 1.5);
								}
							}
						}

						if (skeleton)
						{
							if (distance < VisDist)
							{
								Vector3 vHeadBoneOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 96), &vHeadBoneOut);

								Vector3 vHipOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 2), &vHipOut);

								Vector3 vNeckOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 65), &vNeckOut);

								Vector3 vUpperArmLeftOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 34), &vUpperArmLeftOut);

								Vector3 vUpperArmRightOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 91), &vUpperArmRightOut);

								Vector3 vLeftHandOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 35), &vLeftHandOut);

								Vector3 vRightHandOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 63), &vRightHandOut);

								Vector3 vLeftHandOut1;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 33), &vLeftHandOut1);

								Vector3 vRightHandOut1;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 60), &vRightHandOut1);

								Vector3 vRightThighOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 74), &vRightThighOut);

								Vector3 vLeftThighOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 67), &vLeftThighOut);

								Vector3 vRightCalfOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 75), &vRightCalfOut);

								Vector3 vLeftCalfOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 68), &vLeftCalfOut);

								Vector3 vLeftFootOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 69), &vLeftFootOut);

								Vector3 vRightFootOut;
								WorldToScreen(GetBoneWithRotation(currentactormesh, 76), &vRightFootOut);

								if (isVis(Localpos, pawn, CurrentActor))
								{
									DrawLine(vHipOut.x, vHipOut.y, vNeckOut.x, vNeckOut.y, &Col.red, 1.5);
									DrawLine(vUpperArmLeftOut.x, vUpperArmLeftOut.y, vNeckOut.x, vNeckOut.y, &Col.red, 1.5);
									DrawLine(vUpperArmRightOut.x, vUpperArmRightOut.y, vNeckOut.x, vNeckOut.y, &Col.red, 1.5);
									DrawLine(vLeftHandOut.x, vLeftHandOut.y, vUpperArmLeftOut.x, vUpperArmLeftOut.y, &Col.red, 1.5);
									DrawLine(vRightHandOut.x, vRightHandOut.y, vUpperArmRightOut.x, vUpperArmRightOut.y, &Col.red, 1.5);
									DrawLine(vLeftHandOut.x, vLeftHandOut.y, vLeftHandOut1.x, vLeftHandOut1.y, &Col.red, 1.5);
									DrawLine(vRightHandOut.x, vRightHandOut.y, vRightHandOut1.x, vRightHandOut1.y, &Col.red, 1.5);
									DrawLine(vLeftThighOut.x, vLeftThighOut.y, vHipOut.x, vHipOut.y, &Col.red, 1.5);
									DrawLine(vRightThighOut.x, vRightThighOut.y, vHipOut.x, vHipOut.y, &Col.red, 1.5);
									DrawLine(vLeftCalfOut.x, vLeftCalfOut.y, vLeftThighOut.x, vLeftThighOut.y, &Col.red, 1.5);
									DrawLine(vRightCalfOut.x, vRightCalfOut.y, vRightThighOut.x, vRightThighOut.y, &Col.red, 1.5);
									DrawLine(vLeftFootOut.x, vLeftFootOut.y, vLeftCalfOut.x, vLeftCalfOut.y, &Col.red, 1.5);
									DrawLine(vRightFootOut.x, vRightFootOut.y, vRightCalfOut.x, vRightCalfOut.y, &Col.red, 1.5);
								}
								else
								{
									DrawLine(vHipOut.x, vHipOut.y, vNeckOut.x, vNeckOut.y, &Col.blue, 1.5);
									DrawLine(vUpperArmLeftOut.x, vUpperArmLeftOut.y, vNeckOut.x, vNeckOut.y, &Col.blue, 1.5);
									DrawLine(vUpperArmRightOut.x, vUpperArmRightOut.y, vNeckOut.x, vNeckOut.y, &Col.blue, 1.5);
									DrawLine(vLeftHandOut.x, vLeftHandOut.y, vUpperArmLeftOut.x, vUpperArmLeftOut.y, &Col.blue, 1.5);
									DrawLine(vRightHandOut.x, vRightHandOut.y, vUpperArmRightOut.x, vUpperArmRightOut.y, &Col.blue, 1.5);
									DrawLine(vLeftHandOut.x, vLeftHandOut.y, vLeftHandOut1.x, vLeftHandOut1.y, &Col.blue, 1.5);
									DrawLine(vRightHandOut.x, vRightHandOut.y, vRightHandOut1.x, vRightHandOut1.y, &Col.blue, 1.5);
									DrawLine(vLeftThighOut.x, vLeftThighOut.y, vHipOut.x, vHipOut.y, &Col.blue, 1.5);
									DrawLine(vRightThighOut.x, vRightThighOut.y, vHipOut.x, vHipOut.y, &Col.blue, 1.5);
									DrawLine(vLeftCalfOut.x, vLeftCalfOut.y, vLeftThighOut.x, vLeftThighOut.y, &Col.blue, 1.5);
									DrawLine(vRightCalfOut.x, vRightCalfOut.y, vRightThighOut.x, vRightThighOut.y, &Col.blue, 1.5);
									DrawLine(vLeftFootOut.x, vLeftFootOut.y, vLeftCalfOut.x, vLeftCalfOut.y, &Col.blue, 1.5);
									DrawLine(vRightFootOut.x, vRightFootOut.y, vRightCalfOut.x, vRightCalfOut.y, &Col.blue, 1.5);
								}
							}
						}

						float headX = Aimpos.x - width / 2;
						float headY = Aimpos.y - height / 2;

						if (headX >= -50 && headX <= 50 && headY >= -50 && headY <= 50 && distance < 350) {
							if (GetAsyncKeyState(VK_XBUTTON1)) {
								WriteAngles(headX / 3.5f, headY / 3.5f);
							}
						}
						//}
					}
				}
			}
		}
	}

	Menu();
	EndScene(window);

	return o_present(pSwapChain, sync_interval, flags);
}

void initialize()
{
	base_address = (uint64_t)GetModuleHandleA(NULL);

	w2sAddress = (DWORD_PTR)FindPattern((BYTE*)base_address, 0x10000000, (BYTE*)_xor_("\x48\x83\xEC\x28\xE8\x00\x00\x00\x00\x48\x83\xC4\x28\xC3\xCC\xBA\xE9\x06\x33\x4C").c_str(), _xor_("xxxxx????xxxxxxxxxxx").c_str(), 0);
	vischeckAddress = (DWORD_PTR)FindPattern((BYTE*)base_address, 0x10000000, (BYTE*)_xor_("\x48\x89\x5c\x24\x00\x55\x56\x57\x48\x8d\x6c\x24\x00\x48\x81\xec\x00\x00\x00\x00\x48\x8b\x05\x00\x00\x00\x00\x48\x33\xc4\x48\x89\x45\x37\x49\x8b\xf8").c_str(), _xor_("xxxx?xxxxxxx?xxx????xxx????xxxxxxxxxx").c_str(), 0);
	GetObjectNameAddr = (DWORD_PTR)FindPattern((BYTE*)base_address, 0x10000000, (BYTE*)_xor_("\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x85\xD2\x75\x45\x33\xC0\x48\x89\x01\x48\x89\x41\x08\x8D\x50\x05\xE8\x00\x00\x00\x00\x8B\x53\x08\x8D\x42\x05\x89\x43\x08\x3B\x43\x0C\x7E\x08\x48\x8B\xCB\xE8\x00\x00\x00\x00\x48\x8B\x0B\x48\x8D\x15\x00\x00\x00\x00\x41\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\xC3\x48\x83\xC4\x20\x5B\xC3\x48\x8B\x42\x18").c_str(), _xor_("xxxxxxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxxxxx????xxxxxx????xx????x????xxxxxxxxxxxxx").c_str(), 0);

	auto btAddrUWorld = FindPattern((PBYTE)base_address, 0x10000000, (PBYTE)_xor_("\x48\x8b\x0d\x00\x00\x00\x00\x48\x85\xc9\x74\x30\xe8\x00\x00\x00\x00\x48\x8b\xf8").c_str(), _xor_("xxx????xxxxxx????xxx").c_str(), 0);
	auto btOffUWorld = *reinterpret_cast<uint32_t*>(btAddrUWorld + 3);

	UworldAddress = reinterpret_cast<DWORD_PTR>(btAddrUWorld + 7 + btOffUWorld);

	std::printf(_xor_("w2s: %p.\n").c_str(), w2sAddress - base_address);
	std::printf(_xor_("vischeck: %p.\n").c_str(), vischeckAddress - base_address);
	std::printf(_xor_("GetObjectNameAddr: %p.\n").c_str(), GetObjectNameAddr - base_address);
	std::printf(_xor_("UworldAddress: %p.\n").c_str(), UworldAddress - base_address);
	
	uint64_t DiscordBase = (uint64_t)GetModuleHandleA(_xor_("DiscordHook64.dll").c_str());

	DiscordGetAsyncKeyStatePattern = FindPattern((BYTE*)DiscordBase, 0x268000, (BYTE*)"\x40\x53\x48\x83\xEC\x20\x8B\xD9\xFF\x15\x00\x00\x00\x00", _xor_("xxxxxxxxxx????").c_str(), 0);
	std::printf(_xor_("DiscordGetAsyncKeyState: %p.\n").c_str(), DiscordGetAsyncKeyStatePattern);

	//hook discord overlay
	const auto pcall_present_discord = FindPattern((BYTE*)DiscordBase, 0x10000000, (BYTE*)_xor_("\xFF\x15\x00\x00\x00\x00\x8B\xD8\xE8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xEB\x10").c_str(), _xor_("xx????xxx????x????xx").c_str(), 0);
		
	if (!pcall_present_discord)
		return;

	const auto poriginal_present = reinterpret_cast<f_present*>(pcall_present_discord + *reinterpret_cast<int32_t*>(pcall_present_discord + 0x2) + 0x6);

	if (!*poriginal_present)
		return;

	o_present = *poriginal_present;

	*poriginal_present = hk_present;
}

VOID Main() {
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	Sleep(20000);
	HWND window = FindWindow(0, _xor_(L"Fortnite  ").c_str());

	IDXGISwapChain      *swapChain    = nullptr;
	ID3D11Device        *device       = nullptr;
	ID3D11DeviceContext *context      = nullptr;
	auto                 featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC sd           = { 0 };
	sd.BufferCount                    = 1;
	sd.BufferDesc.Format              = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage                    = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags                          = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.OutputWindow                   = window;
	sd.SampleDesc.Count               = 1;
	sd.Windowed                       = TRUE;

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context))) {
		MessageBox(0, _xor_(L"allah").c_str(), _xor_(L"akbar").c_str(), MB_ICONERROR);
		return;
	}

	auto table = *reinterpret_cast<PVOID **>(swapChain);

	auto present = table[8];
	auto resize = table[13];

	context->Release();
	device->Release();
	swapChain->Release();

	initialize();

	/*MH_CreateHook(present, PresentHooked, reinterpret_cast<PVOID *>(&PresentOriginal));
	MH_EnableHook(present);*/

	MH_Initialize();

	MH_CreateHook(resize, ResizeHook, reinterpret_cast<PVOID *>(&ResizeOriginal));
	MH_EnableHook(resize);

	oriWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		Main();
	}
	return TRUE;
}