
  #include <Stepper.h>
// Define number of steps per rotation:
const int stepsPerRevolution = 512;
// Wiring:
// Pin 8 to IN1 on the ULN2003 driver
// Pin 9 to IN2 on the ULN2003 driver
// Pin 10 to IN3 on the ULN2003 driver
// Pin 11 to IN4 on the ULN2003 driver
// Create stepper object called 'myStepper', note the pin order:
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
 void setup() {
  // Set the speed to 5 rpm:
  myStepper.setSpeed(30);
  Serial.begin(9600);         // open serial port at 9600 baud
}

void loop() 
{
  byte bytes;
  
  if(Serial.available()) 
    {    // if anything inputted into serial monitor.
     String c = Serial.readString(); //Reads serial monitor input as a line of String
     char myChar;
     int bin;
     int Binary[15]; 
     Serial.print(c);// Prints out message inputted into serial monitor
     
   
      if(c.charAt(0)=='L')
        {
          Serial.println("counterclockwise");
          myStepper.step(-stepsPerRevolution);
          delay(1000);
        }
        else if(c.charAt(0)=='R')
        {
          Serial.println("clockwise");
          myStepper.step(stepsPerRevolution);
          delay(1000);
        }  
        else if(c.charAt(0)=='H')
        {
          Serial.println("clockwise");
          myStepper.step(stepsPerRevolution/2);
          delay(1000);
        }
         else if(c.charAt(0)=='J')
        {
          Serial.println("counterclockwise");
          myStepper.step(-stepsPerRevolution/2);
          delay(1000);
        }           
     
     else
     {Serial.println("Starting");
                   myStepper.step(stepsPerRevolution);
                    delay(1000); 
     for(int i=0; i<c.length(); i++)
      {
        myChar = c.charAt(i); // Goes through each character of the string
        for(int i=7; i>=0; i--)
          {
            bytes = bitRead(myChar,i); //turns each character into a byte of information
            // Serial.print(bytes, BIN); //prints out each byte in forms of 8 bit binary
            bin = bytes;
            
            Binary[i]=bin;
            Serial.print(Binary[i]);
            
            if(Binary[i]==Binary[i+1])
                {
                    if(Binary[i]== 1)
                  {
                    Serial.println("Block");
                    myStepper.step(-stepsPerRevolution/2);
                    delay(5000); 
                     myStepper.step(stepsPerRevolution/2);
                    delay(5000);
                  }
                  if(Binary[i]== 0)
                  {
                     Serial.println("Block");
                     myStepper.step(stepsPerRevolution/2);
                    delay(5000);
                    myStepper.step(-stepsPerRevolution/2);
                    delay(5000); 
                  }
                }
            else if(Binary[i] != Binary[i+1] )
                {
                  if(Binary[i]== 0)
                  {
                    Serial.println("Counterclockwise");
                    myStepper.step(-stepsPerRevolution);
                    delay(5000); 
                  }
                  if(Binary[i]== 1)
                  {
                    Serial.println("clockwise");
                    myStepper.step(stepsPerRevolution);
                    delay(5000);
                    
                  }
                }
               
                 
           }
       
       
    }
     Serial.print("Ending");             
    } 
    }
 }
    

   
