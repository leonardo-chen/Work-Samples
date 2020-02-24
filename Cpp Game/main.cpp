#include<conio.h>
#include<windows.h>
#include<iostream>using namespace std;bool finish;const int width = 50;

const int height = 20;

int x, y, ball_x, ball_y, score;

int tailX[100], tailY[100];
int nTail;
enum dir { STOP = 0, LEFT, RIGHT, UP, DOWN};

dir dir1;

void start()
{

finish = false;
dir1 = STOP;
x = width / 2;
y = height / 2; ball_x = rand() % width;
ball_y = rand() % height;
score = 0;
}

void Make_area()
{
system(“cls”);
for (int i = 0; i < width+10; i++)
cout << “-“;
cout << endl;
for (int i = 0;i < height; i++)
{
for (int j = 0; j < width; j++)
{
if (j == 0)
cout << “|”;
if (i == y && j == x)
cout << “O”;
else if (i == ball_y && j == ball_x)
cout << “V”;
else
{
bool print = false;

for (int k = 0; k < nTail; k++)
{
if (tailX[k] == j && tailY[k] == i)
{
cout << “o”;
print = true;
}
}

if (!print)
cout << ” “;
}

if (j == width – 1)
cout << “|”;
}
cout << endl;

}

for (int i = 0; i < width+10; i++)
cout << “-“;
cout << endl;
cout << “Score:” << score << endl;
cout << “Controls:nW=UpnA=LeftnD=RightnS=Down”;
}
void Input()
{
if (_kbhit())
{
switch (_getch())
{
case ‘a’:
dir1 = LEFT;
break;
case ‘d’:
dir1 = RIGHT;
break;
case ‘w’:
dir1 = UP;
break;
case ‘s’:
dir1 = DOWN;
break;
case ‘x’:
finish = true;
break;
}

}

}

void Logic()
{
int prevX = tailX[0];
int prevY = tailY[0];
int prev2X, prev2Y;
tailX[0] = x;
tailY[0] = y;
for (int i = 1; i < nTail; i++)
{
prev2X = tailX[i];
prev2Y = tailY[i];
tailX[i] = prevX;
tailY[i] = prevY;
prevX = prev2X;
prevY = prev2Y;
}

switch (dir1)
{
case LEFT:
x–;
break;
case RIGHT:
x++;
break;
case UP:
y–;
break;
case DOWN:
y++;
break;
default:
break;

}
if (x >= width) x = 0; else if (x < 0) x = width – 1;
if (y >= height) y = 0; else if (y < 0) y = height – 1;
for (int i = 0; i < nTail; i++)
if (tailX[i] == x && tailY[i] == y)
finish = true;
if (x == ball_x && y == ball_y)
{
score += 10;
ball_x = rand() % width;
ball_y = rand() % height;
nTail++;
}

}

int main()
{
start();
while (!finish)
{
Make_area();
Input();
Logic();
Sleep(60);
}
return 0;
}
