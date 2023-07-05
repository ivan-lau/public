#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

class CarID {
public:
    CarID() : id(boost::uuids::random_generator()()) {}

    std::string toString() const {
        return boost::uuids::to_string(id);
    }

    bool operator==(const CarID& other) const {
        return id == other.id;
    }

    bool operator!=(const CarID& other) const {
        return !(*this == other);
    }

private:
    boost::uuids::uuid id;
};

std::ostream& operator<<(std::ostream& os, const CarID& carID) {
    os << boost::uuids::to_string(carID.id);
    return os;
}

class Car {
public:
    Car() : id(CarID()) {}

    CarID getId() const {
        return id;
    }

private:
    CarID id;
};

int main() {
    Car car1;
    Car car2;

    std::cout << "Car 1 ID: " << car1.getId() << std::endl;
    std::cout << "Car 2 ID: " << car2.getId() << std::endl;

    if (car1.getId() == car2.getId()) {
        std::cout << "Car 1 and Car 2 have the same ID" << std::endl;
    } else {
        std::cout << "Car 1 and Car 2 have different IDs" << std::endl;
    }

    return 0;
}
