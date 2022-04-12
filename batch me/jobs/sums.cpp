#include <iostream>
using namespace std;
#include <chrono>
#include <thread>

int main () {
    int sum = 0;
    for(int counter = 0; counter <= 10; counter++){
        sum = sum + counter;
        printf("sum = %d \n", sum);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}
