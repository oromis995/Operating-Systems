#include <iostream>
using namespace std;
#include <chrono>
#include <thread>
int main () {
    for(int dotCounter = 0; dotCounter <= 49; dotCounter++){
        printf(".\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}
