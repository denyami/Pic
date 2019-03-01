#include "Arduino.h"
#include "Motor.h"

// コンストラクタ（初期化処理）
Motor::Motor(int pin1, int pin2, bool mode)
{
  m_MotorPin1 = pin1;             // メンバ変数にピン番号を代入
  m_MotorPin2 = pin2;
  m_mode = mode;
  F_st = 0;
  R_st = 0; 
  m_ac = 0;
  pinMode(m_MotorPin1, OUTPUT);  // ピンモードを指定
  pinMode(m_MotorPin2, OUTPUT);
}

void Motor::onF(void)
{
  if(m_mode == 0)
  {
    F_st = 1;
    R_st = 0;
    digitalWrite(m_MotorPin1, HIGH);
    digitalWrite(m_MotorPin2, LOW);
  }
  else if(m_mode == 1)
  {
    ac(1);
    analogWrite(m_MotorPin1, m_ac);
    analogWrite(m_MotorPin2, LOW);
  }
}

void Motor::onR(void)
{
  if(m_mode == 0)
  {
    F_st = 0;
    R_st = 1;
    digitalWrite(m_MotorPin1, LOW);
    digitalWrite(m_MotorPin2, HIGH);
  }
  else if(m_mode == 1)
  {
    ac(-1);
    analogWrite(m_MotorPin1, LOW);
    analogWrite(m_MotorPin2, -m_ac);
  }
}
void Motor::off(void)
{
  if(m_mode == 0)
  {
    F_st = 0;
    R_st = 0;
    digitalWrite(m_MotorPin1, LOW);
    digitalWrite(m_MotorPin2, LOW);
  }
  else if(m_mode == 1)
  {
    ac(0);
    analogWrite(m_MotorPin1, m_ac);
    analogWrite(m_MotorPin2, m_ac);
  }
}


int Motor::ac(int val){
  if(val > 0)
  {
    m_ac += val;
    if (m_ac > 255)
    {
      m_ac =255;
    }
  }
  else if(val < 0)
  {
    m_ac += val;
    if(m_ac < -255)
    {
      m_ac = -255;
    }
  }
  else
  {
    if(m_ac > 0)
    {
      m_ac--;
    }
    else if(m_ac < 0)
    {
      m_ac++;
    }
    else
    {
      m_ac = 0;
    }
  }
}


void Motor::print(void)
{
  if(m_mode == 1){
    p_val = m_ac;
    Serial.print(F("ac:["));
    Serial.print(p_val);
    Serial.print(F("] "));
  }
  else if(m_mode == 0)
  {
    Serial.print(F("F_st:["));
    Serial.print(F_st);
    Serial.print(F("]"));
    Serial.print(F("R_st:["));
    Serial.print(R_st);
    Serial.print(F("]"));
  }
}

