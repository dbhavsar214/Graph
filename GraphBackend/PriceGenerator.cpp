#include"PriceGenerator.h"

double PriceGenerator(double price, double std) {

        static std::mt19937 gen(std::random_device{}());

        std::normal_distribution<double> change(0.0, std);

        return price + change(gen);
}

void startPriceGenerator() {

    double price = 105.000;

    while (true) {
        price = PriceGenerator(price, 2);
        std::cout << price << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}