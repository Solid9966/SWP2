#include <Servo.h>

// Arduino pin assignment

#define PIN_IR    A0        
#define PIN_LED   9
#define PIN_SERVO 10

#define _DUTY_MIN 0         
#define _DUTY_NEU 2000      
#define _DUTY_MAX 4000      

#define _DIST_MIN  100.0    
#define _DIST_MAX  250.0    

#define EMA_ALPHA  0.5      
#define LOOP_INTERVAL 20    // Loop Interval (20 msec)

Servo myservo;
unsigned long last_loop_time = 0;  // unit: msec

float dist_prev = _DIST_MIN;
float dist_ema = _DIST_MIN;

void setup() {
  pinMode(PIN_LED, OUTPUT);

  myservo.attach(PIN_SERVO); 
  myservo.writeMicroseconds(_DUTY_NEU);

  Serial.begin(1000000);    // 1,000,000 bps
}

void loop() {
  unsigned long time_curr = millis();
  int duty;
  float a_value, dist_raw;

  // wait until next event time
  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;
  last_loop_time += LOOP_INTERVAL;

  a_value = analogRead(PIN_IR);
  dist_raw = ((6762.0 / (a_value - 9.0)) - 4.0) * 10.0 - 60.0;

  // Range filter
  if (dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX) {
    digitalWrite(PIN_LED, HIGH);  // LED ON
    dist_ema = EMA_ALPHA * dist_raw + (1 - EMA_ALPHA) * dist_prev;  // EMA filter 적용
    dist_prev = dist_ema;
  } else {
    digitalWrite(PIN_LED, LOW);   // LED OFF
    dist_ema = dist_prev;         // 범위 밖이면 이전 거리 유지
  }

  // Map function replacement
  duty = _DUTY_MIN + ((dist_ema - _DIST_MIN) * (_DUTY_MAX - _DUTY_MIN)) / (_DIST_MAX - _DIST_MIN);
  
  myservo.writeMicroseconds(duty);

  Serial.print("_DUTY_MIN:");  Serial.print(_DUTY_MIN);
  Serial.print(",_DIST_MIN:"); Serial.print(_DIST_MIN);
  Serial.print(",IR:");        Serial.print(a_value);
  Serial.print(",dist_raw:");  Serial.print(dist_raw);
  Serial.print(",ema:");       Serial.print(dist_ema);
  Serial.print(",servo:");     Serial.print(duty);
  Serial.print(",_DIST_MAX:"); Serial.print(_DIST_MAX);
  Serial.print(",_DUTY_MAX:"); Serial.print(_DUTY_MAX);
  Serial.println("");
}
