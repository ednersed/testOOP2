#include <windows.h>
#include "Header.h"
#include <iostream>
using namespace std;

extern HDC hdc;
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

/*----------------------------------------*/
/*        МЕТОДЫ КЛАССА Location          */
/*----------------------------------------*/
Location::Location(int InitX, int InitY) { X = InitX; Y = InitY; }
Location::~Location() {}
int Location::GetX() const { return X; }
int Location::GetY() const { return Y; }
void Location::SetX(int NewX) { X = NewX; }
void Location::SetY(int NewY) { Y = NewY; }
void Location::SetPosition(int NewX, int NewY) { X = NewX; Y = NewY; }

/*----------------------------------------*/
/*        МЕТОДЫ КЛАССА Point             */
/*----------------------------------------*/
Point::Point(int InitX, int InitY) : Location(InitX, InitY) { Visible = false; }
Point::~Point() {}
bool Point::IsVisible() const { return Visible; }
void Point::Show()
{
    Visible = true;
    HPEN pen = CreatePen(PS_SOLID, 4, RGB(255, 0, 0));
    SelectObject(hdc, pen);
    MoveToEx(hdc, X - 5, Y, NULL); LineTo(hdc, X + 5, Y);
    MoveToEx(hdc, X, Y - 5, NULL); LineTo(hdc, X, Y + 5);
    Ellipse(hdc, X - 8, Y - 8, X + 8, Y + 8);
    DeleteObject(pen);
}
void Point::Hide()
{
    Visible = false;
    HPEN pen = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));
    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Rectangle(hdc, X - 10, Y - 10, X + 10, Y + 10);
    DeleteObject(brush); DeleteObject(pen);
}
void Point::MoveTo(int NewX, int NewY)
{
    Hide(); SetPosition(NewX, NewY); Show();
}
void Point::Drag(int Step)
{
    int FigX = GetX(), FigY = GetY();
    while (1)
    {
        if (KEY_DOWN(VK_ESCAPE)) break;
        if (KEY_DOWN(VK_LEFT)) { FigX -= Step; MoveTo(FigX, FigY); Sleep(200); }
        if (KEY_DOWN(VK_RIGHT)) { FigX += Step; MoveTo(FigX, FigY); Sleep(200); }
        if (KEY_DOWN(VK_DOWN)) { FigY += Step; MoveTo(FigX, FigY); Sleep(200); }
        if (KEY_DOWN(VK_UP)) { FigY -= Step; MoveTo(FigX, FigY); Sleep(200); }
    }
}

/*----------------------------------------*/
/*        МЕТОДЫ КЛАССА Tank              */
/*----------------------------------------*/
Tank::Tank(int InitX, int InitY, int bW, int bH, int tW, int tH, int gL, int trH)
    : Point(InitX, InitY)
{
    bodyWidth = bW; bodyHeight = bH; turretWidth = tW; turretHeight = tH;
    gunLength = gL; trackHeight = trH;
    health = 100;
}
Tank::~Tank() {}
void Tank::Drag(int step)
{
    int figX = GetX(), figY = GetY();
    while (1)
    {
        if (KEY_DOWN(VK_ESCAPE)) break;
        if (KEY_DOWN(VK_LEFT)) { figX -= step; MoveTo(figX, figY); Sleep(200); }
        if (KEY_DOWN(VK_RIGHT)) { figX += step; MoveTo(figX, figY); Sleep(200); }
        if (KEY_DOWN(VK_UP)) { figY -= step; MoveTo(figX, figY); Sleep(200); }
        if (KEY_DOWN(VK_DOWN)) { figY += step; MoveTo(figX, figY); Sleep(200); }
    }
}
void Tank::Show()
{
    Visible = true;
    HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH brush = CreateSolidBrush(RGB(0, 128, 0));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Rectangle(hdc, X, Y + bodyHeight, X + bodyWidth, Y + bodyHeight + trackHeight);
    DeleteObject(brush);
    brush = CreateSolidBrush(RGB(0, 100, 0)); SelectObject(hdc, brush);
    Rectangle(hdc, X + bodyWidth / 6, Y, X + bodyWidth - bodyWidth / 6, Y + bodyHeight);
    DeleteObject(brush);

    // Башня
    int turretTop = Y - turretHeight;
    brush = CreateSolidBrush(RGB(0, 80, 0)); SelectObject(hdc, brush);
    Rectangle(hdc, X + (bodyWidth - turretWidth) / 2, turretTop,
              X + (bodyWidth + turretWidth) / 2, turretTop + turretHeight);
    DeleteObject(brush);

    // ПУШКА из центра башни (универсально!)
    int turretCenterX = X + bodyWidth/2;
    int turretCenterY = turretTop + turretHeight/2;
    int barrelWidth = gunLength;
    int barrelHeight = 20;
    brush = CreateSolidBrush(RGB(70, 70, 70)); SelectObject(hdc, brush);
    Rectangle(hdc, turretCenterX, turretCenterY - barrelHeight/2,
              turretCenterX + barrelWidth, turretCenterY + barrelHeight/2);
    DeleteObject(brush);

    // Колёса/опоры
    brush = CreateSolidBrush(RGB(20,20,20)); SelectObject(hdc, brush);
    Ellipse(hdc, X + bodyWidth/6, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/6 + trackHeight/2, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth/2 - trackHeight/4, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/2 + trackHeight/4, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth - bodyWidth/6 - trackHeight/2, Y + bodyHeight + trackHeight/4,
        X + bodyWidth - bodyWidth/6, Y + bodyHeight + trackHeight - trackHeight/4);
    DeleteObject(brush); DeleteObject(pen);
}
void Tank::Hide()
{
    Visible = false;
    HPEN blackPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    SelectObject(hdc, blackPen); SelectObject(hdc, blackBrush);
    Rectangle(hdc, X - 30, Y - turretHeight - 30,
        X + bodyWidth + gunLength + 40, Y + bodyHeight + trackHeight + 30);
    DeleteObject(blackBrush); DeleteObject(blackPen);
}
void Tank::TakeDamage(int dmg)
{
    health -= dmg;
    if (health < 0) health = 0;
}
int Tank::GetHealth() const { return health; }
// Double dispatch (матрица переходов)
void Tank::OnHit(Rocket* rocket) {
    TakeDamage(rocket->GetPower());
}

/*-------------------- HEAVY TANK ---------------------*/
HeavyTank::HeavyTank(int InitX, int InitY)
    : Tank(InitX, InitY, 300, 130, 130, 80, 170, 45)
{
    health = 150;
}
HeavyTank::~HeavyTank() {}
void HeavyTank::Show()
{
    HPEN pen = CreatePen(PS_SOLID, 3, RGB(50, 0, 0));
    HBRUSH brush = CreateSolidBrush(RGB(100, 50, 50));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Rectangle(hdc, X, Y + bodyHeight, X + bodyWidth, Y + bodyHeight + trackHeight);
    DeleteObject(brush);
    brush = CreateSolidBrush(RGB(90, 30, 30)); SelectObject(hdc, brush);
    Rectangle(hdc, X + bodyWidth / 6, Y, X + bodyWidth - bodyWidth / 6, Y + bodyHeight);
    DeleteObject(brush);

    int turretTop = Y - turretHeight;
    brush = CreateSolidBrush(RGB(80, 20, 20)); SelectObject(hdc, brush);
    Rectangle(hdc, X + (bodyWidth - turretWidth)/2, turretTop,
              X + (bodyWidth + turretWidth)/2, turretTop + turretHeight);
    DeleteObject(brush);

    // ПУШКА из центра башни
    int turretCenterX = X + bodyWidth/2;
    int turretCenterY = turretTop + turretHeight/2;
    int barrelWidth = gunLength;
    int barrelHeight = 28;
    brush = CreateSolidBrush(RGB(110, 60, 60)); SelectObject(hdc, brush);
    Rectangle(hdc, turretCenterX, turretCenterY - barrelHeight/2,
              turretCenterX + barrelWidth, turretCenterY + barrelHeight/2);
    DeleteObject(brush);

    brush = CreateSolidBrush(RGB(40,20,20)); SelectObject(hdc, brush);
    Ellipse(hdc, X + bodyWidth/6, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/6 + trackHeight/2, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth/2 - trackHeight/4, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/2 + trackHeight/4, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth - bodyWidth/6 - trackHeight/2, Y + bodyHeight + trackHeight/4,
        X + bodyWidth - bodyWidth/6, Y + bodyHeight + trackHeight - trackHeight/4);
    DeleteObject(brush); DeleteObject(pen);
}
void HeavyTank::TakeDamage(int dmg)
{
    health -= dmg / 2;
    if (health < 0) health = 0;
}
void HeavyTank::OnHit(Rocket* rocket) {
    TakeDamage(rocket->GetPower() / 2);
}

/*-------------------- LIGHT TANK ---------------------*/
LightTank::LightTank(int InitX, int InitY)
    : Tank(InitX, InitY, 180, 80, 70, 40, 100, 22)
{
    health = 80;
}
LightTank::~LightTank() {}
void LightTank::Show()
{
    HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 128));
    HBRUSH brush = CreateSolidBrush(RGB(100, 100, 180));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Rectangle(hdc, X, Y + bodyHeight, X + bodyWidth, Y + bodyHeight + trackHeight);
    DeleteObject(brush);
    brush = CreateSolidBrush(RGB(80, 80, 180)); SelectObject(hdc, brush);
    Rectangle(hdc, X + bodyWidth / 6, Y, X + bodyWidth - bodyWidth / 6, Y + bodyHeight);
    DeleteObject(brush);

    int turretTop = Y - turretHeight;
    brush = CreateSolidBrush(RGB(50, 50, 140)); SelectObject(hdc, brush);
    Rectangle(hdc, X + (bodyWidth - turretWidth)/2, turretTop,
              X + (bodyWidth + turretWidth)/2, turretTop + turretHeight);
    DeleteObject(brush);

    // ПУШКА из центра башни
    int turretCenterX = X + bodyWidth/2;
    int turretCenterY = turretTop + turretHeight/2;
    int barrelWidth = gunLength;
    int barrelHeight = 14;
    brush = CreateSolidBrush(RGB(120, 120, 200)); SelectObject(hdc, brush);
    Rectangle(hdc, turretCenterX, turretCenterY - barrelHeight/2,
              turretCenterX + barrelWidth, turretCenterY + barrelHeight/2);
    DeleteObject(brush);

    brush = CreateSolidBrush(RGB(60,60,100)); SelectObject(hdc, brush);
    Ellipse(hdc, X + bodyWidth/6, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/6 + trackHeight/2, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth/2 - trackHeight/4, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/2 + trackHeight/4, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth - bodyWidth/6 - trackHeight/2, Y + bodyHeight + trackHeight/4,
        X + bodyWidth - bodyWidth/6, Y + bodyHeight + trackHeight - trackHeight/4);
    DeleteObject(brush); DeleteObject(pen);
}
void LightTank::TakeDamage(int dmg)
{
    health -= dmg * 2;
    if (health < 0) health = 0;
}
void LightTank::OnHit(Rocket* rocket) {
    TakeDamage(rocket->GetPower() * 2);
}

/*-------------------- ROCKET TANK ---------------------*/
RocketTank::RocketTank(int InitX, int InitY)
    : Tank(InitX, InitY, 250, 110, 80, 60, 180, 30)
{
    health = 90;
}
RocketTank::~RocketTank() {}
void RocketTank::Show()
{
    HPEN pen = CreatePen(PS_SOLID, 3, RGB(100, 100, 0));
    HBRUSH brush = CreateSolidBrush(RGB(180, 180, 80));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Rectangle(hdc, X, Y + bodyHeight, X + bodyWidth, Y + bodyHeight + trackHeight);
    DeleteObject(brush);
    brush = CreateSolidBrush(RGB(180, 170, 60)); SelectObject(hdc, brush);
    Rectangle(hdc, X + bodyWidth / 6, Y, X + bodyWidth - bodyWidth / 6, Y + bodyHeight);
    DeleteObject(brush);

    int turretTop = Y - turretHeight;
    brush = CreateSolidBrush(RGB(130, 130, 30)); SelectObject(hdc, brush);
    Rectangle(hdc, X + (bodyWidth - turretWidth)/2, turretTop,
              X + (bodyWidth + turretWidth)/2, turretTop + turretHeight);
    DeleteObject(brush);

    // ПУШКА из центра башни
    int turretCenterX = X + bodyWidth/2;
    int turretCenterY = turretTop + turretHeight/2;
    int barrelWidth = gunLength;
    int barrelHeight = 18;
    brush = CreateSolidBrush(RGB(220, 200, 40)); SelectObject(hdc, brush);
    Rectangle(hdc, turretCenterX, turretCenterY - barrelHeight/2,
              turretCenterX + barrelWidth, turretCenterY + barrelHeight/2);
    DeleteObject(brush);

    brush = CreateSolidBrush(RGB(100, 100, 0)); SelectObject(hdc, brush);
    Ellipse(hdc, X + bodyWidth/6, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/6 + trackHeight/2, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth/2 - trackHeight/4, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/2 + trackHeight/4, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth - bodyWidth/6 - trackHeight/2, Y + bodyHeight + trackHeight/4,
        X + bodyWidth - bodyWidth/6, Y + bodyHeight + trackHeight - trackHeight/4);
    DeleteObject(brush); DeleteObject(pen);
}
void RocketTank::TakeDamage(int dmg)
{
    health -= dmg;
    if (health < 0) health = 0;
}
void RocketTank::OnHit(Rocket* rocket) {
    TakeDamage(rocket->GetPower());
}

/*-------------------- STEALTH TANK ---------------------*/
StealthTank::StealthTank(int InitX, int InitY)
    : Tank(InitX, InitY, 210, 80, 60, 25, 90, 15)
{
    health = 60;
}
StealthTank::~StealthTank() {}
void StealthTank::Show()
{
    HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 60, 0));
    HBRUSH brush = CreateSolidBrush(RGB(40, 80, 40));
    SelectObject(hdc, pen); SelectObject(hdc, brush);
    Rectangle(hdc, X, Y + bodyHeight, X + bodyWidth, Y + bodyHeight + trackHeight);
    DeleteObject(brush);
    brush = CreateSolidBrush(RGB(50, 90, 50)); SelectObject(hdc, brush);
    Rectangle(hdc, X + bodyWidth / 6, Y, X + bodyWidth - bodyWidth / 6, Y + bodyHeight);
    DeleteObject(brush);

    int turretTop = Y - turretHeight;
    brush = CreateSolidBrush(RGB(20, 40, 20)); SelectObject(hdc, brush);
    Rectangle(hdc, X + (bodyWidth - turretWidth)/2, turretTop,
              X + (bodyWidth + turretWidth)/2, turretTop + turretHeight);
    DeleteObject(brush);

    // ПУШКА из центра башни
    int turretCenterX = X + bodyWidth/2;
    int turretCenterY = turretTop + turretHeight/2;
    int barrelWidth = gunLength;
    int barrelHeight = 10;
    brush = CreateSolidBrush(RGB(50, 70, 50)); SelectObject(hdc, brush);
    Rectangle(hdc, turretCenterX, turretCenterY - barrelHeight/2,
              turretCenterX + barrelWidth, turretCenterY + barrelHeight/2);
    DeleteObject(brush);

    brush = CreateSolidBrush(RGB(30, 50, 30)); SelectObject(hdc, brush);
    Ellipse(hdc, X + bodyWidth/6, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/6 + trackHeight/2, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth/2 - trackHeight/4, Y + bodyHeight + trackHeight/4,
        X + bodyWidth/2 + trackHeight/4, Y + bodyHeight + trackHeight - trackHeight/4);
    Ellipse(hdc, X + bodyWidth - bodyWidth/6 - trackHeight/2, Y + bodyHeight + trackHeight/4,
        X + bodyWidth - bodyWidth/6, Y + bodyHeight + trackHeight - trackHeight/4);
    DeleteObject(brush); DeleteObject(pen);
}
void StealthTank::TakeDamage(int dmg)
{
    health -= dmg / 3;
    if (health < 0) health = 0;
}
void StealthTank::OnHit(Rocket* rocket) {
    TakeDamage(rocket->GetPower() / 3);
}

/*-------------------- Класс Rocket ---------------------*/
Rocket::Rocket(int InitX, int InitY, int pwr)
    : Point(InitX, InitY), power(pwr) {}
Rocket::~Rocket() {}

void Rocket::Show()
{
    Visible = true;

    // 1. Draw rocket body (long ellipse, light gray)
    HPEN penBody = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    HBRUSH brushBody = CreateSolidBrush(RGB(230, 230, 230));
    SelectObject(hdc, penBody); SelectObject(hdc, brushBody);
    Ellipse(hdc, X - 18, Y - 6, X + 18, Y + 6);
    DeleteObject(brushBody); DeleteObject(penBody);

    // 2. Draw rocket nose (red triangle)
    POINT nose[3] = {
        { X + 18, Y },
        { X + 10, Y - 8 },
        { X + 10, Y + 8 }
    };
    HPEN penNose = CreatePen(PS_SOLID, 1, RGB(200, 0, 0));
    HBRUSH brushNose = CreateSolidBrush(RGB(255, 0, 0));
    SelectObject(hdc, penNose); SelectObject(hdc, brushNose);
    Polygon(hdc, nose, 3);
    DeleteObject(brushNose); DeleteObject(penNose);

    // 3. Draw rocket tail (blue triangles)
    POINT tail1[3] = {
        { X - 18, Y },
        { X - 28, Y - 8 },
        { X - 18, Y - 4 }
    };
    POINT tail2[3] = {
        { X - 18, Y },
        { X - 28, Y + 8 },
        { X - 18, Y + 4 }
    };
    HPEN penTail = CreatePen(PS_SOLID, 1, RGB(0, 0, 180));
    HBRUSH brushTail = CreateSolidBrush(RGB(0, 0, 255));
    SelectObject(hdc, penTail); SelectObject(hdc, brushTail);
    Polygon(hdc, tail1, 3);
    Polygon(hdc, tail2, 3);
    DeleteObject(brushTail); DeleteObject(penTail);

    // 4. Draw flame (orange/yellow polygon)
    POINT flame[3] = {
        { X - 30, Y },
        { X - 25, Y - 5 },
        { X - 25, Y + 5 }
    };
    HPEN penFlame = CreatePen(PS_SOLID, 1, RGB(255, 140, 0));
    HBRUSH brushFlame = CreateSolidBrush(RGB(255, 200, 0));
    SelectObject(hdc, penFlame); SelectObject(hdc, brushFlame);
    Polygon(hdc, flame, 3);
    DeleteObject(brushFlame); DeleteObject(penFlame);
}
void Rocket::MoveTo(int NewX, int NewY)
{
    Hide();
    SetPosition(NewX, NewY);
    Show();
}
bool Rocket::CheckCollision(Tank* t)
{
    if (t->GetX() == X && t->GetY() == Y) {
        t->OnHit(this); // Double dispatch: матрица переходов
        return true;
    }
    return false;
}
int Rocket::GetPower() const { return power; }
