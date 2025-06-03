#include <windows.h>
#include <iostream>
#include <locale>
#include <cmath>
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

// Улучшенная функция управления танком с полиморфной проверкой коллизий
void ControlTank(Tank* tank, IInteractable** allObjects, int objectCount)
{
    if (tank->IsDestroyed()) return;

    int figX = tank->GetX(), figY = tank->GetY();
    while (1)
    {
        if (KEY_DOWN(VK_ESCAPE)) break;
        if (KEY_DOWN(VK_LEFT)) {
            figX -= tank->GetSpeed();
            tank->MoveTo(figX, figY);
            CheckCollisions(tank, allObjects, objectCount);
            Sleep(200);
        }
        if (KEY_DOWN(VK_RIGHT)) {
            figX += tank->GetSpeed();
            tank->MoveTo(figX, figY);
            CheckCollisions(tank, allObjects, objectCount);
            Sleep(200);
        }
        if (KEY_DOWN(VK_UP)) {
            figY -= tank->GetSpeed();
            tank->MoveTo(figX, figY);
            CheckCollisions(tank, allObjects, objectCount);
            Sleep(200);
        }
        if (KEY_DOWN(VK_DOWN)) {
            figY += tank->GetSpeed();
            tank->MoveTo(figX, figY);
            CheckCollisions(tank, allObjects, objectCount);
            Sleep(200);
        }
    }
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
            cout << "Tank Battle Demonstration" << endl;
            cout << "========================" << endl;

            // Создаем 10 мин и 10 ремонтных комплектов распределенных по всей области
            const int mineCount = 10;
            const int repairCount = 10;

            Mina* mines[mineCount];
            mines[0] = new Mina(300, 200);   // Верхний левый
            mines[1] = new Mina(800, 300);   // Верхний центр
            mines[2] = new Mina(1400, 250);  // Верхний правый
            mines[3] = new Mina(500, 600);   // Центр левый
            mines[4] = new Mina(1000, 650);  // Центр
            mines[5] = new Mina(1500, 600);  // Центр правый
            mines[6] = new Mina(400, 950);   // Низ левый
            mines[7] = new Mina(900, 1000);  // Низ центр
            mines[8] = new Mina(1300, 900);  // Низ правый
            mines[9] = new Mina(1700, 500);  // Правый край

            Remont* repairs[repairCount];
            repairs[0] = new Remont(200, 400);   // Левый край верх
            repairs[1] = new Remont(600, 450);   // Левый центр верх
            repairs[2] = new Remont(1100, 400);  // Центр верх
            repairs[3] = new Remont(1600, 350);  // Правый верх
            repairs[4] = new Remont(350, 750);   // Левый центр
            repairs[5] = new Remont(1200, 800);  // Центр
            repairs[6] = new Remont(1800, 750);  // Правый центр
            repairs[7] = new Remont(250, 1100);  // Левый низ
            repairs[8] = new Remont(750, 1150);  // Центр низ
            repairs[9] = new Remont(1450, 1100); // Правый низ

            // Создаем единый массив интерактивных объектов
            const int totalInteractiveObjects = mineCount + repairCount;
            IInteractable* interactiveObjects[totalInteractiveObjects];

            for (int i = 0; i < mineCount; i++) {
                interactiveObjects[i] = mines[i];
                mines[i]->Show();
            }

            for (int i = 0; i < repairCount; i++) {
                interactiveObjects[mineCount + i] = repairs[i];
                repairs[i]->Show();
            }

            cout << "Battlefield prepared with " << mineCount << " mines and " << repairCount << " repair kits positioned across the area!" << endl;
            Sleep(1000);

            // Создаем все танки на равном расстоянии
            Tank mainTank(700, 800, 250, 120, 120, 70, 150, 40);
            HeavyTank heavy(2000, 1000);
            LightTank light(900, 200);
            DamageTank damage(1200, 200);

            Tank* allTanks[4] = {&mainTank, &heavy, &light, &damage};
            string tankNames[4] = {"Main Tank", "Heavy Tank", "Light Tank", "Damage Tank"};

            for (int i = 0; i < 4; i++) {
                allTanks[i]->Show();
            }

            cout << "All tanks deployed at equal distances!" << endl;
            for (int i = 0; i < 4; i++) {
                cout << tankNames[i] << ": " << allTanks[i]->GetHealth() << " HP" << endl;
            }

            // Управление танками с полиморфной проверкой коллизий
            for (int tankIndex = 0; tankIndex < 4; tankIndex++) {
                cout << "\nPress " << (tankIndex + 1) << " to control " << tankNames[tankIndex] << endl;
                while (1)
                {
                    if (KEY_DOWN(49 + tankIndex)) break;
                    Sleep(100);
                }

                cout << tankNames[tankIndex] << " control active (arrows, ESC to stop)" << endl;
                ControlTank(allTanks[tankIndex], interactiveObjects, totalInteractiveObjects);
            }

            cout << "\n=== ROCKET ATTACK DEMONSTRATION ===" << endl;

            // Демонстрация ракетных атак с полиморфизмом
            Tank* targets[3] = {&light, &heavy, &damage};
            string targetNames[3] = {"Light Tank", "Heavy Tank", "Damage Tank"};
            int rocketPowers[3] = {50, 80, 70};
            int launchPositions[3][2] = {{100, 100}, {1800, 200}, {100, 1000}};

            for (int i = 0; i < 3; i++) {
                cout << "\nPress " << (i + 5) << " to launch rocket at " << targetNames[i] << endl;
                while (1)
                {
                    if (KEY_DOWN(53 + i)) break;
                    Sleep(100);
                }

                if (!targets[i]->IsDestroyed()) {
                    cout << "Rocket launched at " << targetNames[i] << "!" << endl;
                    Rocket* rocket = new Rocket(launchPositions[i][0], launchPositions[i][1],
                                               targets[i]->GetX() + targets[i]->GetBodyWidth()/2,
                                               targets[i]->GetY() + targets[i]->GetBodyHeight()/2,
                                               rocketPowers[i]);
                    rocket->Show();

                    for (int j = 0; j < 200 && rocket->IsActive(); j++) {
                        rocket->MoveToTarget();

                        if (rocket->CheckCollision(targets[i])) {
                            rocket->OnCollision(targets[i]);
                            break;
                        }
                        Sleep(50);
                    }
                    delete rocket;
                } else {
                    cout << targetNames[i] << " already destroyed!" << endl;
                }
            }

            // Демонстрация атаки на главный танк
            cout << "\nPress 8 to launch rocket at Main Tank" << endl;
            while (1)
            {
                if (KEY_DOWN(56)) break;
                Sleep(100);
            }

            if (!mainTank.IsDestroyed()) {
                cout << "Rocket launched at Main Tank!" << endl;
                Rocket* rocket4 = new Rocket(1500, 100, mainTank.GetX() + mainTank.GetBodyWidth()/2,
                              mainTank.GetY() + mainTank.GetBodyHeight()/2, 60);
                rocket4->Show();

                for (int i = 0; i < 200 && rocket4->IsActive(); i++) {
                    rocket4->MoveToTarget();

                    if (rocket4->CheckCollision(&mainTank)) {
                        rocket4->OnCollision(&mainTank);
                        break;
                    }
                    Sleep(50);
                }
                delete rocket4;
            } else {
                cout << "Main Tank already destroyed!" << endl;
            }

            cout << "\n=== FINAL BATTLE REPORT ===" << endl;
            for (int i = 0; i < 4; i++) {
                cout << tankNames[i] << ": " << (allTanks[i]->IsDestroyed() ? "DESTROYED" : "ALIVE")
                     << " (" << allTanks[i]->GetHealth() << " HP)" << endl;
            }

            cout << "\nPress ENTER to exit..." << endl;
            while (1)
            {
                if (KEY_DOWN(VK_RETURN)) break;
                Sleep(100);
            }

            // Очистка памяти
            for (int i = 0; i < mineCount; i++) delete mines[i];
            for (int i = 0; i < repairCount; i++) delete repairs[i];

            ReleaseDC(hwnd, hdc);
        }
    }

    system("pause");
    return 0;
}
