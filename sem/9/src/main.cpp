#include <iostream>

struct Creature {};

struct OceanCreature : public Creature {
    void swim() {
        std::cout << "i swim" << std::endl;
    }
};

struct SkyCreature : public Creature {
    void fly() {
        std::cout << "i fly" << std::endl;
    }
};

struct TerrestrialCreature : public Creature {
    void walk() {
        std::cout << "i walk" << std::endl;
    }
};

struct AmphibiousCreature : public OceanCreature, public TerrestrialCreature {};

struct Bird : public TerrestrialCreature, public SkyCreature {};

struct Waterfowl : public Bird, public OceanCreature {};

int main() {
}