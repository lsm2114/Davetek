#include "PinNamesTypes.h"
#include "mbed.h"
#include "SCENEMANAGER.h"
#include "LCD1602.h"
#include "mbed_thread.h"
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ratio>
#include "Timers.h"
#include "randLIB.h"


const int filledSquare[8] = { // Custom characters are stored as bit patterns (5x8 matrix)
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111
};

/*
const int filledSquare[8] = { // Custom characters are stored as bit patterns (5x8 matrix)
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b00000
};
*/

LCD1602 lcd(A1, A0, A2, A3, D4, D5, D6, D7);
SceneManager sceneManager(lcd);



DigitalIn B1(A5, PullUp); //left most button
DigitalIn B2(A4, PullUp);// all other pins have a constant output of 1 for some reason
DigitalIn B3(A3, PullUp);
DigitalIn B4(A2, PullUp);//right most button

Thread buttonThread;




//these functions just allow scenes to be made quicker, dont have to reference sceneManager every time
void NewScene(int ID){
    sceneManager.NewScene(ID);
}
void NewButton(int sceneID, int nextScene, const char *text, int column, int row){
    sceneManager.NewButton(sceneID, nextScene, text, column, row);
}
void NewText(int sceneID, const char *text, int column, int row){
    sceneManager.NewText(sceneID, text, column, row);
}
void NewCustomChar(int sceneID, const int *customChar, int column, int row){
    sceneManager.NewChar(sceneID, customChar, column, row);

    //lcd.setCursor(column, row);
    //lcd.writeCustomChar(0);
}

void DaveSaysLoop(Scene &baseScene);
//this function is used to exit the loop when the player is wrong in Dave Says
void WrongFunc(Scene &baseScene){
    
    baseScene.ClearScene(lcd);
    NewText(0, "WRONG!!!!!!!!!", 0, 0);
    sceneManager.DisplaySceneByID(0);
    thread_sleep_for(2000);

    DaveSaysLoop(baseScene);
}



//cursor flashing
void buttonThreadFn(){

    while(true){
        lcd.setCursor(14, 0);
        lcd.write("<");
        thread_sleep_for(800);
        lcd.setCursor(14, 0);
        lcd.write(" ");
        thread_sleep_for(800);
    }
}

//this is the code for the main loop of dave says
void DaveSaysLoop(Scene &baseScene){
    lcd.loadCustomChar(0, filledSquare);
    //random number generators dont seem to be working properly, a starting point in this array will be chosen randomly instead
    int numArray[] = {0, 2, 1, 2, 0, 3, 1, 1 ,3 ,2, 1, 0, 0, 1, 2, 0, 3, 3, 
                    2, 1, 0, 1, 3, 2, 3, 0, 1, 0, 2, 2, 0, 0, 1, 2, 3, 1, 0, 
                    1, 0, 2, 3, 0, 3, 3, 2, 1, 0, 0, 1, 3, 1, 1, 3};

    int numArrayLength = 53;

    int numSequenceArray[100]; // max number of sequenced numbers
    int numSequence = 0;//number of numbers in the sequence

    int counter = 0; //counter is used to determine the starting point in numArray

    baseScene.ClearScene(lcd);

    NewText(0, "READY?", 0, 0);
    sceneManager.DisplaySceneByID(0); //display the scene


    //this is the main while loop which contains the whole game
    while(true){
        if(counter >= numArrayLength){
            counter = 0;
        }
        counter ++;

        //if button input
        if(B1){
            int random =0;
            baseScene.ClearScene(lcd);//clears all text in the scene

            while(true){
                thread_sleep_for(1000); //time before sequence starts

                if(counter >= numArrayLength){ //counter cant go out of arrays bounds
                    counter = 0;
                }

                random = numArray[counter];//chooses first random number from numArray
                counter++;

                numSequenceArray[numSequence] = random;
                numSequence++;
                
                for(int i = 0; i < numSequence; i++){
                    baseScene.ClearScene(lcd);//clears all text in the scene
                    //thread_sleep_for(100);

                    switch(numSequenceArray[i]){       //decides which area to print text in.
                        case 0: 
                        //NewText(0, "X", 0, 0);
                        NewText(0, "1", 0, 0);// Scene 0, text, position column, row
                        NewCustomChar(0, filledSquare, 1, 0);
                        NewCustomChar(0, filledSquare, 2, 0);
                        break;
                        case 1:
                        //NewText(0, "X", 15, 0);
                        NewCustomChar(0, filledSquare, 15, 0);
                        NewCustomChar(0, filledSquare, 14, 0);
                        NewText(0, "2", 13, 0);
                        break;
                        case 2:
                        //NewText(0, "X", 0, 1);
                        NewText(0, "3", 0, 1);
                        NewCustomChar(0, filledSquare, 1, 1);
                        NewCustomChar(0, filledSquare, 2, 1);
                        break;
                        case 3:
                        //NewText(0, "X", 15, 1);
                        NewCustomChar(0, filledSquare, 15, 1);
                        NewCustomChar(0, filledSquare, 14, 1);
                        NewText(0, "4", 13, 1);
                        break;
                        
                    }

                    //this little block of code looks a bit messy, but allows the characters to be displayed on the screen for a set amount of time
                    thread_sleep_for(300);
                    sceneManager.DisplaySceneByID(0); //custom characters currently arent compatible with scenes. Print them seperately.
                    thread_sleep_for(800);
                    baseScene.ClearScene(lcd);
                    thread_sleep_for(500);
                }

                //
                // needs work from this onwards. Create a system allowing the user to
                // input the sequence using the buttons.
                //


                NewText(0, "Sequence?", 0, 0);
                sceneManager.DisplaySceneByID(0); //display the scene

                int i = 0;
                bool buttonStillPressed = 1; //this bool is used to make sure the loop detects multiple disinct button presses
                bool b1=0, b2=0, b3=0, b4=0;
                while(i != numSequence){

                    for(i = 0; i < numSequence; i++){
                        
                        while(true){
                            
                            
                            //prevents loop being triggered until all buttons are unpressed
                            if(!B1 && !B2 && !B3 && !B4){
                                buttonStillPressed = 0;
                                thread_sleep_for(10);
                            }

                            if(buttonStillPressed == 0){
                                if(B1){
                                    b1 = true;
                                }
                                else if(B2){
                                    b2 = true;
                                }
                                else if(B3){
                                    b3 = true;
                                }
                                else if(B4){
                                    b4 = true;
                                }
                            }


                            //printf("SEQUENCE ARRAY: %d  ", numSequenceArray[i]);
                            if(b1){
                                b1=0, b2=0, b3=0, b4=0;
                                printf("B1 pressed  ");
                                if(numSequenceArray[i] == 0){                                         
                                    buttonStillPressed = 1;
                                    break;                           
                                }
                                else{
                                baseScene.ClearScene(lcd);
                                WrongFunc(baseScene);
                                }
                                
                            }
                            else if(b2){
                                b1=0, b2=0, b3=0, b4=0;
                                printf("B2 pressed  ");
                                if(numSequenceArray[i] == 1){ 
                                    buttonStillPressed = 1;
                                    break;                           
                                }
                                else{
                                baseScene.ClearScene(lcd);
                                WrongFunc(baseScene);
                                }
                            }
                            else if(b3){
                                b1=0, b2=0, b3=0, b4=0;
                                printf("B3 pressed  ");
                                if(numSequenceArray[i] == 2){ 
                                    buttonStillPressed = 1;
                                    break;                           
                                }
                                else{
                                baseScene.ClearScene(lcd);
                                WrongFunc(baseScene);
                                }
                            }
                            else if(b4){
                                b1=0, b2=0, b3=0, b4=0;
                                printf("B4 pressed  ");
                                if(numSequenceArray[i] == 3){ 
                                    buttonStillPressed = 1;
                                    break;                           
                                }
                                else{
                                baseScene.ClearScene(lcd);
                                WrongFunc(baseScene);
                                }
                            }

                            thread_sleep_for(30);
                        }
                    }                   
                }        
            }
        }
        thread_sleep_for(10);
    }
}

void quickDrawSinglePlayerGame(Scene &quickDrawScene){
    quickDrawScene.ClearScene(lcd);

    bool gameOver = false; //The function will return when this becomes true (at the end of the game)
    bool roundOver = false; //This checks if the player has been penalised and cannot continue to play during that particular round
    bool playAgain = false; //This offers the player if they want to play again
    
    Timers timer; 
    
    int react = 0; //The amount of time until the prompt appears on screen
    int average = 0; //The average reaction time of the player calculated at the end of the game
    int reaction[5]; //Holds the individual reaction times of the player
    int lengthOfGame = 5; //This is the amount of rounds in a game, will be useful for programmability

    //Welcome message
    NewText(1, "WELCOME TO", 0, 0); 
    NewText(1, "QUICK DRAW", 0, 1);
    sceneManager.DisplaySceneByID(1);
    thread_sleep_for(1000);


    //Game begin
    while(gameOver == false){
        //Use a for loop to determine the amount of rounds
        for(int i = 0; i < lengthOfGame; i++){

            //Display what round it is and give a prompt to the user that it is starting
            quickDrawScene.ClearScene(lcd);
            NewText(1, "ROUND ", 0, 0); //fix
            sceneManager.DisplaySceneByID(1);
            thread_sleep_for(500);
            quickDrawScene.ClearScene(lcd);

            //Generate a random amount of time in milliseconds (not microseconds)
            randLIB_seed_random();
            react = randLIB_get_8bit() * 15;
            timer.start();

            while(timer.elapsed() < react && roundOver == false){
                if(B1 || B2 || B3 || B4){
                    timer.stop();
                    NewText(1, "TOO EARLY", 0, 0);
                    sceneManager.DisplaySceneByID(1);
                    thread_sleep_for(1000);
                    quickDrawScene.ClearScene(lcd);

                    reaction[i] = 5000; //penalty for being too early
                    roundOver = true;   
                }
            }

            if(roundOver == false){
                NewText(1, "REACT" , 0, 0); //Use backlight here
                sceneManager.DisplaySceneByID(1);
            }

            while(timer.elapsed() >= react && timer.elapsed() < 5000 && roundOver == false){
                if(B1 || B2 || B3 || B4){
                    timer.stop();
                    quickDrawScene.ClearScene(lcd);
                    NewText(1, "REACTION TIME", 0, 0); //fix
                    sceneManager.DisplaySceneByID(1);
                    thread_sleep_for(1000);
                    quickDrawScene.ClearScene(lcd);

                    reaction[i] = timer.elapsed() - react; //No penalty, just the reaction time of the player
                    roundOver = true;
                }
            }
            if(timer.elapsed() >= 5000){
                timer.stop();
                NewText(1, "TOO LATE", 0, 0); //fix
                sceneManager.DisplaySceneByID(1);
                thread_sleep_for(1000);
                quickDrawScene.ClearScene(lcd);

                reaction[i] = 5000; // penalty for being too late
            }
            roundOver = false;
        }
        //Calculate average reaction time
        for(int i = 0; i < lengthOfGame; i++){
            average = average + reaction[i];
        }
        average = average/lengthOfGame;

        NewText(1, "YOUR REACTION", 0, 0);
        NewText(1, "TIME", 0, 1); //add in the average here with the fix
        sceneManager.DisplaySceneByID(1);
        thread_sleep_for(2000);
   
        
        NewText(1, "PLAY AGAIN", 0, 0);
        NewText(1, "B1 IS Y B4 IS N", 0, 1);
        sceneManager.DisplaySceneByID(1);
        playAgain = false;
        while(playAgain == false){
            if(B1){
                playAgain = true;
            }
            if(B4){
                gameOver = true;
                playAgain = true;
            }
        }
    } 
}

void quickDrawMultiplayerGameHost(Scene &quickDrawScene){
    //The game needs to start simulatenously for both players
    //Send a ping to the joined player here
    //the ping needs to tell the guest that the quickDrawMultiplayerGameGuest needs to start running
    quickDrawScene.ClearScene(lcd);

    bool gameOver = false; //The function will return when this becomes true (at the end of the game)
    bool roundOver = false; //This checks if the player has been penalised and cannot continue to play during that particular round
    bool playAgain = false; //This offers the player if they want to play again
    
    Timers timer; 
    
    int react = 0; //The amount of time until the prompt appears on screen
    int average = 0; //The average reaction time of the player calculated at the end of the game
    int lengthOfGame = 5; //This is the amount of rounds in a game, will be useful for programmability
    int reaction[lengthOfGame]; //Holds the individual reaction times of the player
    int guestAverage = 0; 

    //Welcome message
    NewText(1, "MULTIPLAYER", 0, 0); 
    NewText(1, "QUICK DRAW", 0, 1);
    sceneManager.DisplaySceneByID(1);
    thread_sleep_for(1000);

    //Game begin
    while(gameOver == false){
        //Use a for loop to determine the amount of rounds
        for(int i = 0; i < lengthOfGame; i++){

            //Display what round it is and give a prompt to the user that it is starting
            quickDrawScene.ClearScene(lcd);
            NewText(1, "ROUND ", 0, 0); //fix
            sceneManager.DisplaySceneByID(1);
            thread_sleep_for(500);
            quickDrawScene.ClearScene(lcd);

            //Generate a random amount of time in milliseconds (not microseconds)
            randLIB_seed_random();
            react = randLIB_get_8bit() * 15;
            timer.start();

            while(timer.elapsed() < react && roundOver == false){
                if(B1 || B2 || B3 || B4){
                    timer.stop();
                    NewText(1, "TOO EARLY", 0, 0);
                    sceneManager.DisplaySceneByID(1);
                    thread_sleep_for(1000);
                    quickDrawScene.ClearScene(lcd);

                    reaction[i] = 5000; //penalty for being too early
                    roundOver = true;   
                }
            }

            if(roundOver == false){
                NewText(1, "REACT" , 0, 0); //Use backlight here
                sceneManager.DisplaySceneByID(1);
            }

            while(timer.elapsed() >= react && timer.elapsed() < 5000 && roundOver == false){
                if(B1 || B2 || B3 || B4){
                    timer.stop();
                    quickDrawScene.ClearScene(lcd);
                    NewText(1, "REACTION TIME", 0, 0); //fix
                    sceneManager.DisplaySceneByID(1);
                    thread_sleep_for(1000);
                    quickDrawScene.ClearScene(lcd);

                    reaction[i] = timer.elapsed(); //No penalty, just the reaction time of the player
                    
                }
            }
            if(timer.elapsed() >= 1000){
                timer.stop();
                NewText(1, "TOO LATE", 0, 0); //fix
                sceneManager.DisplaySceneByID(1);
                thread_sleep_for(1000);
                quickDrawScene.ClearScene(lcd);

                reaction[i] = 5000; // penalty for being too late
            }
            roundOver = false;
        }
        //Calculate average reaction time
        for(int i = 0; i < lengthOfGame; i++){
            average = average + reaction[i];
        }
        average = average/lengthOfGame;

        NewText(1, "YOUR REACTION", 0, 0);
        NewText(1, "TIME", 0, 1); //add in the average here with the fix
        sceneManager.DisplaySceneByID(1);
        thread_sleep_for(2000);
   
        
        NewText(1, "PLAY AGAIN", 0, 0);
        NewText(1, "B1 IS Y B4 IS N", 0, 1);
        sceneManager.DisplaySceneByID(1);
        playAgain = false;
        while(playAgain == false){
            if(B1){
                playAgain = true;
            }
            if(B4){
                gameOver = true;
                playAgain = true;
            }
        }
    } 
}

void quickDrawMultiplayerGameGuest(Scene &quickDrawScene){
    quickDrawScene.ClearScene(lcd);

    bool gameOver = false; //The function will return when this becomes true (at the end of the game)
    bool roundOver = false; //This checks if the player has been penalised and cannot continue to play during that particular round
    bool playAgain = false; //This offers the player if they want to play again
    bool progress = false;
    
    Timers timer; 
    
    int react = 0; //The amount of time until the prompt appears on screen
    int average = 0; //The average reaction time of the player calculated at the end of the game
    int hostAverage = 0;
    int lengthOfGame = 5; //This is the amount of rounds in a game, will be useful for programmability
    int reaction[lengthOfGame]; //Holds the individual reaction times of the player

    //Welcome message
    NewText(1, "MULTIPLAYER", 0, 0); 
    NewText(1, "QUICK DRAW", 0, 1);
    sceneManager.DisplaySceneByID(1);

    //Game begin
    while(gameOver == false){
        //Get a ping for progress here
        if(progress == true){
            progress = false;
            //Use a for loop to determine the amount of rounds
            for(int i = 0; i < lengthOfGame; i++){

                //Display what round it is and give a prompt to the user that it is starting
                quickDrawScene.ClearScene(lcd);
                NewText(1, "ROUND ", 0, 0); //fix
                sceneManager.DisplaySceneByID(1);
                thread_sleep_for(500);
                quickDrawScene.ClearScene(lcd);

                //Get this random number from the host not generated by the guest
                randLIB_seed_random();
                react = randLIB_get_8bit() * 15;
                timer.start();

                while(timer.elapsed() < react && roundOver == false){
                    if(B1 || B2 || B3 || B4){
                        timer.stop();
                        NewText(1, "TOO EARLY", 0, 0);
                        sceneManager.DisplaySceneByID(1);
                        thread_sleep_for(1000);
                        quickDrawScene.ClearScene(lcd);

                        reaction[i] = 5000; //penalty for being too early
                        roundOver = true;   
                    }
                }

                if(roundOver == false){
                    NewText(1, "REACT" , 0, 0); //Use backlight here
                    sceneManager.DisplaySceneByID(1);
                }

                while(timer.elapsed() >= react && timer.elapsed() < 5000 && roundOver == false){
                    if(B1 || B2 || B3 || B4){
                        timer.stop();
                        quickDrawScene.ClearScene(lcd);
                        NewText(1, "REACTION TIME", 0, 0); //fix
                        sceneManager.DisplaySceneByID(1);
                        thread_sleep_for(1000);
                        quickDrawScene.ClearScene(lcd);

                        reaction[i] = timer.elapsed(); //No penalty, just the reaction time of the player
                        
                    }
                }
                if(timer.elapsed() >= 1000){
                    timer.stop();
                    NewText(1, "TOO LATE", 0, 0); //fix
                    sceneManager.DisplaySceneByID(1);
                    thread_sleep_for(1000);
                    quickDrawScene.ClearScene(lcd);

                    reaction[i] = 5000; // penalty for being too late
                }
                roundOver = false;

                while(progress == false){
                    //get progress from host here
                    progress = true;
                }
            }
            //Calculate average reaction time
            for(int i = 0; i < lengthOfGame; i++){
                average = average + reaction[i];
            }
            average = average/lengthOfGame;

            NewText(1, "YOUR REACTION", 0, 0);
            NewText(1, "TIME", 0, 1); //add in the average here with the fix
            sceneManager.DisplaySceneByID(1);
            thread_sleep_for(2000);

            NewText(1, "HOST REACTION", 0, 0);
            NewText(1, "TIME", 0, 1); //add in the host average here with the fix, will need to get this from payload
            sceneManager.DisplaySceneByID(1);
            thread_sleep_for(2000);

            //Compare reaction time and assign a winner and loser
            //Need to get reaction time from host
            hostAverage = 500; //Get the function here to get the thing from the host
            if(average > hostAverage){
                quickDrawScene.ClearScene(lcd);
                NewText(1, "YOU LOSE", 0, 0);
                NewText(1, "", 0, 1); //add in the average here with the fix
                sceneManager.DisplaySceneByID(1);
                thread_sleep_for(2000);
            }
            else{
                quickDrawScene.ClearScene(lcd);
                NewText(1, "YOU WIN", 0, 0);
                NewText(1, "", 0, 1); //add in the average here with the fix
                sceneManager.DisplaySceneByID(1);
                thread_sleep_for(2000);
            }
            quickDrawScene.ClearScene(lcd);
        }
    }
}


int main(){

    lcd.init(); //initialise LCD 
    NewScene(0);
    sceneManager.DisplaySceneByID(0);


    Scene & baseScene = sceneManager.sceneArray[0]; //reference to scene being used


    //DaveSaysLoop(baseScene);

    
    //Singleplayer here
    NewScene(1);
    sceneManager.DisplaySceneByID(1);
    Scene & quickDrawScene = sceneManager.sceneArray[1];
    quickDrawSinglePlayerGame(quickDrawScene);
    
    //Multiplayer here
    /*
    NewScene(1);
    sceneManager.DisplaySceneByID(1);
    Scene & quickDrawScene = sceneManager.sceneArray[1];
    */
}

