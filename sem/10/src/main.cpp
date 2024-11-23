#include <iostream>
#include <memory>
#include <vector>

struct Creature {
    virtual void eat() = 0;
    virtual ~Creature() = default;
};

struct OceanCreature : virtual public Creature {
    void eat() override {
        std::cout << "ate another fish" << std::endl;
    }

    void swim() {
        std::cout << "i swim" << std::endl;
    }
};

struct SkyCreature : virtual public Creature {
    void eat() override {
        std::cout << "ate another bird" << std::endl;
    }

    void fly() {
        std::cout << "i fly" << std::endl;
    }
};

struct TerrestrialCreature : virtual public Creature {
    void eat() override {
        std::cout << "ate cow" << std::endl;
    }

    void walk() {
        std::cout << "i walk" << std::endl;
    }
};

struct AmphibiousCreature : public TerrestrialCreature, public OceanCreature {
    void eat() override {
        std::cout << "ate cow" << std::endl;
    }
};

struct Bird : public TerrestrialCreature, public SkyCreature {
    void eat() override {
        std::cout << "ate worm" << std::endl;
    }
};

struct Waterfowl : public Bird, public OceanCreature {
    void eat() override {
        std::cout << "ate frog" << std::endl;
    }
};

int main() {
    std::vector<Waterfowl> x = {
        Waterfowl(),
        Waterfowl(),
        Waterfowl(),
    };

    for (auto &c : x) {
        c.eat();
    }

    std::vector<std::unique_ptr<Creature>> y;
    y.push_back(std::make_unique<Waterfowl>());
    y.push_back(std::make_unique<AmphibiousCreature>());
    y.push_back(std::make_unique<Bird>());
    y.push_back(std::make_unique<SkyCreature>());

    for (auto &c : y) {
        c->eat();
    }
    // y.push_back(std::make_unique<Waterfowl>());
}