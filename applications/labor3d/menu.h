#ifndef MENU_HH
#define MENU_HH

const int menu0 = 0;
const int menu1 = 256;
const int menu2 = 2 * 256;
const int menu3 = 3 * 256;
const int menu4 = 4 * 256;
const int menu5 = 5 * 256;
const int menu6 = 6 * 256;

void mkQMenu();
void mkQMenu(int i, vector<string> &items, vector<char> &key);

void DrawQMenu(const vector<string> &items, const vector<char> &key);

#endif
