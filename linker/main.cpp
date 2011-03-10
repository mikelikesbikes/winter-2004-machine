#include "main.h"

int main()
{  
  int length=3;
  bool hi;
  ifstream arr[7], input;
  ofstream out("output");
  arr[0].open("jen.txt");
  arr[1].open("mary.txt");
  arr[2].open("nappier.txt");
  arr[3].open("you");
  arr[4].open("us");
  arr[5].open("what'sup");
  
  hi=linker( arr, out, length);
  out.close();
  arr[0].close();
  arr[1].close();
  arr[2].close();
  arr[3].close();
  arr[4].close();
  arr[5].close();
}
