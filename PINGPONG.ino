//***************************************************************************  
  
//***********************************************************************/  
#include <Arduino.h>  
#include <stdio.h>  
#include <math.h>  
  
  
#define DEBUG_ENABLED  1  
#define blueToothSerial Serial2  
  
// IO settings  
#define LEDARRAY_D 53  
#define LEDARRAY_C 52  
#define LEDARRAY_B 51  
#define LEDARRAY_A 50  
#define LEDARRAY_G 49  
#define LEDARRAY_DI 48  
#define LEDARRAY_CLK 47  
#define LEDARRAY_LAT 46  
#define interruptPinDownLeft 22  
#define interruptPinDownRight 24  
#define interruptPinUpLeft 26  
#define interruptPinUpRight 28  
#define interruptStart 30  
#define Num_Of_Word 13  // paddle has 13 possible locations   
#define Num_Word 1      // define two dimension array  
#define max_score 6     // when one player reach 5, he wins  
  
#define gamepad1X A1    //3D accelerometer's input parameter   
#define gamepad1Z A5                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
#define gamepad2X A3  
#define gamepad2Z A4  
  
int val_x1;int val_z1;// 3D accelerometer's local parameter   
int val_x2;int val_z2;  
  
int leftcount1=0;// These are parameters for 3D accelerometer  
int rightcount1=0;  
int leftcount2=0;  
int rightcount2=0;  
double b1;  
double b2;  
  
unsigned char Display_Swap_Buffer[Num_Word][32]={0};    // This is what will show on screen        
bool Shift_Bit = 0;                                     // These parameter to implement pattern's rolling on the screen  
bool Flag_Shift = 0;  
unsigned char Timer0_Count = 0;  
unsigned char temp = 0x80;  
unsigned char Shift_Count = 0;  
unsigned char Display_Word_Count = 0;    
  
unsigned char Display_Up_Buffer[Num_Word][32]={0};    //we differentiate the whole screen into two parts, half top and half bottom  
unsigned char Display_Down_Buffer[Num_Word][32]={0};  
unsigned char Display_left_Buffer[max_score][32]={0};//we differentiate the whole screen into two parts, half left and half right  
unsigned char Display_right_Buffer[max_score][32]={0};  
unsigned char Display_Buffer[2];                    // and combine them  
unsigned char UpPaddleLocation = 7; // when reset, paddle shows in the middle of edge  
unsigned char DownPaddleLocation = 6;// when reset, paddle shows in the middle of edge  
unsigned char Row = 7;               //when reset, ball shows in the midlle of screen  
unsigned char Column = 7;           //when reset, ball shows in the midlle of screen  
unsigned int cnt;                   //for delay  
unsigned int count = 0;  
  
unsigned char START_STATE;  
unsigned char p1_score = 0;       //when reset, player1's score is 0  
unsigned char p2_score = 0;       //when reset, player1's score is 0  
unsigned char BALL_SPEED = 5;       //when reset, ball's speed is 5  
unsigned char rand_state;  
  
unsigned char show_start = 1;  
unsigned char show_score = 1;  
  
  
  
unsigned char left_score[max_score][32] =   {   //pattern of numbers on left-half screen  
  {0xff,0xff,0xff,0xc1,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xc1,0xff,0xff,0xff,0xff,0xff, //0  
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},  
  {0xff,0xff,0xff,0xf7,0xf7,0xf7,0xf7,0xf7,0xf7,0xf7,0xf7,0xff,0xff,0xff,0xff,0xff, //1  
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},  
  {0xff,0xff,0xff,0xc7,0xbb,0xbb,0xf7,0xef,0xdf,0xbf,0x83,0xff,0xff,0xff,0xff,0xff, //2  
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},  
  {0xff,0xff,0xff,0xc7,0xbb,0xfb,0xc7,0xfb,0xbb,0xbb,0xc7,0xff,0xff,0xff,0xff,0xff, //3  
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},  
  {0xff,0xff,0xff,0xbb,0xbb,0xbb,0xc3,0xfb,0xfb,0xfb,0xfb,0xff,0xff,0xff,0xff,0xff, //4  
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff},  
  {0xff,0xff,0xff,0x81,0xbf,0xbf,0xc3,0xfd,0xfd,0xfd,0x83,0xff,0xff,0xff,0xff,0xff, //5  
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}   
  };  
  
  
unsigned char right_score[max_score][32] = {    //pattern of numbers on right -half screen  
  {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, //0  
  0xff,0xff,0xff,0xc1,0xdd,0xdd,0xdd,0xdd,0xdd,0xdd,0xc1,0xff,0xff,0xff,0xff,0xff},  
  {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, //1  
  0xff,0xff,0xff,0xf7,0xf7,0xf7,0xf7,0xf7,0xf7,0xf7,0xf7,0xff,0xff,0xff,0xff,0xff},  
  {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, //2  
  0xff,0xff,0xff,0xc7,0xbb,0xbb,0xf7,0xef,0xdf,0xbf,0x83,0xff,0xff,0xff,0xff,0xff},  
  {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, //3  
  0xff,0xff,0xff,0xc7,0xbb,0xfb,0xc7,0xfb,0xbb,0xbb,0xc7,0xff,0xff,0xff,0xff,0xff},  
  {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, //4  
  0xff,0xff,0xff,0xbb,0xbb,0xbb,0xc3,0xfb,0xfb,0xfb,0xfb,0xff,0xff,0xff,0xff,0xff},  
  {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, //5  
  0xff,0xff,0xff,0x81,0xbf,0xbf,0xc3,0xfd,0xfd,0xfd,0x83,0xff,0xff,0xff,0xff,0xff}   
  };  
    
unsigned char clear_led[Num_Word][32] =   //extinguish all screen  
  {   
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  
  };  
  
unsigned char Press_Start[1][32] =        // This pattern shows "Press Start" in screen  
  {   
  0xff,0x1f,0x5f,0x52,0x16,0x76,0x76,0x76,0xff,0x1b,0x7a,0x11,0xda,0xda,0x1a,0xff,  
  0xff,0xff,0xff,0x24,0xad,0x24,0xf6,0x24,0xff,0xfd,0x25,0xa8,0x2d,0xad,0x2d,0xff,  
  };  
  
unsigned char P1_WIN[1][32] =           // This pattern shows "Pl WIN" in screen  
  {   
  0xff,0xff,0xf1,0xf5,0xf1,0xf7,0xf7,0xff,0xff,0xdd,0xd5,0xd5,0xe3,0xeb,0xff,0xff,  
  0xff,0xff,0xbf,0xbf,0xbf,0xbf,0xbf,0xff,0xff,0x5b,0x4b,0x43,0x53,0x5b,0xff,0xff,  
  };  
  
unsigned char P2_WIN[1][32] =           // This pattern shows "P2 WIN" in screen  
  {   
  0xff,0xff,0xf1,0xf5,0xf1,0xf7,0xf7,0xff,0xff,0xdd,0xd5,0xd5,0xe3,0xeb,0xff,0xff,  
  0xff,0xff,0x1f,0xdf,0x9f,0x3f,0x1f,0xff,0xff,0x5b,0x4b,0x43,0x53,0x5b,0xff,0xff,  
  };  
  
typedef enum      //finite state machine seperate the whole game into four stages  
{  
  WAIT,  
  START,  
  SCORE,  
  END,  
}BALL_State_t;  
  
BALL_State_t BallState = WAIT;  
  
  
typedef enum    //eight types of ball's states  
{  
  S_D,          //moving down straight  
  S_U,          //moving up straight  
  L_D,          //moving down left in a 45 degree angle  
  R_D,          //moving down right in a 45 degree angle  
  L_U,          //moving up left in a 45 degree angle  
  R_U,          //moving up right in a 45 degree angle  
  Rst,          //goes back to the middle of screen  
  Change,       //ball's movement will change   
} Mov;  
Mov BallMov = S_D;      //current ball movement  
Mov LastBallMov = Rst;  //ball movement before last changing  
Mov LastBallMov_2;      //ball movement before last changing  
  
  
const unsigned char UpPaddle[Num_Of_Word][32] =// each paddle has 13 possible locations in this game, each location corresponds to one pattern   
{  
  {0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0x87,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xC3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xE1,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xF0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xF8,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xFC,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0x87,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xC3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xE1,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xF0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}//13 pattern of paddle  
  
};  
  
const unsigned char DownPaddle[Num_Of_Word][32] =// each paddle has 13 possible locations in this game, each location corresponds to one pattern   
{  
  {255,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x87,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC3,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE1,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF8,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3F},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x1F},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x0F},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x87},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC3},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE1},  
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0}//13 pattern of paddle  
};  
  
unsigned char Ball[16][16] = // ball shows in the middle of screen '0' represent the ball  
{  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},  
};  
  
unsigned char Word1[Num_Word][32]; // this is the final matrix to show ball  
  
unsigned char zero[Num_Word][32] =  
  {   
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,  
  };  
  
unsigned char one[Num_Word][32] =  
  {   
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,  
  };  
  
void SceneToWord(){// transform ball's[16][16] matrix into the same matrix as [1][32]paddle, so it can be shown  
  char i, j,k, key = 0;  
  unsigned char value;  
  for (j=0;j<16;j++){  
  Word1[0][j] = zero[0][j];  
  Word1[0][j+16] = zero[0][j+16];  
  }  
  for(i = 0; i < 16; i++){  
    for(k = 0; k < 16; k++){  
      if(k < 8){  
        value = Ball[i][k] << (7 - k);  
        Word1[0][i] += value;  
      } else {  
        value = Ball[i][k] << (15 - k);  
        Word1[0][i+16] += value;  
      }  
    }  
  }  
}  
//(rather than show them in turn with blinking  
// this part is to show two paddles and ball together to eliminate blinking  
unsigned char combined1[Num_Word][32];  
unsigned char combined2[Num_Word][32];  
unsigned char combined_score[Num_Word][32];  
  
void combine_pattern(){  
  unsigned char i,j,k,m;  
  for (k = 0; k < 32; k++){  
    combined1[0][k] = one[0][k];  
  }  
  for (m = 0; m < 32; m++){  
    combined2[0][m] = one[0][m];  
  }  
  for (i = 0; i < 32; i++){  
      combined1[0][i] &= Word1[0][i];  
      combined1[0][i] &= Display_Up_Buffer[0][i];  
    }   
  for (j = 0; j < 32; j++){  
      combined2[0][j] &= combined1[0][j];  
      combined2[0][j] &= Display_Down_Buffer[0][j];  
    }     
}  
//rather than show them in turn with blinking  
// this part is to show two paddles and ball together to eliminate blinking)  
  
void Clear_Display()// all set '1', which is extinguishing LED  
{  
  unsigned char i,j;  
  for(j = 0 ; j < Num_Word; j++)  
  {  
    for(i = 0 ; i < 32 ;i++)  
    {  
      Display_Up_Buffer[j][i] = 0xFF;  
      Display_Down_Buffer[j][i] = 0xFF;    
    }  
  }  
}  
  
void UpPaddleLeft(){  //move uppaddle to left one pixel  
  if( UpPaddleLocation <= 0)  
  {  
    UpPaddleLocation = UpPaddleLocation;  
  }else{  
    UpPaddleLocation--;  
  }  
}  
  
void UpPaddleRight(){   //move uppaddle to right one pixel  
  if( UpPaddleLocation >= 12 )  
  {  
    UpPaddleLocation = UpPaddleLocation;  
  }else{  
    UpPaddleLocation++;  
  }    
    
}  
  
void DownPaddleLeft(){   //move downpaddle to left one pixel  
  if( DownPaddleLocation <= 0)  
  {  
    DownPaddleLocation = DownPaddleLocation;  
  }else{  
    DownPaddleLocation--;  
  }  
}  
  
void DownPaddleRight(){   //move downpaddle to right one pixel  
  if( DownPaddleLocation >= 12 )  
  {  
    DownPaddleLocation = DownPaddleLocation;  
  }else{  
    DownPaddleLocation++;  
  }    
    
}  
  
void Start(){       // game goes into start stage  
  START_STATE = 1;  
}  
  
void BallSD(){    //ball moves straight down  
  Ball[Row][Column] = 1;  
  Row++;  
  Ball[Row][Column] = 0;  
}  
  
void BallSU(){      //ball moves straight up  
  Ball[Row][Column] = 1;  
  Row--;  
  Ball[Row][Column] = 0;  
}  
  
void BallLD(){    //ball moves left donw in a 45 degree angle  
  Ball[Row][Column] = 1;  
  Column--;  
  Row++;  
  Ball[Row][Column] = 0;  
}  
  
void BallRD(){    //ball moves right down in a 45 degree angle  
  Ball[Row][Column] = 1;  
  Column++;  
  Row++;  
  Ball[Row][Column] = 0;  
}  
  
void BallLU(){    //ball moves left up in a 45 degree angle  
  Ball[Row][Column] = 1;  
  Column--;  
  Row--;  
  Ball[Row][Column] = 0;  
}  
  
void BallRU(){    //ball moves right up in a 45 degree angle  
  Ball[Row][Column] = 1;  
  Column++;  
  Row--;  
  Ball[Row][Column] = 0;  
}  
  
void BallRst(){    //ball goes back to its born point  
  Ball[Row][Column] = 1;  
  Row = 7;  
  Column = 7;  
  Ball[Row][Column] = 0;  
  UpPaddleLocation = 6;  
  DownPaddleLocation = 6;  
  
}  
  
void randstate(){ //ball's trajectory is random chosen  
  switch (rand_state){  
    case 1:  
     BallMov = R_U;  
    break;  
    case 2:  
     BallMov = L_U;  
    break;  
    case 3:  
     BallMov = S_U;  
    break;  
    case 4:  
     BallMov = L_U;  
    break;  
    case 5:  
     BallMov = R_D;  
    break;  
    case 6:  
     BallMov = L_D;  
    break;  
    case 7:  
     BallMov = S_D;  
    break;  
    case 8:  
     BallMov = S_D;  
    break;  
  }  
}  
  
void MovBall(){ //this is function how to decide whether ball's movement need be changed or still keep.  
  switch (BallMov){  
    case S_U :   
      if (Row == 1){  // if ball reaches top paddle  
        BallMov = Change;  
        LastBallMov = S_U;  
        LastBallMov_2 = S_U;  
      }else if(Row == 0){  
        Row++;  
        BallSU();  
        BallMov = Change;  
      }  
      else{  
      BallSU();  
      }  
      break;  
    case S_D :   
      if (Row == 14){// if ball reaches bottom paddle  
        BallMov = Change;  
        LastBallMov = S_D;  
        LastBallMov_2 = S_D;  
      }else if(Row == 15){  
        Row--;  
        BallSD();  
        BallMov = Change;  
      }  
      else{  
      BallSD();  
      }    
      break;  
    case R_D :   
      if (Row == 14 || Column == 15){// if ball reaches bottom paddle at the corner  
        BallMov = Change;  
        LastBallMov = R_D;  
        LastBallMov_2 = R_D;  
      } else if(Row == 15){  
        Row--;  
        BallRD();  
        BallMov = Change;  
      }  
      else{  
      BallRD();  
      }  
      break;  
    case L_D :   
      if (Row == 14 || Column == 0){// if ball reaches bottom paddle at the corner  
        BallMov = Change;  
        LastBallMov = L_D;  
        LastBallMov_2 = L_D;  
      }      else if(Row == 15){  
        Row--;  
        BallLD();  
        BallMov = Change;  
      }  
      else{  
      BallLD();  
      }  
      break;  
    case R_U :  
      if (Row == 1 || Column == 15){// if ball reaches top paddle at the corner  
        BallMov = Change;  
        LastBallMov = R_U;   
        LastBallMov_2 = R_U;  
      }  
      else if(Row == 0){  
        Row++;  
        BallRU();  
        BallMov = Change;  
      }  
      else{  
      BallRU();  
      }  
      break;  
    case L_U :  
      if (Row ==1 || Column == 0){// if ball reaches top paddle at the corner  
        BallMov = Change;  
        LastBallMov = L_U;  
        LastBallMov_2 = L_U;  
      }else if(Row == 0){  
        Row++;  
        BallLU();  
        BallMov = Change;  
      }  
      else{  
        BallLU();  
      }  
      break;  
    case Rst :   
      if( LastBallMov == Rst){  
      BallRst();  
      LastBallMov = S_U;  
      }  
      if( UpPaddleLocation != 6 || DownPaddleLocation != 6){// ball begins to move only after one of players do something to paddle  
      BallMov = S_U;  
      }  
      break;  
    case Change ://considering all possibilities and define next ball's movement when ball is bounced or fall  
      if ( Row == 14)// if ball reaches the bottom of screen  
      {  
        if (Column  == DownPaddleLocation && Column == 0){// when ball hits left down paddle's first pixel at the corner  
                    rand_state = random(1,4);  
          randstate();    
        }  
        else if (Column == DownPaddleLocation){// when ball hits left down paddle's first pixel  
                    rand_state = random(1,4);  
          randstate();   
        }  
        else if (Column == DownPaddleLocation+1 || Column == DownPaddleLocation+2){//when ball hits left down paddle's second and third pixel  
                   rand_state = random(1,4);  
          randstate();   
        }  
        else if (Column  == DownPaddleLocation+3 && Column == 15){//when ball hits left down paddle's last pixel at the corner  
                    rand_state = random(1,4);  
          randstate();     
        }  
        else if (Column  == DownPaddleLocation+3){//when ball hits left down paddle's last pixel  
                    rand_state = random(1,4);  
          randstate();   
        }  
        else if (Column == DownPaddleLocation-1 && LastBallMov == R_D){// if the ball reaches paddle in a 45degree angle, paddle can bounce it back   
          BallMov = L_U;  
        }  
        else if (Column == DownPaddleLocation+4 && LastBallMov == L_D){// if the ball reaches paddle in a 45degree angle, paddle can bounce it back   
          BallMov = R_U;  
        }  
        else{  
         BallMov = LastBallMov;  
         Ball[14][15] = 1;// extinguish this LED  
         Ball[14][0] = 1;// extinguish this LED  
         Row++;  
         cnt+=5;  
        }  
      }  
      else if (Row == 15){ // if ball falls, the opposite side player gain one score  
          p2_score++;  
          BallMov = Rst;  
          LastBallMov = Rst;  
          BallState = SCORE;  
         // transmit(); this is for testing  
        }  
      else if ( Row == 1)// if ball reaches the top of screen, same conditions as it reaches th bottom of screen  
      {  
        if (Column  == UpPaddleLocation && Column == 0){//when ball hits left up pard  
                    rand_state = random(5,8);  
          randstate();   
        }  
        else if (Column == UpPaddleLocation){  
                   rand_state = random(5,8);  
          randstate();   
        }  
        else if (Column == UpPaddleLocation +1 || Column == UpPaddleLocation +2){  
                    rand_state = random(5,8);  
          randstate();   
        }  
        else if (Column == UpPaddleLocation+3 && Column == 15){  
                   rand_state = random(5,8);  
          randstate();   
        }  
        else if (Column  == UpPaddleLocation+3){//when ball hits left down pard  
                    rand_state = random(5,8);  
          randstate();   
        }  
        else if (Column == UpPaddleLocation-1 && LastBallMov == R_U){  
          BallMov = L_D;  
        }  
        else if (Column == UpPaddleLocation+4 && LastBallMov == L_U){  
          BallMov = R_D;  
        }  
        else{  
        BallMov = LastBallMov;  
        Ball[1][15] = 1;  
        Ball[1][0] = 1;  
        Row--;  
        cnt+=5;  
        }  
      }  
      else if (Row == 0){  
          p1_score++;  
          BallMov = Rst;  
          LastBallMov = Rst;  
          BallState = SCORE;  
         // transmit();  
        }  
      else if ( Column == 0 && Row >= 1 && Row <= 14){  
        if (LastBallMov == L_D){  
          BallMov = R_D;  
        }else if (LastBallMov == L_U){  
          BallMov = R_U;  
          }  
      }  
      else if ( Column == 15 && Row >= 1 && Row <= 14){  
        if (LastBallMov == R_D){  
          BallMov = L_D;  
        }else if (LastBallMov == R_U){  
          BallMov = L_U;  
        }  
      }  
      else {}  
      BALL_SPEED = random(8,9);  
      break;  
    default : break;  
  }  
}  
void setup() {  
  // put your setup code here, to run once:  
  pinMode(LEDARRAY_D, OUTPUT);   
  pinMode(LEDARRAY_C, OUTPUT);  
  pinMode(LEDARRAY_B, OUTPUT);  
  pinMode(LEDARRAY_A, OUTPUT);  
  pinMode(LEDARRAY_G, OUTPUT);  
  pinMode(LEDARRAY_DI, OUTPUT);  
  pinMode(LEDARRAY_CLK, OUTPUT);  
  pinMode(LEDARRAY_LAT, OUTPUT);  
  pinMode(gamepad1X, INPUT);  
  pinMode(gamepad1Z, INPUT);  
  pinMode(gamepad2X, INPUT);  
  pinMode(gamepad2Z, INPUT);  
  Clear_Display();  
  Serial.begin(9600);  
  setupBlueToothConnection();  
  /**** these four interrupts are not game machine's function. it is for testing when we use pushbutton to control paddles 
  attachInterrupt(digitalPinToInterrupt(interruptPinUpLeft), UpPaddleLeft, FALLING); 
  attachInterrupt(digitalPinToInterrupt(interruptPinUpRight), UpPaddleRight, FALLING); 
  attachInterrupt(digitalPinToInterrupt(interruptPinDownLeft), DownPaddleLeft, FALLING); 
  attachInterrupt(digitalPinToInterrupt(interruptPinDownRight), DownPaddleRight, FALLING); 
  ****/  
  attachInterrupt(digitalPinToInterrupt(interruptStart), Start, FALLING);// when press start pushbutton, game start  
  
}  
  
void loadleftscore(){       //show player1's score  
  unsigned char i;  
  for( i=0;i<32;i++){  
    Display_left_Buffer[0][i] = left_score[p1_score][i];  
  }  
}  
  
void loadrightscore(){       //show player2's score  
  unsigned char i;  
  for( i=0;i<32;i++){  
    Display_right_Buffer[0][i] = right_score[p2_score][i];  
  }  
}  
  
  
void combine_sacores(){       //show player1's score and player2's score together on the screen  
  unsigned char i,j,k,m;  
  for (k = 0; k < 32; k++){  
    combined_score[0][k] = one[0][k];  
  }  
  for (i = 0; i < 32; i++){  
      combined_score[0][i] &= Display_left_Buffer[0][i];  
      combined_score[0][i] &=  Display_right_Buffer[0][i];  
    }  
}   
  
void loop() {   //keep running this game  
  // put your main code here, to run repeatedly:  
  ball_state_machine();  
}  
  
void accelerometer2(){// this is function that how accelerometer controls paddle's movement  
  float a2;  
  float A2;  
  for (int i=0;i<30;i++)// collect 30 data rather than one  
  {  
    val_x2+=analogRead(gamepad2X);  
    val_z2+=analogRead(gamepad2Z);  
  }  
   val_x2=val_x2/10;  
   val_z2=val_z2/10;  
   b2=(double) (abs(val_x2-320))/(abs(val_z2-320));  
   a2=atan(b2);  
   A2 = a2/3.14*180;  
   //Serial.println(A2);  this is for adjusting and testing  
   if (A2<45) {  
    leftcount2++;  
   }  
   else if(A2>50) {  
    rightcount2++;  
   }  
   else{  
    leftcount2=0;      
    rightcount2=0;  
   }  
   if(leftcount2>5){  // decide after player keep its gesture for a reasonably time  
    DownPaddleLeft();  
    leftcount2 = 0;  
   }  
   if(rightcount2>5){ // decide after player keep its gesture for a reasonably time  
    DownPaddleRight();  
    rightcount2 = 0;  
   }  
   val_z2=0;  
   val_x2=0;  
}  
  
void accelerometer1(){// this is function that how accelerometer controls paddle's movement  
  float a1;  
  float A1;  
  for (int i=0;i<30;i++)// collect 30 data rather than one  
  {  
    val_x1+=analogRead(gamepad1X);  
    val_z1+=analogRead(gamepad1Z);  
  }  
   val_x1=val_x1/10;  
   val_z1=val_z1/10;  
   b1=(double) (abs(val_x1-320))/(abs(val_z1-320));  
   a1=atan(b1);  
   A1 = a1/3.14*180;  
   //Serial.print(A1);              //the value of Angle  this is for adjusting and testing  
   //Serial.print("    ");  
   if (A1<40) {  
    leftcount1++;  
   }  
   else if(A1>45) {  
    rightcount1++;  
   }  
   else{  
    leftcount1=0;      
    rightcount1=0;  
   }  
   if(leftcount1>5){  // decide after player keep its gesture for a reasonably time  
    UpPaddleLeft();  
    leftcount1 = 0;  
   }  
   if(rightcount1>5){  // decide after player keep its gesture for a reasonably time  
    UpPaddleRight();  
    rightcount1 = 0;  
   }  
   val_z1=0;  
   val_x1=0;  
}  
  
//main FSM to control the game status  
void ball_state_machine(){  
  unsigned char i,j;  
  switch(BallState){  
    case WAIT:  
    p2_score = 0;  
    p1_score = 0;  
    Display(Press_Start);  
    if (START_STATE == 1){  // when start button pushed, game goes into start stage  
      BallState = START;  
      START_STATE = 0;  
    }else{  
      BallState = WAIT;  
    }  
    if(show_start == 1){  // when game goes into Wait stage, transmit this information to smartphone  
       blueToothSerial.println(" Waiting for Start");  
       blueToothSerial.println("\n");  
       blueToothSerial.flush();  
       show_start = 0;  
    }  
    break;  
    case START:         // these are functions running during the game  
      accelerometer1();  
      accelerometer2();  
      loadUpPaddle();  
      loadDownPaddle();  
      cnt++;  
      SceneToWord();  
      combine_pattern();  
      Display(combined2);  
      if (cnt >= BALL_SPEED){// the more delay, the slower the ball is  
        cnt = 0;  
        MovBall();  
      }  
    break;  
    case SCORE:     // when one player gets score, game goes SCORE stage to show score and transmit it to smartphone  
    count++;  
    loadrightscore();  
    loadleftscore();  
    combine_sacores();  
    if (show_score==1){  
    String str="Player1:"+String(p1_score) +"     " + "Player2:"+String(p2_score);  
    blueToothSerial.print(str);  
    blueToothSerial.println("\n");  
    blueToothSerial.flush();  
    show_score = 0;  
    }  
    Display(combined_score);  
    if  (count == 300){// this delay decides how long this state last  
      if (p1_score == 5 or p2_score == 5){  
      BallState = END;// if one player reaches five score, this round of game is over  
      count = 0;  
    }else{  
      BallState = START;// if no one reaches five, keep going   
      show_score=1;  
      count = 0;  
    }  
    }  
    break;  
    case END:  
    if (p1_score == 5){  
    blueToothSerial.println("Player1 Win"); // transmit game result to smartphone  
    blueToothSerial.println("\n");  
    for(int t = 0; t<64; t++){  // this delay decides how long pattern "Player1 Win" shows on the screen  
     unsigned int i;  
     for(i = 0 ; i < 10; i ++)     // this delay decides rolling speed  
     {  
       Display(Display_Swap_Buffer);  
     }  
     Calc_Shift_P1();             // shifting display P1 WIN on LED matrix  
     Shift_Count++;  
     if(Shift_Count == 32)  
     {  
       Shift_Count = 0;  
     }  
    }  
    }  
    if (p2_score == 5){  
    blueToothSerial.println("Player2 Win");  
    blueToothSerial.println("\n");  
    for(int t = 0; t<64; t++){// this delay decides how long pattern "Player1 Win" shows on the screen  
     unsigned int i;  
     for(i = 0 ; i < 10; i ++)          // this delay decides rolling speed  
     {  
       Display(Display_Swap_Buffer);  
     }  
     Calc_Shift_P2();                 // shifting display P2 WIN on LED matrix  
     Shift_Count++;  
     if(Shift_Count == 32)  
     {  
       Shift_Count = 0;  
     }  
    }  
    }  
      
    BallState = WAIT;  
    START_STATE = 0;  
    show_start =1;  
    show_score = 1;  
    break;  
    default: break;  
  }  
}  
  
  
void loadUpPaddle(){          //send top paddle to buffer and ready to show it on screen  
  unsigned char i;  
  for( i=0;i<32;i++){  
    Display_Up_Buffer[0][i] = UpPaddle[UpPaddleLocation][i];  
  }  
}  
  
void loadDownPaddle(){          //send down paddle to buffer and ready to show it on screen  
  unsigned char i;  
  for( i=0;i<32;i++){  
    Display_Down_Buffer[0][i] = DownPaddle[DownPaddleLocation][i];  
  }  
}  
  
void Calc_Shift_P1()// this is the function of how to do rolling, we only use rolling for displaying the result of game.  
                    // There are two result: Player1 win and Player2 win  
{  
  unsigned char i;  
  
    for(i = 0;i < 16;i++)  
    {  
      if((Display_Swap_Buffer[0][16+i]&0x80) == 0)                    
      {  
        Display_Swap_Buffer[0][i] = (Display_Swap_Buffer[0][i] << 1)&0xfe;        
      }  
      else  
      {  
        Display_Swap_Buffer[0][i] = (Display_Swap_Buffer[0][i] << 1)|0x01;        
      }  
  
      if(Shift_Count < 8)  
      {  
        Shift_Bit = P1_WIN[0][i]&temp;  
      }  
      else if(Shift_Count < 16)  
      {  
        Shift_Bit = P1_WIN[0][16+i]&temp;  
      }  
      else  
      {  
        Shift_Bit = 1;          
      }  
  
      if( Shift_Bit == 0)                             
      {  
        Display_Swap_Buffer[0][16+i] = (Display_Swap_Buffer[0][16+i] << 1)&0xfe;      
      }  
      else  
      {  
        Shift_Bit = 1;  
        Display_Swap_Buffer[0][16+i] = (Display_Swap_Buffer[0][16+i] << 1)|0x01;      
      }  
        
    }  
    temp = (temp>>1)&0x7f;  
    if(temp == 0x00)  
    {  
      temp = 0x80;  
    }  
}  
  
void Calc_Shift_P2()// this is the function of how to do rolling, we only use rolling for displaying the result of game.  
                    // There are two result: Player1 win and Player2 win  
{  
  unsigned char i;  
  
    for(i = 0;i < 16;i++)  
    {  
      if((Display_Swap_Buffer[0][16+i]&0x80) == 0)                    
      {  
        Display_Swap_Buffer[0][i] = (Display_Swap_Buffer[0][i] << 1)&0xfe;        
      }  
      else  
      {  
        Display_Swap_Buffer[0][i] = (Display_Swap_Buffer[0][i] << 1)|0x01;        
      }  
  
      if(Shift_Count < 8)  
      {  
        Shift_Bit = P2_WIN[0][i]&temp;  
      }  
      else if(Shift_Count < 16)  
      {  
        Shift_Bit = P2_WIN[0][16+i]&temp;  
      }  
      else  
      {  
        Shift_Bit = 1;          
      }  
  
      if( Shift_Bit == 0)                             
      {  
        Display_Swap_Buffer[0][16+i] = (Display_Swap_Buffer[0][16+i] << 1)&0xfe;      
      }  
      else  
      {  
        Shift_Bit = 1;  
        Display_Swap_Buffer[0][16+i] = (Display_Swap_Buffer[0][16+i] << 1)|0x01;      
      }  
        
    }  
    temp = (temp>>1)&0x7f;  
    if(temp == 0x00)  
    {  
      temp = 0x80;  
    }  
}  
//*************************************************************  
  
//this this the function of how to control LED Dot Matrix  
void Display(unsigned char dat[][32])            
{  
  unsigned char i;  
  
  for( i = 0 ; i < 16 ; i++ )  
  {  
    digitalWrite(LEDARRAY_G, HIGH);     
      
    Display_Buffer[0] = dat[0][i];//dat[][] is Display_Swap_Buffer      
    Display_Buffer[1] = dat[0][i+16];  
  
    Send(Display_Buffer[1]);  
    Send(Display_Buffer[0]);  
  
    digitalWrite(LEDARRAY_LAT, HIGH);         
    delayMicroseconds(1);  
    
    digitalWrite(LEDARRAY_LAT, LOW);  
    delayMicroseconds(1);  
  
    Scan_Line(i);           
  
    digitalWrite(LEDARRAY_G, LOW);  
      
    delayMicroseconds(300);;    
  }   
}  
  
//****************************************************  
//this this the function of how to control LED Dot Matrix  
//****************************************************  
void Scan_Line( unsigned char m)  
{   
  switch(m)  
  {  
    case 0:       
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW);            
      break;  
    case 1:           
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH);       
      break;  
    case 2:           
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW);       
      break;  
    case 3:           
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH);      
      break;  
    case 4:  
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW);       
      break;  
    case 5:  
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH);      
      break;  
    case 6:  
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW);      
      break;  
    case 7:  
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH);       
      break;  
    case 8:  
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW);       
      break;  
    case 9:  
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH);      
      break;    
    case 10:  
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW);      
      break;  
    case 11:  
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH);       
      break;  
    case 12:  
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW);      
      break;  
    case 13:  
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH);       
      break;  
    case 14:  
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW);       
      break;  
    case 15:  
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH);      
      break;  
    default : break;    
  }  
}  
  
//****************************************************  
 //this this the function of how to control LED Dot Matrix       
//****************************************************  
void Send( unsigned char dat)  
{  
  unsigned char i;  
  digitalWrite(LEDARRAY_CLK, LOW);  
  delayMicroseconds(1);;    
  digitalWrite(LEDARRAY_LAT, LOW);  
  delayMicroseconds(1);;  
  
  for( i = 0 ; i < 8 ; i++ )  
  {  
    if( dat&0x01 )  
    {  
      digitalWrite(LEDARRAY_DI, HIGH);    
    }  
    else  
    {  
      digitalWrite(LEDARRAY_DI, LOW);  
    }  
  
    delayMicroseconds(1);  
    digitalWrite(LEDARRAY_CLK, HIGH);           
      delayMicroseconds(1);  
    digitalWrite(LEDARRAY_CLK, LOW);  
      delayMicroseconds(1);     
    dat >>= 1;  
        
  }       
}  
//Bluetooth setup  
void setupBlueToothConnection()  
{  
    blueToothSerial.begin(38400);                           // Set BluetoothBee BaudRate to default baud rate 38400  
    blueToothSerial.print("\r\n+STWMOD=0\r\n");             // set the bluetooth work in slave mode  
    blueToothSerial.print("\r\n+STNA=GROUP25\r\n");    // set the bluetooth name as "SeeedBTSlave"  
    blueToothSerial.print("\r\n+STOAUT=1\r\n");             // Permit Paired device to connect me  
    blueToothSerial.print("\r\n+STAUTO=0\r\n");             // Auto-connection should be forbidden here  
    blueToothSerial.print("\r\n+STPIN=0000\r\n"); //set pin to 0000  
    delay(2000);                                            // This delay is required.  
    blueToothSerial.print("\r\n+INQ=1\r\n");                // make the slave bluetooth inquirable  
    Serial.println("The slave bluetooth is inquirable!");  
    delay(2000);                                            // This delay is required.  
    blueToothSerial.flush();  
}  
