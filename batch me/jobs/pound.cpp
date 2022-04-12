#include <iostream>
using namespace std;
#include <chrono>
#include <thread>

int main () {
    for(int counter = 0; counter < 100; counter++){
        printf("# \n");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}
