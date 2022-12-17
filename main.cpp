#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include <math.h>
#include <cmath>
#include <sstream>

static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static float Sin(void*, int i) {
    return sinf(i * 0.1f);
}

static float Cos(void*, int i) {
    return cosf(i * 0.1f);
}

static float Sqrrt(void*, int i) {
    return sqrtf(i);
}

static float Saw(void*, int i) {
    return (i & 1) ? 1.0f : -1.0f;
}

static float Tan(void*, int i) {
    return tanf(i * 0.1f);
}

static float Log2(void*, int i) {
    return log2f(i * 0.1f);
}

int main(int, char**)
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 20.0f);

    bool show_sin_window = false;
    bool show_saw_window = false;
    bool show_avg_window = false;
    bool show_cos_window = false;
    bool show_tan_window = false;
    bool show_veltime_window = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ShowWindow(::GetConsoleWindow(), SW_HIDE);

        {
            static float display_count = 2;
            static float display_count1 = 2;
            static float display_count2 = 2;
            static float display_count3 = 2;
            static float display_count4 = 2;

            static bool animate = false;
            static float values[90] = {};
            static int values_offset = 0;
            static double refresh_time = 0.0;
            char overlay[32];
            float average = 0.0f;

            if (!animate || refresh_time == 0.0) {
                refresh_time = ImGui::GetTime();
            }
            
            while (refresh_time < ImGui::GetTime()) {
                static float phase = 0.0f;
                values[values_offset] = cosf(phase);
                values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
                phase += 0.10f * values_offset;
                refresh_time += 1.0f / 60.0f;
            }
            
            for (int n = 0; n < IM_ARRAYSIZE(values); n++) {
                average += values[n];
            }
        
            average /= (float)IM_ARRAYSIZE(values);

            ImGui::SetNextWindowSize(ImVec2(400, 800));
            ImGui::Begin("Physics And Math And Stuff IDK", NULL, ImGuiWindowFlags_NoResize);                          // Create a window called "Hello, world!" and append into it.
            ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::Checkbox("Average Window", &show_avg_window);

            ImGui::Checkbox("Sin Window", &show_sin_window);

            ImGui::Checkbox("Saw Window", &show_saw_window);

            ImGui::Checkbox("Cos Window", &show_cos_window);

            ImGui::Checkbox("Tan Window", &show_tan_window);

            ImGui::Checkbox("Vel/Time Window", &show_veltime_window);


            if (show_sin_window) {
                ImGui::SetNextWindowSize(ImVec2(400, 200));
                ImGui::Begin("Sin", NULL, ImGuiWindowFlags_NoResize);
                ImGui::SetNextItemWidth(ImGui::GetFontSize() * -12);

                ImGui::SliderFloat("Amount Sin", &display_count, 2, 1600);
                ImGui::PlotLines("Sin", Sin, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));

                ImGui::End();
            }

            if (show_saw_window) {
                ImGui::SetNextWindowSize(ImVec2(400, 200));
                ImGui::Begin("Saw", NULL, ImGuiWindowFlags_NoResize);
                ImGui::SetNextItemWidth(ImGui::GetFontSize() * -12);

                ImGui::SliderFloat("Amount Saw", &display_count1, 2, 70);
                ImGui::PlotLines("Saw", Saw, NULL, display_count1, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));

                ImGui::End();
            }

            if (show_avg_window) {
                ImGui::SetNextWindowSize(ImVec2(400, 200));
                ImGui::Begin("Average", NULL, ImGuiWindowFlags_NoResize);
                ImGui::SetNextItemWidth(ImGui::GetFontSize() * -12);

                ImGui::Checkbox("Enable Animations", &animate);
                sprintf(overlay, "%f", average);
                ImGui::PlotLines("Average", values, IM_ARRAYSIZE(values), values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 80.0f));

                ImGui::End();
            }

            if (show_cos_window) {
                ImGui::SetNextWindowSize(ImVec2(400, 200));
                ImGui::Begin("Cos", NULL, ImGuiWindowFlags_NoResize);
                ImGui::SetNextItemWidth(ImGui::GetFontSize() * -12);

                ImGui::SliderFloat("Amount Cos", &display_count2, 2, 1600);
                ImGui::PlotLines("Cos", Cos, NULL, display_count2, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));

                ImGui::End();
            }

            if (show_tan_window) {
                ImGui::SetNextWindowSize(ImVec2(400, 200));
                ImGui::Begin("Tan", NULL, ImGuiWindowFlags_NoResize);
                ImGui::SetNextItemWidth(ImGui::GetFontSize() * -12);

                ImGui::SliderFloat("Amount Tan", &display_count3, 2, 400);
                ImGui::PlotLines("Tan", Tan, NULL, display_count3, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));

                ImGui::End();
            }

            if (show_veltime_window) {
                ImGui::SetNextWindowSize(ImVec2(400, 200));
                ImGui::Begin("Log2", NULL, ImGuiWindowFlags_NoResize);
                ImGui::SetNextItemWidth(ImGui::GetFontSize() * -12);

                ImGui::SliderFloat("Log2 Amount", &display_count4, 2, 400);
                ImGui::PlotLines("Log2", Log2, NULL, display_count4, 0, NULL, -1.0f, 1.0f, ImVec2(0, 80));

                ImGui::End();
            }

            ImGui::End();
        }

        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*clear_color.w*255.0f), (int)(clear_color.y*clear_color.w*255.0f), (int)(clear_color.z*clear_color.w*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}


bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
