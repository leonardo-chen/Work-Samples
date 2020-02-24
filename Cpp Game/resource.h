#define IDR_MYMENU 101
#define IDD_ABOUT  102
#define ID_FILE_EXIT 9001
#define ID_NEWGAME_GO 9002
#define ID_GO 9003
#define ID_CANCEL 9004
#define IDC_STATIC 103

#define background1 9005
#define background2 9041
#define background3 9106
#define background4 9107
#define background5 9105

#define xspriteR1 9006
#define xspriteR1mask 9007
#define xspriteR2 9008
#define xspriteR2mask 9009
#define xspriteR3 9010
#define xspriteR3mask 9011
#define xspriteR4 9012
#define xspriteR4mask 9013
#define xspriteRF1 9014
#define xspriteRF1mask 9015
#define xspriteRF2 9016
#define xspriteRF2mask 9017
#define xspriteRF3 9018
#define xspriteRF3mask 9019
#define xspriteRF4 9077
#define xspriteRF4mask 9078
#define xspriteRF5 9079
#define xspriteRF5mask 9080

#define xspriteL1 9020
#define xspriteL1mask 9021
#define xspriteL2 9022
#define xspriteL2mask 9023
#define xspriteL3 9024
#define xspriteL3mask 9025
#define xspriteL4 9026
#define xspriteL4mask 9027
#define xspriteLF1 9028
#define xspriteLF1mask 9029
#define xspriteLF2 9030
#define xspriteLF2mask 9031
#define xspriteLF3 9032
#define xspriteLF3mask 9033
#define xspriteLF4 9100
#define xspriteLF4mask 9101
#define xspriteLF5 9104
#define xspriteLF5mask 9103

#define tntsprite 9034
#define tntspritemask 9035

#define powsprite 9036
#define  powspritemask 9037

#define spacemansprite 9038
#define  spacemanspritemask 9039

#define  platformsprite 9040

#define blastspriteR 9042
#define blastspriteRmask 9043

#define blastspriteL 9089
#define blastspriteLmask 9090

#define healthsprite 9057
#define healthspritemask 9058

#define enemystandleftsprite 9044
#define enemystandleftspritemask 9045
#define enemysquatleftsprite 9046
#define enemysquatleftspritemask 9047
#define enemyjumpleftsprite 9048
#define enemyjumpleftspritemask 9049
#define enemystandrightsprite 9050
#define enemystandrightspritemask 9051
#define enemysquatrightsprite 9052
#define enemysquatrightspritemask 9053
#define enemyjumprightsprite 9054
#define enemyjumprightspritemask 9055

#define splash 9056 //a new splash resource. Make sure this is a one time use number
#define scorepage 9099

#define wastesprite 9102
#define wastespritemask 9200

//global variables 
int gameon,ret,score,life,buildlife,bulletcount; //general game variables
char message[10];//utility variable to hold strings to display
HFONT hf,hfOld;//variables to hold font we want to use
long lfHeight; //this will be the font height
HBRUSH mybrush;//gives us the ability to create a paint brush  
//Bitmap variables we can fill with artwork
HBITMAP enemybmp[6];
HBITMAP enemymaskbmp[6];
HBITMAP blastRbmp = NULL; //these are the bitmaps to hold the weapon blast
HBITMAP blastmaskRbmp = NULL;
HBITMAP blastLbmp = NULL; //these are the bitmaps to hold the weapon blast
HBITMAP blastmaskLbmp = NULL;
HBITMAP spritebmp[18]; //the sprite has 14 poses, hence the[14]
HBITMAP spritemaskbmp[18];
HBITMAP healthbmp = NULL;
HBITMAP healthmaskbmp = NULL;
HBITMAP tntspritebmp = NULL;//this is the floating bomb, only one pose
HBITMAP tntspritemaskbmp = NULL;
HBITMAP powspritebmp = NULL;   //this is a cartoon explosion for when we touch the bomb
HBITMAP powspritemaskbmp = NULL;
HBITMAP spacemanspritebmp = NULL;  //these space men fly around, if I catch them I get more points
HBITMAP spacemanspritemaskbmp = NULL;
HBITMAP backgroundbmp[6];   //this holds the big picture of space
HBITMAP splashbmp = NULL; //a new splash bitmap
HBITMAP scorebmp = NULL;
HBITMAP wastebmp = NULL;
HBITMAP wastemaskbmp = NULL;
//this gives us variables to place each background in the correct position
int bgpos[5];
int backgroundwidth;
int backgroundcount;
//these structures give each character or object in our game a miniature
//database of information to pack around with them.
HBITMAP platformbmp = NULL; //the platoform that you can jump to
HBITMAP bufferbmp = NULL;  // allows me to do double buffering
HBITMAP oldbufferbmp = NULL;//a copy to preserve the bitmap
HBITMAP hbmOld = NULL;  //a copy to preserve 
RECT clientrect;//a rectangle variable to hold the screen size
BITMAP bm;  // a temportary bitmap variable to use when we are getting the size of the sprites
const int ID_TIMER = 1; //
int endscreen;
HDC hdc,hdcMem, bufferhdc;  //handles to  the device contexts that we use for double buffering
int toggle;
struct spriteinfo
{
int mode, oldmode; //0 walking, 1 firing, 2 jumping, oldmode to restore previous
int pose; //what image of the sprite we show.
int maxpose;//the end of the walk or fire sequence... 3 for walking right 11 for walking left
int left;     
int top;
int height;
int width;
int xmomentum,ymomentum;
int life;
int direction;
}spriterec;

struct enemyinfo
{
int left;
int top;
int height;
int width;
int jumpphase;//indicates where he is in the process of jumping.
int directionlength;
int xmomentum, ymomentum;
int pose, startpose;
int active;
}enemyrec[5];

struct platforminfo
{
int left;     
int top;
int height;
int width;
int xmomentum,ymomentum;
}platformrec[5];

struct tntspriteinfo
{
int left;
int top;
int height;
int width;
int explodingtimer;//allows us to explode only for a while
int xmomentum,ymomentum,active,exploding;
}tntspriterec[5];

struct fireinfo
{
int left;
int top;
int height;
int width;
int explodingtimer;//allows us to explode only for a while
int xmomentum, ymomentum, active, exploding,direction;
}firerec[10];

struct spacemaninfo
{
int left;
int top;
int height;
int width;
int oxygen;//he will run out of oxygen after a bit if we do not pick him up
int xmomentum,ymomentum;
}spacemanrec[5];

struct healthinfo
{
int left;
int top;
int height;
int width;
int xmomentum;
int vary;
}healthrec;

struct wasteinfo
{
int left;
int top;
int xmomentum;
}wasterec;













