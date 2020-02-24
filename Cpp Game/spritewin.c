#include <windows.h>
#include "resource.h"
#include <stdlib.h>
//OKneed to make skeleton win32 app, add menu File...New Game.Quit..make
//background, workcanvas..formcanvas device contexts
//load background bitmap onto background background canvas, to workcanvas then 
//blit to window now load and add sprite to workcanvas...
//add the timer function.. use it to activate the screen refreshes and adjust 
//position of sprites.
const char g_szClassName[] = "myWindowClass";
//function prototypes
BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam,LPARAM lParam);
void drawscreen(HWND hwnd, HDC hdc,HDC bufferhdc);//draws all the characters 
void initializegame(HDC hdc);//sets all of the variable when you start a new game
void loadart(HWND hwnd);//loads all of the artwork into memory to use later
void setspritepose(void);//determines the appropriate pose for running, firing...
void enemybrain(HDC hdc);//this subroutine will give us a place to put our
//brain for the enemy.
void collision(HDC hdc); //checks for collisions among objects
void firecollision(HDC hdc); //checks for collisions between the bullets and the enemy
void drawsplash(HWND hwnd, HDC hdc, HDC bufferhdc);
void healthcollision(HDC hdc);
void drawscore(HWND hwnd, HDC hdc, HDC bufferhdc);
//void noise(char *noise);
//the Window Procedure allows you to respond to window events
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
int i;
    switch(msg)
    {
        case WM_CLOSE://when the window closes
            DestroyWindow(hwnd);
        break;
        case WM_CREATE://when you create the window..this is a big one.
	
        loadart(hwnd);  //calls the subroutine that puts all of the pictures we 
        //use in the game into memory
       // this sets the timer or event loop run or "loop" every 50 milliseconds.
        ret = SetTimer(hwnd, ID_TIMER, 50, NULL);//sets event loop "speed"
        if(ret == 0)
        MessageBox(hwnd, "Could not SetTimer()!", "Error", MB_OK | MB_ICONEXCLAMATION);
	    GetClientRect(hwnd, &clientrect);//gets the size of the window
	    hdc = GetDC(hwnd); //get the window device context
        bufferhdc = CreateCompatibleDC(hdc);  //make a buffer that matches the 
        //window's device context
        bufferbmp = CreateCompatibleBitmap(hdc, clientrect.right, clientrect.bottom); 
        //make sure the buffer context has a compatible bitmap to paint on , 
        //and sizes it to match the size of the window
        SelectObject(bufferhdc, bufferbmp);//put that new bitmap on the buffer 
        gameon = 0;

	    //set the font
   	    hdc = GetDC(NULL);  //gets the handle to the window device context
    	lfHeight = -MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72); 
        //sets font size to 24
    	ReleaseDC(NULL, hdc);//releases the hdc handle, don't need it anymore
    	hf = CreateFont(lfHeight, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, "Comic Sans MS");
        //OK make the font!
   	    //this just sets it to comic, at a 24 point size
  
	break;
	case WM_SIZE:
       		GetClientRect(hwnd, &clientrect);//if we resize we have to get the 
       		//size again and place it over the buffer context
       		hdc = GetDC(hwnd);  // gets the device context of the resized window
        	bufferbmp = CreateCompatibleBitmap(hdc, clientrect.right, clientrect.bottom);
        	//make a bitmap to match the window device context
        	SelectObject(bufferhdc, bufferbmp); //assign the bitmap to the buffer
            break;
  	case WM_TIMER:
		//call each of our subroutines on every click of the timer,
		//creating real time animation!!
                if(gameon == ID_GO)
		        {  
                    enemybrain(hdc);
                    setspritepose();
                    firecollision(hdc); 
                    collision(hdc);   
                    healthcollision(hdc);
                    drawscreen(hwnd,hdc,bufferhdc);
                }
                else 
                {
                if (endscreen == 1)
                    drawscore(hwnd,hdc,bufferhdc);
                else
                    drawsplash(hwnd,hdc,bufferhdc);
                }
            break;
  	case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case ID_FILE_EXIT:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
                break;
                case ID_NEWGAME_GO:
            		// in the future we could initialize everything again here,
			        //to start the game over.
			        
			        gameon = DialogBox(GetModuleHandle(NULL), 
                	MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);
            		if(gameon == ID_GO)	
				{
                        endscreen = 1;
                		initializegame(hdc);
            			}
            		else if(gameon == ID_CANCEL)
				{
                		MessageBox(hwnd, "OK, We will not start a new game.", "Notice",
                    		MB_OK | MB_ICONINFORMATION);
            			}
            		else if(gameon == -1)
				{
                		MessageBox(hwnd, "Dialog failed!", "Error",
                   		 MB_OK | MB_ICONINFORMATION);
            			}
                break;
            }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
	        DeleteObject(bufferbmp);
        break;
	    case WM_CHAR:
	    {
	    //here we will process key commands to make our character move;
	    TCHAR tchar = (TCHAR)wParam; //Get the key that was pressed
	    if (tchar == ' ')
	    {
	        if (spriterec.mode != 1)//if he is not already firing
	        {
            for (i = 0; i < bulletcount; i++)
            { 
                if (firerec[i].active == 0)//look for an inactive bullet
                    {
                        bulletcount --;
                        spriterec.oldmode = spriterec.mode;//remember the old mode
                        if (spriterec.direction == 0)
                        {
                            spriterec.pose = 4;//firing
                            spriterec.mode = 1;//firing
                            firerec[i].xmomentum = 10;
                            firerec[i].active = 1; //activate it
                            firerec[i].left = spriterec.left + spriterec.width;
                            firerec[i].top = spriterec.top; //set the coordinates 
                            firerec[i].direction = 0; 
                        }
                        else
                        {
                            spriterec.pose = 13;//firing
                            spriterec.mode = 1;//firing
                            firerec[i].xmomentum = -10;
                            firerec[i].active = 1; //activate it
                            firerec[i].left = spriterec.left - 50;
                            firerec[i].top = spriterec.top; //set the coordinates
                             firerec[i].direction = 1; 
                        }
                        break;
                    }
            }
            }
            }
        
        }
    break;
        case WM_KEYDOWN:
        {
            TCHAR tchar = (TCHAR)wParam; //Get the key that was pressed
            if (wParam == VK_RIGHT)
            {
            //healthrec.xmomentum = -(spriterec.xmomentum);
            if (spriterec.xmomentum < 50) 
                spriterec.xmomentum += 5;
                if (spriterec.direction != 0)
                {
                    spriterec.direction = 0;
                    spriterec.pose = 0;
                }
            }
            if (wParam == VK_LEFT)
            {
            if (spriterec.xmomentum > -50)
                spriterec.xmomentum -= 5;
                if (spriterec.xmomentum < 0)
                if (spriterec.direction != 9)
                {
                    spriterec.direction = 9;
                    spriterec.pose = 13;
                }
            }
            if (wParam == VK_UP)
            if ((spriterec.mode != 1)&&(spriterec.mode != 2))//if he is not already jumping
            {
                spriterec.mode = 2; //make him jump
                spriterec.ymomentum = 15; //and give him an upward momentum
            }
            if (wParam == VK_DOWN)
            if (spriterec.mode = 2)
            if (spriterec.direction == 0)
            {
                spriterec.ymomentum = -30;
                spriterec.pose = 8;
            }else
            {
                spriterec.ymomentum = -30;
                spriterec.pose = 16;
            }   
        }
    break; 
default:
return DefWindowProc(hwnd, msg, wParam, lParam);
}
return 0;
}

BOOL CALLBACK AboutDlgProc(HWND hwnd, UINT Message, WPARAM wParam,LPARAM lParam)
{
    switch(Message)
    {
        case WM_INITDIALOG:

        return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case ID_GO:
                    EndDialog(hwnd, ID_GO);
                break;
                case ID_CANCEL:
                    EndDialog(hwnd, ID_CANCEL);
                break;
            }
        break;
        default:
            return FALSE;
    }
    return TRUE;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MYMENU);
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

hwnd = GetDesktopWindow(); //returns the window handle to your entire desktop screen
GetClientRect(hwnd,&clientrect); //returns a rectangle that is the same size, so we can 
//figure out how big to make our windo
    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "MegaSprite!",
        NULL, //I replaced WS_OVERLAPPED with NULL here to prevent resizing 
        0, 0, 632, 496,// I replaced the ClientRect values with the size of
        //my bitmaps here
        NULL, NULL, hInstance, NULL);
    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
        MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
   // ShowWindow(hwnd, nCmdShow);
    ShowWindow(hwnd, SW_NORMAL);

    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
void initializegame(HDC hdc) // We will put stuff here to start the game. 
{
int i;
 	spriterec.pose = 0;//sets the initial pose of the sprite
    GetObject(spritebmp[spriterec.pose], sizeof(bm), &bm);//find sprite size
    //positions him in the center-bottom of the screen initially
  	spriterec.top = (clientrect.bottom) - bm.bmHeight; 
  	spriterec.left = (clientrect.right /2) - bm.bmWidth /2;
    spriterec.width = bm.bmWidth;//gets the height and width of the sprite to use
    spriterec.height = bm.bmHeight;//later in the collisions
	spriterec.xmomentum = 0;//he is standing still
	spriterec.ymomentum = 0;
    //spriterec.mode = 0;
    GetObject(backgroundbmp[0], sizeof(bm), &bm);//find the background size
    backgroundwidth = bm.bmWidth - 20;
    backgroundcount = 5;
    toggle = 1;
    for (i = 0; i < backgroundcount; i ++)//set up a loop that fills all of bgpos[]
    {
        bgpos[i] =(i * backgroundwidth);//sets the first pos at 0, and the 
        //second pos at 1 * the width of the bitmaps. As we add more bitmaps,
        //the third bitmap position would be set at 2 * the width and so on
    } 
GetObject (enemybmp[0], sizeof(bm), &bm);//find the enemy size
    for (i = 0; i < 5; i++)
    {
        enemyrec[i].width = bm.bmWidth;
        enemyrec[i].height = bm.bmHeight;
        enemyrec[i].left = i * 700;
        enemyrec[i].top = clientrect.bottom - enemyrec[i].height;
        enemyrec[i].active = 1; //make sure he is active
    }
score = 0;  //you have no score
spriterec.life = 5;  //and 5 lives
GetObject(blastRbmp, sizeof(bm), &bm);//find the blast size
for (i = 0; i < 75 ; i++)
{
firerec[i].height = bm.bmHeight;//stores the height and width of the blast
firerec[i].width = bm.bmWidth;
firerec[i].explodingtimer = 0;//allows us to explode only for a while
firerec[i].active = 0;//the blast is not active until we fire it
firerec[i].exploding = 0;//and it is not exploding until it hits something
}
GetObject (healthbmp, sizeof(bm), &bm);
        healthrec.width = bm.bmWidth;
        healthrec.height = bm.bmHeight;
        healthrec.top = 300; 
        healthrec.left = 500;
bulletcount = 10;
if (bulletcount == 0)
{
    endscreen = 1;
    //end text
}
}
void drawscreen(HWND hwnd,HDC hdc,HDC bufferhdc)
{
int i;
//to do double buffering make a buffercanvas..do everything to that and as a final 
//step draw to the hdc  
hdcMem = CreateCompatibleDC(hdc);  //makes a memory canvas
for (i = 0;i < backgroundcount;i++)
{
SelectObject(hdcMem, backgroundbmp[i]); //puts the background bitmap on the memory canvas
GetObject(bufferbmp, sizeof(bm), &bm);//gets the info (size etc) of the bitmap structure bm
BitBlt(bufferhdc, bgpos[i], 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
//copies from memory to window canvas
if (spriterec.xmomentum > 0)
{
if (bgpos[i] < -backgroundwidth) bgpos[i] += backgroundwidth * (backgroundcount);
bgpos[i] -= spriterec.xmomentum;
}else
if (bgpos[i] > backgroundwidth) bgpos[i] -= backgroundwidth * (backgroundcount);
bgpos[i] -= spriterec.xmomentum;
//if the background goes all the way out of the window to the left, put it
//all the way at the end of the other background(s) so that it can scroll by
//again.
}
for (i = 0; i < 5; i++)
{
    enemyrec[i].left -= spriterec.xmomentum;
}

SelectObject(hdcMem, healthmaskbmp); //puts the sprite bitmap onto 
//the memory canvas
GetObject(healthmaskbmp, sizeof(bm), &bm);//gets the info of the 
//sprite bitmap
BitBlt(bufferhdc, healthrec.left, healthrec.top+ healthrec.vary,
bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCAND); //puts the memory on the 

SelectObject(hdcMem, healthbmp);  //puts the sprite bitmap onto 
//the memory canvas
GetObject(healthbmp, sizeof(bm), &bm);//gets the info of the 
//spritemask bitmap
BitBlt(bufferhdc, healthrec.left, healthrec.top + healthrec.vary,
bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT); //puts the memory on the 

healthrec.left += -spriterec.xmomentum;
//healthrec.top = healthrec.top + healthrec.vary;

// if toggle = 1 we will add 1 to the variation until it gets to 10
//then we will reverse the toggle
// if the toggle is reversed, we will subtract the variation until it gets to 0, then we will reverse the toggle
   if (toggle == 1)
          {
              healthrec.vary ++;
              if (healthrec.vary > 20) toggle *= -1;     
            }
            else
            {
              healthrec.vary --;
              if (healthrec.vary < 1) toggle *= -1;      
            }
            
//here we draw the sprite to the screen
SelectObject(hdcMem, spritemaskbmp[spriterec.pose]);  //puts the sprite bitmap onto 
//the memory canvas
GetObject(spritemaskbmp[spriterec.pose], sizeof(bm), &bm);//gets the info of the 
//spritemask bitmap
BitBlt(bufferhdc, spriterec.left,spriterec.top,
bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCAND); //puts the memory on the 

//and the sprite mask
SelectObject(hdcMem, spritebmp[spriterec.pose]);  //puts the sprite bitmap onto 
//the memory canvas
GetObject(spritebmp[spriterec.pose], sizeof(bm), &bm);//gets the info of the 
//sprite bitmap
BitBlt(bufferhdc, spriterec.left,spriterec.top,
bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT); //puts the memory on the 
//buffer canvas

for (i = 0; i < bulletcount; i++)
{
    if (firerec[i].active == 1)
    {
        if (firerec[i].direction == 0)
        {
        //here we draw the sprite to the screen
        SelectObject(hdcMem, blastmaskRbmp); //puts the blast mask onto the memory canvas
        GetObject(blastmaskRbmp, sizeof(bm), &bm);//gets the info of the blastmask bitmap
        BitBlt(bufferhdc, firerec[i].left, firerec[i].top,
        bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCAND); //puts the memory on the buffer canvas
        //and the sprite mask
        SelectObject(hdcMem, blastRbmp); //puts the sprite bitmap onto the memory canvas
        GetObject(blastRbmp, sizeof(bm), &bm);//gets the info of the blast bitmap
        BitBlt(bufferhdc, firerec[i].left, firerec[i].top,
        bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT); //puts the memory on the buffer canvas
        firerec[i].left += firerec[i].xmomentum;//this moves the bullet
        }else 
        {
        //here we draw the sprite to the screen
        SelectObject(hdcMem, blastmaskLbmp); //puts the blast mask onto the memory canvas
        GetObject(blastmaskLbmp, sizeof(bm), &bm);//gets the info of the blastmask bitmap
        BitBlt(bufferhdc, firerec[i].left, firerec[i].top,
        bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCAND); //puts the memory on the buffer canvas
        //and the sprite mask
        SelectObject(hdcMem, blastLbmp); //puts the sprite bitmap onto the memory canvas
        GetObject(blastLbmp, sizeof(bm), &bm);//gets the info of the blast bitmap
        BitBlt(bufferhdc, firerec[i].left, firerec[i].top,
        bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT); //puts the memory on the buffer canvas
        firerec[i].left += firerec[i].xmomentum;//this moves the bullet
        }
    }
}
for (i = 0; i < 5; i++)
{
    if (enemyrec[i].active == 1)//only draw if active!
    {
    SelectObject(hdcMem, enemymaskbmp[enemyrec[i].pose]);//puts the spritemask
    //onto the memory canvas
    GetObject(enemymaskbmp[enemyrec[i].pose], sizeof(bm), &bm);//gets the info of the
    //spritemask bitmap
    BitBlt(bufferhdc, enemyrec[i].left, enemyrec[i].top,
    bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCAND); //puts the memory on the buffer
    //canvas
    SelectObject(hdcMem, enemybmp[enemyrec[i].pose]); //puts the sprite bitmap
    //onto the memory canvas
    GetObject(enemybmp[enemyrec[i].pose], sizeof(bm), &bm);//gets the info of the
    //sprite bitmap
    BitBlt(bufferhdc, enemyrec[i].left, enemyrec[i].top,
    bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCPAINT); //puts the memory on the
    enemyrec[i].left += enemyrec[i].xmomentum; //this moves the enemy
    enemyrec[i].top += enemyrec[i].ymomentum; //this moves the enemy
    //buffer canvas
    }
}
SetBkMode(bufferhdc, TRANSPARENT);//make the text mode transparent
SetTextColor(bufferhdc,RGB(0, 255, 0));//make the text color green
SelectObject(bufferhdc, hf);//select the hf variable to the bufferhdc
itoa(score,message,10); //convert the value of score to a text message
TextOutA(bufferhdc,10,20,"Score->",7);//print out the label score->
TextOutA(bufferhdc,150,20,message,strlen(message));//then print the actual
//value of score after the label  Example: Score-> 50
if (spriterec.life < 3)
{
SetTextColor(bufferhdc,RGB(255, 0, 0));//make the text color green
}
else if (spriterec.life < 4)
{
SetTextColor(bufferhdc,RGB(255, 153, 51));//make the text color green
}
else 
SetTextColor(bufferhdc,RGB(0, 255, 0));//make the text color green
itoa(spriterec.life, message, 10);//convert the value of your character's life
TextOutA(bufferhdc,10,80,"Lives->",7);//print out the label Lives->
TextOutA(bufferhdc,150,80,message,strlen(message));//print out the actual value
//of life after the label  Example: Lives-> 5
if (bulletcount < 3)
{
SetTextColor(bufferhdc,RGB(255, 0, 0));//make the text color green
}
else if (bulletcount < 4)
{
SetTextColor(bufferhdc,RGB(255, 153, 51));//make the text color green
}
else
SetTextColor(bufferhdc,RGB(0, 255, 0));//make the text color green
itoa(bulletcount, message, 10);//convert the value of your character's life
TextOutA(bufferhdc,10,140,"Bullets->",9);//print out the label Lives->
TextOutA(bufferhdc,150,140,message,strlen(message));//print out the actual value
//this is the bullet count
GetObject(bufferbmp, sizeof(bm), &bm);//gets the info (size etc) of the bitmap 
//structure bm
//and we draw the completed scene (sprites and backgrounds) to the window device 
//context all at once.
BitBlt(hdc,0,0, bm.bmWidth, bm.bmHeight, bufferhdc, 0, 0, SRCCOPY);  
SelectObject(hdcMem, hbmOld);
DeleteDC(hdcMem);
}
void loadart(HWND hwnd)
{
//here we fill the proper elements of the sprite array with the proper
// poses. You can substitute your
// art for mine, but do not change the names. change the names in the 
//.rc files
spritebmp[0] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteR1));
if(spritebmp[0] == NULL)
MessageBox(hwnd, "Could not load xspriteR1!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[0] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteR1mask));
if(spritemaskbmp[0] == NULL)
MessageBox(hwnd, "Could not load xspriteR1mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[1] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteR2));
if(spritebmp[1] == NULL)
MessageBox(hwnd, "Could not load xspriteR2!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[1] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteR2mask));
if(spritemaskbmp[1] == NULL)
MessageBox(hwnd, "Could not load xspriteR2mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[2] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteR3));
if(spritebmp[2] == NULL)
MessageBox(hwnd, "Could not load xspriteR3!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[2] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteR3mask));
if(spritemaskbmp[2] == NULL)
MessageBox(hwnd, "Could not load xspriteR3mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[3] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteR4));
if(spritebmp[3] == NULL)
MessageBox(hwnd, "Could not load xspriteR4!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[3] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteR4mask));
if(spritemaskbmp[3] == NULL)
MessageBox(hwnd, "Could not load xspriteR4mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[4] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteRF1));
if(spritebmp[4] == NULL)
MessageBox(hwnd, "Could not load xspriteRF1!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[4] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteRF1mask));
if(spritemaskbmp[4] == NULL)
MessageBox(hwnd, "Could not load xspriteRF1mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[5] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteRF1));
if(spritebmp[5] == NULL)
MessageBox(hwnd, "Could not load xspriteRF2!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[5] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteRF2mask));
if(spritemaskbmp[5] == NULL)
MessageBox(hwnd, "Could not load xspriteRF2mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[6] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteRF3));
if(spritebmp[6] == NULL)
MessageBox(hwnd, "Could not load xspriteRF3!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[6] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteRF3mask));
if(spritemaskbmp[6] == NULL)
MessageBox(hwnd, "Could not load xspriteRF3mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[7] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteRF4));
if(spritebmp[7] == NULL)
MessageBox(hwnd, "Could not load xspriteRF4!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[7] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteRF4mask));
if(spritemaskbmp[7] == NULL)
MessageBox(hwnd, "Could not load xspriteRF4mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[8] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteRF5));
if(spritebmp[8] == NULL)
MessageBox(hwnd, "Could not load xspriteRF5!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[8] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteRF5mask));
if(spritemaskbmp[8] == NULL)
MessageBox(hwnd, "Could not load xspriteRF5mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[9] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteL1));
if(spritebmp[9] == NULL)
MessageBox(hwnd, "Could not load xspriteL1!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[9] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteL1mask));
if(spritemaskbmp[9] == NULL)
MessageBox(hwnd, "Could not load xspriteL1mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[10] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteL2));
if(spritebmp[10] == NULL)
MessageBox(hwnd, "Could not load xspriteL2!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[10] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteL2mask));
if(spritemaskbmp[10] == NULL)
MessageBox(hwnd, "Could not load xspriteL2mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[11] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteL3));
if(spritebmp[11] == NULL)
MessageBox(hwnd, "Could not load xspriteL3!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[11] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteL3mask));
if(spritemaskbmp[11] == NULL)
MessageBox(hwnd, "Could not load xspriteL3mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[12] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteL4));
if(spritebmp[12] == NULL)
MessageBox(hwnd, "Could not load xspriteL4!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[12] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteL4mask));
if(spritemaskbmp[12] == NULL)
MessageBox(hwnd, "Could not load xspriteL4mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[13] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteLF5));
if(spritebmp[13] == NULL)
MessageBox(hwnd, "Could not load xspriteL5!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[13] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteLF5mask));
if(spritemaskbmp[13] == NULL)
MessageBox(hwnd, "Could not load xspriteL5mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[13] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteLF1));
if(spritebmp[13] == NULL)
MessageBox(hwnd, "Could not load xspriteLF1!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[13] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteLF1mask));
if(spritemaskbmp[13] == NULL)
MessageBox(hwnd, "Could not load xspriteLF1mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[14] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteLF2));
if(spritebmp[14] == NULL)
MessageBox(hwnd, "Could not load xspriteLF2!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[14] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteLF2mask));
if(spritemaskbmp[14] == NULL)
MessageBox(hwnd, "Could not load xspriteLF2mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[15] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteLF3));
if(spritebmp[15] == NULL)
MessageBox(hwnd, "Could not load xspriteLF3!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[15] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteLF3mask));
if(spritemaskbmp[15] == NULL)
MessageBox(hwnd, "Could not load xspriteLF3mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[16] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteLF4));
if(spritebmp[16] == NULL)
MessageBox(hwnd, "Could not load xspriteLF4!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[16] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteLF4mask));
if(spritemaskbmp[16] == NULL)
MessageBox(hwnd, "Could not load xspriteLF4mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

spritebmp[17] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteLF5));
if(spritebmp[17] == NULL)
MessageBox(hwnd, "Could not load xspriteLF5!", "Error", MB_OK | MB_ICONEXCLAMATION);
spritemaskbmp[17] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(xspriteLF5mask));
if(spritemaskbmp[17] == NULL)
MessageBox(hwnd, "Could not load xspriteLF5mask!", "Error", MB_OK | MB_ICONEXCLAMATION);

backgroundbmp[0] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(background1));
if(backgroundbmp[0] == NULL)
MessageBox(hwnd, "Could not load background1!", "Error", MB_OK | MB_ICONEXCLAMATION);

backgroundbmp[1] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(background2));
if(backgroundbmp[1] == NULL)
MessageBox(hwnd, "Could not load background2!", "Error", MB_OK | MB_ICONEXCLAMATION);

backgroundbmp[2] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(background3));
if(backgroundbmp[2] == NULL)
MessageBox(hwnd, "Could not load background3!", "Error", MB_OK | MB_ICONEXCLAMATION);

backgroundbmp[3] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(background4));
if(backgroundbmp[3] == NULL)
MessageBox(hwnd, "Could not load background4!", "Error", MB_OK | MB_ICONEXCLAMATION);

backgroundbmp[4] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(background5));
if(backgroundbmp[4] == NULL)
MessageBox(hwnd, "Could not load background5!", "Error", MB_OK | MB_ICONEXCLAMATION);

splashbmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(splash));
if (splashbmp == NULL)
MessageBox(hwnd, "Could not load splash", "Error", MB_OK | MB_ICONEXCLAMATION);

scorebmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(scorepage));
if (scorebmp == NULL)
MessageBox(hwnd, "Could not load scorepage", "Error", MB_OK | MB_ICONEXCLAMATION);

blastRbmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(blastspriteR));
if (blastRbmp == NULL)
MessageBox(hwnd, "Could not load blastRbmp!", "Error", MB_OK | MB_ICONEXCLAMATION);
blastmaskRbmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(blastspriteRmask));
if (blastmaskRbmp == NULL)
MessageBox(hwnd, "Could not load blastspriteRmask!", "Error", MB_OK | MB_ICONEXCLAMATION);

blastLbmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(blastspriteL));
if (blastLbmp == NULL)
MessageBox(hwnd, "Could not load blastLbmp!", "Error", MB_OK | MB_ICONEXCLAMATION);
blastmaskLbmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(blastspriteLmask));
if (blastmaskLbmp == NULL)
MessageBox(hwnd, "Could not load blastspriteLmask!", "Error", MB_OK | MB_ICONEXCLAMATION);

//Leo I retyped this here.....solved the problem IDK
healthbmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(healthsprite));
if (healthbmp == NULL)
MessageBox(hwnd, "Could not load healthsprite!", "Error", MB_OK | MB_ICONEXCLAMATION);
healthmaskbmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(healthspritemask));
if (healthmaskbmp == NULL)
MessageBox(hwnd, "Could not load healthspritemask!", "Error", MB_OK | MB_ICONEXCLAMATION);

wastebmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(wastesprite));
if (wastebmp == NULL)
MessageBox(hwnd, "Could not load wastesprite!", "Error", MB_OK | MB_ICONEXCLAMATION);
wastemaskbmp = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(wastespritemask));
if (wastemaskbmp == NULL)
MessageBox(hwnd, "Could not load wastespritemask!", "Error", MB_OK | MB_ICONEXCLAMATION);

//here I may load my adversary sprite. See how easy it is! Check the .h and the .rc
//files for the complete picture, including the record tntsprite[5] declaration
//so that I can have 5 enemies!

enemybmp[0] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(enemystandleftsprite));
if (enemybmp[0] == NULL)
MessageBox(hwnd, "Could not load enemystandleftsprite", "Error", MB_OK | MB_ICONEXCLAMATION);
enemymaskbmp[0] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(enemystandleftspritemask));
if (enemymaskbmp[0] == NULL)
MessageBox(hwnd, "Could not load enemystandleftspritemask", "Error", MB_OK | MB_ICONEXCLAMATION);

enemybmp[1] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(enemysquatleftsprite));
if (enemybmp[1] == NULL)
MessageBox(hwnd, "Could not load enemysquatleftsprite", "Error", MB_OK | MB_ICONEXCLAMATION);
enemymaskbmp[1] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(enemysquatleftspritemask));
if (enemymaskbmp[1] == NULL)
MessageBox(hwnd, "Could not load enemysquatleftspritemask", "Error", MB_OK | MB_ICONEXCLAMATION);

enemybmp[2] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(enemyjumpleftsprite));
if (enemybmp[2] == NULL)
MessageBox(hwnd, "Could not load enemyjumpleftsprite", "Error", MB_OK | MB_ICONEXCLAMATION);
enemymaskbmp[2] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(enemyjumpleftspritemask));
if (enemymaskbmp[2] == NULL)
MessageBox(hwnd, "Could not load enemyjumpleftspritemask", "Error", MB_OK | MB_ICONEXCLAMATION);

enemybmp[3] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(enemystandrightsprite));
if (enemybmp[3] == NULL)
MessageBox(hwnd, "Could not load enemystandrightsprite", "Error", MB_OK | MB_ICONEXCLAMATION);
enemymaskbmp[3] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(enemystandrightspritemask));
if (enemymaskbmp[3] == NULL)
MessageBox(hwnd, "Could not load enemystandrightspritemask", "Error", MB_OK | MB_ICONEXCLAMATION);

enemybmp[4] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(enemysquatrightsprite));
if (enemybmp[4] == NULL)
MessageBox(hwnd, "Could not load enemysquatrightsprite", "Error", MB_OK | MB_ICONEXCLAMATION);
enemymaskbmp[4] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(enemysquatrightspritemask));
if (enemymaskbmp[4] == NULL)
MessageBox(hwnd, "Could not load enemysquatrightspritemask", "Error", MB_OK | MB_ICONEXCLAMATION);

enemybmp[5] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(enemyjumprightsprite));
if (enemybmp[5] == NULL)
MessageBox(hwnd, "Could not load enemyjumprightsprite", "Error", MB_OK | MB_ICONEXCLAMATION);
enemymaskbmp[5] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(enemyjumprightspritemask));
if (enemymaskbmp[5] == NULL)
MessageBox(hwnd, "Could not load enemyjumprightspritemask", "Error", MB_OK | MB_ICONEXCLAMATION);
}

void setspritepose(void)
{
int spritecentery, spritecenterx;
int healthcentery, healthcenterx;
spritecentery = spriterec.top + (spriterec.height / 2); //add i/2 of the height
//to the top to get the current center of the sprite
spritecenterx = spriterec.left + (spriterec.width / 2); //add 1/2 of the width
//to the left to get the current center of the sprite
healthcentery = healthrec.top + (healthrec.height / 2);
healthcenterx = healthrec.left + (healthrec.width / 2);



    if(spriterec.mode == 2)//he is jumping
    {
        spriterec.top -= spriterec.ymomentum;//send him upward
        spriterec.ymomentum --;//let gravity pull him back down
        //if he lands on the ground
        if (spriterec.top + spriterec.height >= clientrect.bottom)
        {
            spriterec.top = clientrect.bottom - spriterec.height;//set his pos
            spriterec.mode = 0;//and put him back to walking mode
        }
    }
    if(spriterec.mode == 0)//walking
    {
        
          
        //if he lands on the ground
        if (spriterec.top + spriterec.height < clientrect.bottom)
        {
           spriterec.top -= spriterec.ymomentum;
            spriterec.ymomentum --;//let gravity pull him back down
          
        }
        
        if (spriterec.xmomentum != 0) //we are moving
        {
            spriterec.pose += 1;
            if (spriterec.pose > 3 +spriterec.direction)
                spriterec.pose = spriterec.direction;
        }else  
        spriterec.pose =spriterec.direction;
        }
          
        
        
    if (spriterec.mode == 1) //firing
    {
        if ( spriterec.top + spriterec.height < clientrect.bottom)
        {
            spriterec.top -= spriterec.ymomentum;
            spriterec.ymomentum --;
        }
        
        
        spriterec.pose += 1;
        if (spriterec.pose > 6 +spriterec.direction) //if we exceed the firing poses
        {
          
            spriterec.pose = spriterec.direction;//set our pose back to 0
            spriterec.mode = spriterec.oldmode;//restore the mode
        }
    }
    
    
    
    if ((abs(spritecenterx - healthcenterx) < 30)&&(abs(spritecentery - healthcentery) < 30))
    if (spriterec.direction == 0)
    {
        spriterec.pose = 7;
        spriterec.ymomentum --;
    }else
    {
        spriterec.pose = 17;
        spriterec.ymomentum --;
    }           
}
void enemybrain(HDC hdc)
{
int enemydistance;
for (int i = 0; i < 5; i++)//cycle through all five enemies
{
if (enemyrec[i].jumpphase == 0) //he is not already jumping
{
    if (abs(enemyrec[i].left - spriterec.left) < 1000) //if the character is
    //within 340 pixels of the enemy
    if (rand()% 20 == 1)//he will have a 1 in 20 chance of deciding to jump
    {
    if (enemyrec[i].left > (spriterec.left + spriterec.width)) //if the character
    //is to the left of the enemy
        enemyrec[i].startpose = 0; //set the enemy pose facing left
    else    enemyrec[i].startpose = 3; //set the enemy pose facing right
        enemyrec[i].jumpphase = 1;//ok he is starting to jump
        enemyrec[i].pose = enemyrec[i].startpose; //set the pose to what we
        //calculated above.
    }
}
else
    //if he is in the process of jumping do the following
{
int enemydistance = abs(enemyrec[i].left - spriterec.left);
//for first part of his jump he starts to squat
if ((enemyrec[i].jumpphase > 0) && (enemyrec[i].jumpphase < 8))
    {
    enemyrec[i].pose = enemyrec[i].startpose + 1;
    enemyrec[i].jumpphase ++; //start moving through the jump phase
    }
else if (enemyrec[i].jumpphase == 8) //when I get to the middle of the jump
//move to the flying part of the jump
    {
    enemyrec[i].pose = enemyrec[i].startpose + 2; //set to the flying pose
    enemyrec[i].ymomentum = -30; //set his momentum upward
        if (enemyrec[i].startpose == 3) //if I am jumping to the right
            enemyrec[i].xmomentum = (enemydistance / 16); //send him to the right
        else //otherwise I am jumping to the left
            enemyrec[i].xmomentum = ((-enemydistance) / 16);//send him left
            enemyrec[i].jumpphase ++; //cycle through the jump phase
        if (spriterec.ymomentum > 10) enemyrec[i].ymomentum = rand() % 10; //send him to the right
    }
else if (enemyrec[i].jumpphase > 8) //if I am in the air
    {
        enemyrec[i].ymomentum += 4;
        
    //now if I travel below the floor of the game
    if (enemyrec[i].top + enemyrec[i].height > clientrect.bottom)
        {
            //make sure I am standing on the floor move up if required
            enemyrec[i].top = clientrect.bottom - enemyrec[i].height;
            enemyrec[i].xmomentum = 0; //stop all of my momentum
            enemyrec[i].ymomentum = 0;
            enemyrec[i].jumpphase = 0; //set me to not jumping so I can "decide"
            //to jump again
        }
    }
}
if (abs((enemyrec[i].left - spriterec.left)) > + 2000)
{ 
    if (spriterec.xmomentum > 0)
    enemyrec[i].left += (rand() % 500) + 500 + spriterec.left; 
    else
    enemyrec[i].left -= (rand() % 1000)- 1000 - spriterec.left;       
}
if ((enemyrec[i].jumpphase > 8)&&(spriterec.mode == 2))
{
    if (abs(enemyrec[i].left - spriterec.left) < 100)
    {
        if (enemyrec[i].startpose == 3)
            enemyrec[i].xmomentum = 30;
        else
            enemyrec[i].xmomentum = -30;
    }   
}
}
}
void collision(HDC hdc) //This subroutine will give us a place to type in
//collision detection code
{
    int i, j;//this is a utility variable that will let us loop with i
    //this first step will detect if any of the enemies are colliding with us
    //first we will get the "center" of our character
    int spritecenterx, spritecentery; //define two integer variables to store the
    //center coordinates of the enemy
    int enemycenterx, enemycentery; //define two integer variables to store the 
    //center coordinates of the enemy
    spritecentery = spriterec.top + (spriterec.height / 2); //add i/2 of the height
    //to the top to get the current center of the sprite
    spritecenterx = spriterec.left + (spriterec.width / 2); //add 1/2 of the width
    //to the left to get the current center of the sprite
    //now we will perform a loop to get the center of each enemy and see if any of
    //them are hitting us
    for (i = 0; i < 5; i++)//start the loop with i
    {
        if (enemyrec[i].active == 1)//here is the new condition that only checks if 
        //the enemy is active
        {
            
            enemycenterx = enemyrec[i].left + (enemyrec[i].width / 2);//find the center of
            //the enemy on the X
            enemycentery = enemyrec[i].top + (enemyrec[i].height / 2);//find the center of 
            //the enemy on the Y
            //see if the absolute value of a subtraction operation between the two
            //centers is less than 40 on both axis. You can adjust the number for more
            //or less precision
            if ((abs(spritecenterx - enemycenterx) < 30) &&
            (abs(spritecentery - enemycentery) < 30))
            {
                //if the enemy hits us I want him to bounce off us
                enemyrec[i].ymomentum *= -1;
                //and take away a life
                spriterec.life --;
            }
            //I do not want to die unless all my lives are gone
            if (spriterec.life == 0) gameon = 0; endscreen = 1;//if we are hitting end the game
        }
    }
}
void firecollision(HDC hdc) //This subroutine will give us a place to type in
//collision detection code for the bullets
{
    int i, j;//The i and j variables allow us to perform two loops, on insidethe other,
    //or "nested loops" This is because we have 5 enemies and bullets, and we have
    //to compare the coordinates of all five enemies to all five bullets.
    int enemycenterx, enemycentery; //define two integer variables to store the 
    //center coordinates of the enemy
    int firecenterx, firecentery;//define two integer variables to store the
    //center coordinates of the bullets
    //now we will perform a loop with i to get the center of each enemy and see if
    //and then cycle through the bullets with j to see if any of them are hitting
    //each enemy
    for (i = 0; i < 5; i++)//start the enemy loop with i
    {
        enemycenterx = enemyrec[i].left + (enemyrec[i].width / 2);//find the center of 
        //the enemy on the X
        enemycentery = enemyrec[i].top + (enemyrec[i].height / 2);//find the center of 
        //the enemy on the Y
        //now do a loop with J to go through all 5 bullets
        for (j = 0; j < bulletcount; j++)
        {
            if ((enemyrec[i].active == 1)&&(firerec[j].active == 1))//don't check
            //if either the bullet or enemy is inactive
            {
                //get the centers of the bullet
                firecenterx = firerec[j].left + (firerec[j].width / 2);
                firecentery = firerec[j].top + (firerec[j].height / 2);
                //see if the enemy and the bullet are within 30 pixels of one
                //another using the abs function
                if ((abs(firecenterx - enemycenterx) < 30)&&
                (abs(firecentery - enemycentery) < 30))
                {
                    //if we hit
                    //enemyrec[i].active = 0; //inactivate the enemy
                    enemyrec[i].left =( rand() % 1000) + backgroundwidth;
                    firerec[j].active = 0; //inactivate the bullet
                    score+=1;
                    bulletcount++;
                }
                if (firerec[j].left > backgroundwidth) firerec[i].active = 0;
                //if our bullet goes offscreen deactivate it so we can re-use it.
            }
        }
    }
}
void drawsplash(HWND hwnd, HDC hdc, HDC bufferhdc)
{
//step draw to the hdc
hdcMem = CreateCompatibleDC(hdc); //makes a memory canvas
SelectObject(hdcMem, splashbmp); //puts the splash bitmap on the memory canvas
GetObject(splashbmp, sizeof(bm), &bm);//gets the info (size, etc.) of the bitmap
//structure so we have the
BitBlt(hdc,0,0,bm.bmWidth,bm.bmHeight,hdcMem,0,0,SRCCOPY);//paint straight to
//the hdc
endscreen = 0;
SelectObject(hdcMem, hbmOld);
DeleteDC(hdcMem);
}

void healthcollision(HDC hdc)
{
int spritecentery, spritecenterx;
int healthcentery, healthcenterx;
spritecentery = spriterec.top + (spriterec.height / 2); //add i/2 of the height
//to the top to get the current center of the sprite
spritecenterx = spriterec.left + (spriterec.width / 2); //add 1/2 of the width
//to the left to get the current center of the sprite
healthcentery = healthrec.top + (healthrec.height / 2);
healthcenterx = healthrec.left + (healthrec.width / 2);
if ((abs(spritecenterx - healthcenterx) < 30)&&(abs(spritecentery - healthcentery) < 30))
{                                       
    spriterec.life += 1;
    healthrec.left += (rand() % 1000) + 1000;
}
//else if (abs((healthrec.left - spriterec.left)) > 1000)
//{ 
    //if (spriterec.xmomentum > 0)
    //healthrec.left += (rand() % 1000)+ 1000 + spriterec.left; 
    //else
    //healthrec.left -= (rand() % 1000)- 1000 - spriterec.left;       
//}
}
void drawscore(HWND hwnd, HDC hdc, HDC bufferhdc)
{
//step draw to the hdc
hdcMem = CreateCompatibleDC(hdc); //makes a memory canvas
SelectObject(hdcMem, scorebmp); //puts the splash bitmap on the memory canvas
GetObject(scorebmp, sizeof(bm), &bm);//gets the info (size, etc.) of the bitmap

BitBlt(bufferhdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
//structure so we have the
SetBkMode(bufferhdc, TRANSPARENT);//make the text mode transparent
SetTextColor(bufferhdc,RGB(0, 255, 0));//make the text color green
SelectObject(bufferhdc, hf);//select the hf variable to the bufferhdc
itoa(score,message,10); //convert the value of score to a text message
TextOutA(bufferhdc,100,100,"Your Score is -> ",17);//print out the label score->
TextOutA(bufferhdc,360,100,message,strlen(message));//then print the actual

if(score > 100)
   TextOutA(bufferhdc,150,200,"That was great!",15);
else if(score > 74)
   TextOutA(bufferhdc,175,200,"A good score",12);
else if(score > 50)
    TextOutA(bufferhdc,175,200,"Pretty good",11);
else if(score > 30)
    TextOutA(bufferhdc,175,200,"That was Okay",13);
else if(score > 10)
     TextOutA(bufferhdc,200,200,"Nice try",8);
else
    TextOutA(bufferhdc,100,200,"Try harder next time",20);
BitBlt(hdc,0,0,bm.bmWidth,bm.bmHeight,bufferhdc,0,0,SRCCOPY);//paint straight to
//the hdc
SelectObject(hdcMem, hbmOld);
DeleteDC(hdcMem);
}
//void noise(char *noise)
//{
    //HANDLE ResHandle;
    //const char *WavData;
    //ResHandle = FindResource(NULL, noise, "WAVE");
    //ResHandle = LoadResource(NULL, (HRSRC)ResHandle);
    //WavData = (const char *)LockResource(ResHandle);
    //PlaySound(WavData, NULL, SND_MEMORY | SND_ASYNC);
    //FreeResource(ResHandle);
//}
   



    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
 
































