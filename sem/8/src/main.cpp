#include <iostream>

struct Creature {
  protected:
    void swim() {
        std::cout << "i swim" << std::endl;
    }

    void walk() {
        std::cout << "i walk" << std::endl;
    }

    void fly() {
        std::cout << "i fly" << std::endl;
    }
};

struct OceanCreature : private Creature {
    using Creature::swim;
};

struct AmphibiousCreature : private Creature {
    using Creature::swim;
    using Creature::walk;
};

struct TerrestrialCreature : private Creature {
    using Creature::walk;
};

struct Bird : private Creature {
    using Creature::fly;
    using Creature::walk;
};

struct Waterfowl : private Creature {
    using Creature::fly;
    using Creature::swim;
    using Creature::walk;
};

int main() {
    Bird c;
}