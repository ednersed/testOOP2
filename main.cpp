#include <windows.h>
#include <iostream>
#include <locale>
using namespace std;

#include "Header.h"

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

HDC hdc;

void SetConsoleSize(int width, int height)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    COORD bufSize = { (SHORT)width, (SHORT)height };
    SetConsoleScreenBufferSize(hOut, bufSize);
    SMALL_RECT winRect = { 0, 0, (SHORT)(width - 1), (SHORT)(height - 1) };
    SetConsoleWindowInfo(hOut, TRUE, &winRect);
    HWND hwnd = GetConsoleWindow();
    if (hwnd)
        MoveWindow(hwnd, 30, 30, width * 9, height * 16, TRUE);
}

void SetupConsoleFont() {
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = 16;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

int main()
{
    setlocale(LC_ALL, "English");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    SetupConsoleFont();
    system("Color 0F");
    SetConsoleSize(2000, 60);

    HWND hwnd = GetConsoleWindow();

    if (hwnd != NULL)
    {
        RECT r;
        GetWindowRect(hwnd, &r);
        hdc = GetDC(hwnd);

        if (hdc != 0)
        {
            cout << "Initialization successful. Preparing to draw the tank..." << endl;
            Sleep(1000);

            Tank tankObj(700, 800, 250, 120, 120, 70, 150, 40);
            tankObj.Show();
            cout << "Tank drawn at position (700, 800)." << endl;
            cout << "Press 1 to continue." << endl;
            while (1)
            {
                if (KEY_DOWN(49)) break;
                Sleep(100);
            }

            cout << "Moving the tank to position (700, 800)..." << endl;
            tankObj.MoveTo(700, 800);
            cout << "Press 2 to continue." << endl;
            while (1)
            {
                if (KEY_DOWN(50)) break;
                Sleep(100);
            }

            cout << "Tank control mode:" << endl;
            cout << "- Use arrow keys to move" << endl;
            cout << "- Press ESC to exit control mode" << endl;
            tankObj.Drag(30);
            cout << "Tank control finished. Press 3 to continue." << endl;
            while (1)
            {
                if (KEY_DOWN(51)) break;
                Sleep(100);
            }

            cout << "Location class demonstration..." << endl;
            Location locationObj(100, 100);
            cout << "Location object created at position ("
                << locationObj.GetX() << ", "
                << locationObj.GetY() << ")" << endl;

            locationObj.SetX(150);
            locationObj.SetY(150);

            cout << "Object coordinates changed to ("
                << locationObj.GetX() << ", "
                << locationObj.GetY() << ")" << endl;

            cout << "Press 4 to continue to tank hierarchies." << endl;
            while (1)
            {
                if (KEY_DOWN(52)) break;
                Sleep(100);
            }

            // ----------- ???????????? ???????? -----------
            int centerX = 1400;
            int centerY = 800;
            int tankSpacing = 320;

            cout << "Vertical hierarchy: HeavyTank and LightTank" << endl;
            HeavyTank heavy(centerX - tankSpacing, centerY);
            LightTank light(centerX, centerY);
            heavy.Show();
            light.Show();

            cout << "Press 5 to control HeavyTank (arrows, ESC to exit)." << endl;
            while (1)
            {
                if (KEY_DOWN(53)) break;
                Sleep(100);
            }
            heavy.Drag(30);

            cout << "Press 6 to control LightTank (arrows, ESC to exit)." << endl;
            while (1)
            {
                if (KEY_DOWN(54)) break;
                Sleep(100);
            }
            light.Drag(30);

            cout << "Press 7 to launch a rocket at HeavyTank." << endl;
            while (1)
            {
                if (KEY_DOWN(55)) break;
                Sleep(100);
            }
            Rocket rocket1(heavy.GetX(), heavy.GetY(), 30);
            rocket1.Show();
            Sleep(700);
            if (rocket1.CheckCollision(&heavy)) {
                cout << "HeavyTank current health: " << heavy.GetHealth() << endl;
            }
            Sleep(1000);

            // ----------- ??????? ???????? -----------
            cout << "Fan hierarchy: RocketTank and StealthTank" << endl;
            RocketTank rocketTank(centerX + tankSpacing, centerY);
            StealthTank stealthTank(centerX + 2 * tankSpacing, centerY);
            rocketTank.Show();
            stealthTank.Show();

            cout << "Press 8 to control RocketTank (arrows, ESC to exit)." << endl;
            while (1)
            {
                if (KEY_DOWN(56)) break;
                Sleep(100);
            }
            rocketTank.Drag(30);

            cout << "Press 9 to control StealthTank (arrows, ESC to exit)." << endl;
            while (1)
            {
                if (KEY_DOWN(57)) break;
                Sleep(100);
            }
            stealthTank.Drag(30);

            cout << "Press 0 to launch a rocket at StealthTank." << endl;
            while (1)
            {
                if (KEY_DOWN(48)) break;
                Sleep(100);
            }
            Rocket rocket2(stealthTank.GetX(), stealthTank.GetY(), 20);
            rocket2.Show();
            Sleep(700);
            if (rocket2.CheckCollision(&stealthTank)) {
                cout << "StealthTank current health: " << stealthTank.GetHealth() << endl;
            }
            Sleep(1000);

            cout << "Press ENTER to clear all tanks and exit..." << endl;
            while (1)
            {
                if (KEY_DOWN(VK_RETURN)) break;
                Sleep(100);
            }
            heavy.Hide();
            light.Hide();
            rocketTank.Hide();
            stealthTank.Hide();
            tankObj.Hide();

            ReleaseDC(hwnd, hdc);
        }
        else
        {
            cout << "Error: Failed to get the device context!" << endl;
        }
    }
    else
    {
        cout << "Error: Failed to get the console window handle!" << endl;
    }

    cout << "Program finished. Press any key to exit..." << endl;
    system("pause");
    return 0;
}
