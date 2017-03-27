void loop()
{
if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == 'a')
    {
      Timer1.pwm(SPEEDPIN, BASEDUTY);
      Timer1.pwm(SPEEDPIN, fastSpeed);
      delay(1000);
      Serial.println("DONE FAST");
      Timer1.pwm(SPEEDPIN, restingRate);
      delay(delayTime);
      Timer1.pwm(SPEEDPIN, 70);
      delay(1000);
      Serial.println("DONE SLOW");
      Timer1.pwm(SPEEDPIN, BASEDUTY);
    }
    else if (temp == '+'){
      delayTime = delayTime + 50;
    }
    else if (temp == '-'){
      delayTime = delayTime - 50;
    }
    else if (temp == 'u'){
      restingRate = restingRate + 2; 
    }
    else if (temp == 'd'){
      restingRate = restingRate - 2;
    }
    else if (temp == 'y'){
      fastSpeed = fastSpeed + 2;
    }
    else if (temp == 'x'){
      fastSpeed = fastSpeed -2;
    }
    Serial.println(delayTime);
    Serial.println(restingRate);
      
  }
}