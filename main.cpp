// Chao Huang      Game Construction
//#include "manager.h"
#include "menuManager.h"
int main(int, char*[]) {
   try {
      //Manager manager;
      //manager.play();
   		MenuManager myMenu;
   		myMenu.play();

   }
   catch (const string& msg) { std::cout << msg << std::endl; }
   catch (...) {
      std::cout << "Oops, someone threw an exception!" << std::endl;
   }
   return 0;
}
