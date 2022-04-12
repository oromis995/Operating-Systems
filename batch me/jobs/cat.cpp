#include <iostream>
using namespace std;
#include <chrono>
#include <thread>

int main () {
  int x = 200;
  printf("         __..--''``---....___   _..._    __\n");
  std::this_thread::sleep_for(std::chrono::milliseconds(x));
  printf(" /// //_.-'    .-/\";  `        ``<._  ``.''_ `. / // /\n");
  std::this_thread::sleep_for(std::chrono::milliseconds(x));
  printf("///_.-' _..--.'_    \\                    `( ) ) // //\n");
  std::this_thread::sleep_for(std::chrono::milliseconds(x));
  printf("/ (_..-' // (< _     ;_..__               ; `' / ///\n");
  std::this_thread::sleep_for(std::chrono::milliseconds(x));
  printf(" / // // //  `-._,_)' // / ``--...____..-' /// / //\n");
  std::this_thread::sleep_for(std::chrono::milliseconds(x));
}





