#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H


class GameObject{
public:

    bool isValid;

    virtual void render();

    virtual void update();

    virtual void keyboard(int key, int scancode, int action, int mods);

    virtual void mouseButton(int button, int action, int modifier);

    virtual void mouseMovement(double xpos, double ypos);

    virtual void setObjectOrder(int zIndex);

    int getObjectOrder();

    GameObject();

    virtual ~GameObject();

    virtual void setActive(bool isActive);

    bool getActive();

    bool checkIfCanExecuteCallback();

protected:
    bool isActive;
    int objectId;

private:
    int objectOrder;
    bool lastActiveState;

};

#endif