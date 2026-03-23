#include <iostream>
#include "Headers/Game.h"
#include "Headers/TriangleComponent.h"

int main()
{
    Game Game(L"MyCool3DApp", 800, 800);
    Game.AddComponent(std::make_unique<TriangleComponent>(&Game));
    Game.Init();
    Game.Run();
    std::cout << "hi" << std::endl;
    return 0;
}