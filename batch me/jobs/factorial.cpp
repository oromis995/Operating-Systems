#include <iostream>
using namespace std;
#include <chrono>
#include <thread>
#include <string>

int main () {
    int factorial = 10;
    for(int counter = 1; counter <= 9; counter++){ 
        factorial = factorial*(10 - counter);
        printf(" factorial = %d \n", factorial);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

