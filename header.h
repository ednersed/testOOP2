#pragma once

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

// Базовый интерфейс для объектов, способных взаимодействовать
class IInteractable {
public:
    virtual ~IInteractable() = default;
    virtual bool CheckCollision(class Tank* tank) = 0;
    virtual void OnCollision(Tank* tank) = 0;
    virtual bool IsActive() const = 0;
};

class Tank : public Point, public IInteractable {
protected:
    int bodyWidth, bodyHeight, turretWidth, turretHeight;
    int gunLength, trackHeight;
    int health;
    int speed;
    bool isDestroyed;
public:
    Tank(int InitX, int InitY, int bW, int bH, int tW, int tH, int gL, int trH, int spd = 30);
    virtual ~Tank();
    virtual void Show();
    virtual void Hide();
    virtual void Drag(int step);
    virtual void TakeDamage(int dmg);
    void Heal(int amount);
    void Explode();

    // Реализация IInteractable для базового танка (танки не взаимодействуют друг с другом)
    virtual bool CheckCollision(Tank* tank) override { return false; }
    virtual void OnCollision(Tank* tank) override {}
    virtual bool IsActive() const override { return !isDestroyed; }

    int GetHealth() const;
    int GetSpeed() const;
    int GetBodyWidth() const;
    int GetBodyHeight() const;
    bool IsDestroyed() const;
};

class HeavyTank : public Tank {
private:
    int targetX, targetY;
public:
    HeavyTank(int InitX, int InitY);
    ~HeavyTank();
    void Show() override;
    void Hide() override;
    virtual void TakeDamage(int dmg);
    void SetTarget(int x, int y);
    void ShowTargetingSystem();
};

class LightTank : public Tank {
public:
    LightTank(int InitX, int InitY);
    ~LightTank();
    virtual void Show();
    virtual void TakeDamage(int dmg);
};

class DamageTank : public Tank {
public:
    DamageTank(int InitX, int InitY);
    ~DamageTank();
    virtual void Show();
    virtual void TakeDamage(int dmg);
};

// Ракета наследуется от Tank и реализует взаимодействие
class Rocket : public Tank {
private:
    int power;
    int directionX, directionY;
    bool isActive;
    int targetX, targetY;
public:
    Rocket(int InitX, int InitY, int targetX, int targetY, int pwr, int w = 36, int h = 12, int spd = 15);
    ~Rocket();
    virtual void Show() override;
    virtual void Hide() override;
    virtual void MoveTo(int NewX, int NewY) override;

    // Реализация IInteractable
    virtual bool CheckCollision(Tank* tank) override;
    virtual void OnCollision(Tank* tank) override;
    virtual bool IsActive() const override { return isActive; }

    void MoveToTarget();
    int GetPower() const;
    void Deactivate();
};

// Мина наследуется от Tank и реализует взаимодействие
class Mina : public Tank {
private:
    int damage;
    bool isExploded;
public:
    Mina(int InitX, int InitY, int dmg = 50, int w = 20, int h = 20);
    ~Mina();
    virtual void Show() override;
    virtual void Hide() override;

    // Реализация IInteractable
    virtual bool CheckCollision(Tank* tank) override;
    virtual void OnCollision(Tank* tank) override;
    virtual bool IsActive() const override { return !isExploded; }

    void Explode();
    int GetDamage() const;
    bool IsExploded() const;
};

// Ремонтный комплект наследуется от Tank и реализует взаимодействие
class Remont : public Tank {
private:
    int healAmount;
    bool isUsed;
public:
    Remont(int InitX, int InitY, int heal = 30, int w = 40, int h = 30);
    ~Remont();
    virtual void Show() override;
    virtual void Hide() override;

    // Реализация IInteractable
    virtual bool CheckCollision(Tank* tank) override;
    virtual void OnCollision(Tank* tank) override;
    virtual bool IsActive() const override { return !isUsed; }

    void Use();
    int GetHealAmount() const;
    bool IsUsed() const;
};

// Универсальная функция проверки коллизий с использованием полиморфизма
void CheckCollisions(Tank* activeTank, IInteractable** objects, int objectCount);
