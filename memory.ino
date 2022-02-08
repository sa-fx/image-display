/*Based on code from QD electronic
*
* File Name:            memory.ino
* Version:              14.12
* Hardware Requirement: Arduino Uno
*/
/*This version of the program is designed for use with images formatted
  outside of Node JS. Node JS images will NOT work here!*/

/////////////////////////////// Libraries //////////////////////////////

#include <SD.h>
#include <SPI.h>
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_KBV my_lcd(ILI9486,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset
//if the IC model is not known and the modules is readable,you can use this constructed function
//LCDWIKI_KBV my_lcd(320,480,A3,A2,A1,A0,A4);//width,height,cs,cd,wr,rd,reset

////////////////////////////// Constants ///////////////////////////////////

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define PIXEL_NUMBER  (my_lcd.Get_Display_Width()/4)
#define NUMBER_OF_FILES 100//The maximum number of files expected on the SD card
#define FILE_NAME_SIZE_MAX 20//The length of the names of said files on the SD card

//A counter to prevent repeated images
int lastNumber = 0;

char digits[] = {'060', '061', '062', '063', '064', '065', '066', '067', '068', '069' };
//A reference array for the numbers 0 - 9

uint32_t bmp_offset = 0;
uint16_t s_width = my_lcd.Get_Display_Width();  
uint16_t s_heigh = my_lcd.Get_Display_Height();
//int16_t PIXEL_NUMBER;

uint16_t read_16(File fp)
{
    uint8_t low;
    uint16_t high;
    low = fp.read();
    high = fp.read();
    return (high<<8)|low;
}

uint32_t read_32(File fp)
{
    uint16_t low;
    uint32_t high;
    low = read_16(fp);
    high = read_16(fp);
    return (high<<8)|low;   
 }
 
bool analysis_bpm_header(File fp)
{
    if(read_16(fp) != 0x4D42)
    {
      return false;  
    }
    //get bpm size
    read_32(fp);
    //get creator information
    read_32(fp);
    //get offset information
    bmp_offset = read_32(fp);
    //get DIB infomation
    read_32(fp);
    //get width and heigh information
    uint32_t bpm_width = read_32(fp);
    uint32_t bpm_heigh = read_32(fp);
    if((bpm_width != s_width) || (bpm_heigh != 16776992))
    {
      return false; 
    }
    if(read_16(fp) != 1)
    {
        return false;
    }
    read_16(fp);
    if(read_32(fp) != 0)
    {
      return false; 
     }
     return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void draw_bmp_picture(File fp)
{
  uint16_t i,j,k,l,m=0;
  uint8_t bpm_data[PIXEL_NUMBER*3] = {0};
  uint16_t bpm_color[PIXEL_NUMBER];
  fp.seek(bmp_offset);
  for(i = 0;i < s_heigh;i++)
  {
    for(j = 0;j<s_width/PIXEL_NUMBER;j++)
    {
      m = 0;
      fp.read(bpm_data,PIXEL_NUMBER*3);
      for(k = 0;k<PIXEL_NUMBER;k++)
      {
        bpm_color[k]= my_lcd.Color_To_565(bpm_data[m+2], bpm_data[m+1], bpm_data[m+0]); //change to 565
        m +=3;
      }
      for(l = 0;l<PIXEL_NUMBER;l++)
      {
        my_lcd.Set_Draw_color(bpm_color[l]);
        my_lcd.Draw_Pixel(j*PIXEL_NUMBER+l,i);
      }    
     }
   }    
}


////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() 
{

   Serial.begin(9600);
   my_lcd.Init_LCD();
   Serial.println(my_lcd.Read_ID(), HEX);

   my_lcd.Fill_Screen(BLUE);
   //s_width = my_lcd.Get_Display_Width();  
   //s_heigh = my_lcd.Get_Display_Height();
   //PIXEL_NUMBER = my_lcd.Get_Display_Width()/4;

  //Init SD_Card
   pinMode(10, OUTPUT);
   
    if (!SD.begin(10))//If the SD card can't be found, the program won't continue
    {
      my_lcd.Set_Text_Back_colour(BLUE);
      my_lcd.Set_Text_colour(WHITE);    
      my_lcd.Set_Text_Size(1);
      my_lcd.Print_String("Well, this is awkward...",75,0);
      my_lcd.Print_String("The SD card could not be opened",0,10);
      my_lcd.Print_String("Please check the SD card is inserted correctly and",0,20);
      my_lcd.Print_String("reset the board",0,30);
      my_lcd.Print_String("Details - SD Card Init fail",0,50);
      while(1);
    }
  //randomSeed(analogRead(A0));
}

/////////////////////////////////////////////////////////////////////////////////

void loop() 
{
    //Initialise variables
    int fileNumber = 0;
    int tens = 0;
    int units = 0;
    String(filePath) = "";

    File bmp_file;
      //Pick a random number for an image to display
      fileNumber = rand() % NUMBER_OF_FILES;
      while(fileNumber == lastNumber){
        Serial.println("Number repeated: Choosing new one");//Loop until image is not repeated
        fileNumber = rand() % NUMBER_OF_FILES;
      }
      Serial.println();
      Serial.print("File number: ");
      Serial.println(fileNumber);

      String fileStr = String(fileNumber);//Sets the image to be opened
      Serial.print("Length of file number: ");
      Serial.println(fileStr.length());

      if(fileStr.length() != 1) {//Code for when the file name is two digits long (could be modified to work for numbers above 99)
        tens = (fileNumber/10)%10;//Find the tens digit
        tens = digits[tens];//Find the tens digit in Ascii
        units = fileNumber%10;//Find the units digit
        units = digits[units];//Find the units digit in Ascii
        Serial.print("Value of tens digit: ");
        Serial.println(tens);
        Serial.print("Value of units digit: ");
        Serial.println(units);

        char file_name[ ] = { tens, units, 46, 98, 109, 112 };// file_name = [ tens units . B M P]

        filePath = file_name;//Set the file path

        Serial.print("Length of file path: ");
        Serial.println(filePath.length());

        filePath.remove(6, filePath.length() - 5);//Remove extra characters to make file path correctly formatted

        Serial.print("New length of file path: ");
        Serial.println(filePath.length());
      }
      else{//Code for when the file name is only one digit long
        char numb = digits[fileNumber];

        char file_name[ ] = { numb, 46, 98, 109, 112 };//file_name = [ numb . B M P]

        filePath = file_name;
        
        Serial.print("Length of file path: ");
        Serial.println(filePath.length());

        filePath.remove(5, filePath.length() - 4);

        Serial.print("New length of file path: ");
        Serial.println(filePath.length());
      }
       Serial.print("Value of file path: ");
       Serial.println(filePath);
       bmp_file = SD.open(filePath);//Open the SD card and read the current image

       if(!bmp_file)//If the check for the bmp image fails, there is no image
       {
            my_lcd.Set_Text_Back_colour(BLUE);
            my_lcd.Set_Text_colour(WHITE);    
            my_lcd.Set_Text_Size(1);
            my_lcd.Print_String("404 - BMP image not found!",0,10);
            Serial.println("404 - BMP image not found!");
            return;
        }
        if(!analysis_bpm_header(bmp_file))//If the header comes back incorrect or corrupted, it can't be used
        {  
            my_lcd.Set_Text_Back_colour(BLUE);
            my_lcd.Set_Text_colour(WHITE);    
            my_lcd.Set_Text_Size(1);
            my_lcd.Print_String("406 - Bad BMP image!",0,0);
            Serial.println("406 - Bad BMP image!");
            return;
        }
          draw_bmp_picture(bmp_file);//Call the function to draw the image
         bmp_file.close();//Stop reading the file
         lastNumber = fileNumber;//Set new last image to prevent looping
         delay(4000);//Pause to show image
}
