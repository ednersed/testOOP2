#pragma once

class Rocket; // Forward declaration

class Location {
protected:
    int X, Y;
public:
    Location(int InitX = 0, int InitY = 0);
    virtual ~Location();
    int GetX() const;
    int GetY() const;
    void SetX(int NewX);
    void SetY(int NewY);
    void SetPosition(int NewX, int NewY);
};

class Point : public Location {
protected:
    bool Visible;
public:
    Point(int InitX = 0, int InitY = 0);
    virtual ~Point();
    bool IsVisible() const;
    virtual void Show();
    virtual void Hide();
    virtual void MoveTo(int NewX, int NewY);
    virtual void Drag(int Step);
};

class Tank : public Point {
protected:
    int bodyWidth, bodyHeight, turretWidth, turretHeight;
    int gunLength, trackHeight;
    int health;
public:
    Tank(int InitX, int InitY, int bW, int bH, int tW, int tH, int gL, int trH);
    virtual ~Tank();
    virtual void Show();
    virtual void Hide();
    virtual void Drag(int step);
    virtual void TakeDamage(int dmg);
    int GetHealth() const;

    // Добавлено для double dispatch:
    virtual void OnHit(Rocket* rocket);
};

class HeavyTank : public Tank {
public:
    HeavyTank(int InitX, int InitY);
    ~HeavyTank();
    void Show() override;
    void TakeDamage(int dmg) override;
    void OnHit(Rocket* rocket) override;
};

class LightTank : public Tank {
public:
    LightTank(int InitX, int InitY);
    ~LightTank();
    void Show() override;
    void TakeDamage(int dmg) override;
    void OnHit(Rocket* rocket) override;
};

class RocketTank : public Tank {
public:
    RocketTank(int InitX, int InitY);
    ~RocketTank();
    void Show() override;
    void TakeDamage(int dmg) override;
    void OnHit(Rocket* rocket) override;
};

class StealthTank : public Tank {
public:
    StealthTank(int InitX, int InitY);
    ~StealthTank();
    void Show() override;
    void TakeDamage(int dmg) override;
    void OnHit(Rocket* rocket) override;
};

class Rocket : public Point {
    int power;
public:
    Rocket(int InitX, int InitY, int pwr);
    ~Rocket();
    void Show() override;
    void MoveTo(int NewX, int NewY) override;
    bool CheckCollision(Tank* t);
    int GetPower() const;
};
