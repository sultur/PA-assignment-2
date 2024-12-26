#include <iostream>
#include "batch_instance.hpp"
#include "util.hpp"

int main(int argc, char** argv) {
    auto inst = Instance::parse(std::cin);
    std::cout << inst;
}