


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#define s0 4
#define s1 5
#define s2 6
#define s3 7
#define tcsOut 8
#define ledPin13 13
#define ledPin12 12
#define ledPin11 11
#define ledPin10 10
#define sensorPin0 0
#define sensorPin1 1
#define sensorPin2 2


class Ball{
  public:
    int clr;
    int sze;
    int rampAngle(){
      /*
      if(clr==1 && sze==1) return 0;
      else if(clr==1 && sze==2) return 20;
      else if(clr==1 && sze==3) return 40;
      else if(clr==2 && sze==1) return 60;
      else if(clr==2 && sze==2) return 80;
      else if(clr==2 && sze==3) return 100;
      else if(clr==3 && sze==1) return 120;
      else if(clr==3 && sze==2) return 140;
      else if(clr==3 && sze==3) return 160;
      else return 180; */

      //only color 
      if(clr==1) return 45 ; 
      else if(clr==2)return 90 ; 
      else if(clr==3)return 135 ; 
      else return 180 ;
    }    
};

LiquidCrystal_I2C lcd(0x3F,16,2);

Servo servo1;
Servo servo2; 
Servo servo3; 
Servo servo4;

int stateNo;
int threshold = 20;
int frequency = 0;
Ball ourBall;


int color(){
  int r = 0;
  int g = 0;
  int b = 0;
  for(int i=0; i<50; i++){
    digitalWrite(s2,LOW);
    digitalWrite(s3,LOW);
    frequency = pulseIn(tcsOut,LOW);
    r+=frequency;
    delay(10);

    digitalWrite(s2,HIGH);
    digitalWrite(s3,HIGH);
    frequency = pulseIn(tcsOut,LOW);
    g+=frequency;
    delay(10);
    
    digitalWrite(s2,LOW);
    digitalWrite(s3,HIGH);
    frequency = pulseIn(tcsOut,LOW);
    b+=frequency;
    delay(10);
  }
  Serial.print("r : ") ; 
  Serial.print(r);
  Serial.print(" ");
   Serial.print("g : ") ; 
  Serial.print(g);
  Serial.print(" ");
   Serial.print("b : ") ; 
  Serial.print(b);
  Serial.print(" ");
  Serial.print("\n");
  Serial.print("ratio : ") ; 
  double x = (g*1.0)/b ; 
  Serial.print(x) ; 
  Serial.print("\n") ;

  if(r<b && r<g && r>0) return 1;
   
  if((g<r && g<b && g>0) || x<=1.15) return 2;
  if(b<g && b<r && b>0) return 3;
  return 0;
}

int sizes(int angle){
  if(angle>5 && angle<62){
    return 1;
  }
  else if(angle>=62 && angle<118){
    return 2;
  }
  else if(angle>=118 && angle<175){
    return 3;
  }
  return 0;
}

void printColorToDisplay(){
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("color:");
  
  
  ourBall.clr = color();
  lcd.setCursor(0,1);
  if(ourBall.clr==0) lcd.print("No color found");
  else if(ourBall.clr==1) lcd.print("Red");
  else if(ourBall.clr==2) lcd.print("Green");
  else if(ourBall.clr==3) lcd.print("Blue");
}


void setup() {
  // put your setup code here, to run once:
  stateNo = 1;
  ourBall.clr = 0;
  ourBall.sze = 0;

  servo1.attach(13);
  servo1.write(0);
  servo2.attach(12);
  servo2.write(0);
  servo3.attach(11);
  servo3.write(0);
  servo4.attach(10);
  servo4.write(0);

  lcd.begin();

  pinMode(3,OUTPUT);
  digitalWrite(3,LOW);
  pinMode(s0,OUTPUT);
  pinMode(s1,OUTPUT);
  pinMode(s2,OUTPUT);
  pinMode(s3,OUTPUT);
  pinMode(tcsOut,INPUT);
  digitalWrite(s0,HIGH);
  digitalWrite(s1,LOW);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(stateNo == 1){ // fetch another ball

  servo1.write(0);
  servo4.write(0);
      delay(500); 
      servo1.write(90);
      stateNo = 2;
    
  }
  else if(stateNo == 2){ // color sensing
        //stateNo = 1;
        delay(200);
        printColorToDisplay();
        delay(200);
        servo2.write(180);
        delay(500);
        servo1.write(180);
        delay(500);
        stateNo = 3 ; 
  }
  else if(stateNo == 3){ // level 1 releases, level 2 receives
        stateNo = 4 ; 
        delay(200) ; 
        servo1.write(0); 
        servo2.write(90);  
        servo3.write(0);     
  }
  else if(stateNo == 4){ // size measurement
    delay(200);
    int angle = 0;
    int oldval=200;
    for(int i=0; i<=180; i++){
      servo3.write(i);
      int val= analogRead(0);
      for(int j=0; j<1; j++){
        int val= analogRead(0);
        //Serial.print(val);
        //Serial.print("okay \n");  
          Serial.print("OUTPUT : ") ; 
          Serial.print(val) ; 
          Serial.print("\n") ;    
        if(oldval+oldval+oldval<val+val && val >=30){
          angle = i;
          j=100;
          i = 200;
          break;
        }
        oldval = val;  
      }
    }
    servo3.write(0);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(angle);
    ourBall.sze = sizes(angle);
    delay(500);
    stateNo = 5;
  }
  else if(stateNo == 5){ // level 1 releases, level 2 receives
        stateNo = 6 ; 
        servo4.write(0);
        delay(1000);
        servo4.write(ourBall.rampAngle());
        delay(200) ;      
        servo2.write(0);  
  }
  else if(stateNo == 6){
    digitalWrite(3,HIGH);
    delay(2000);
    digitalWrite(3,LOW);
    stateNo = 1;
  }
}
