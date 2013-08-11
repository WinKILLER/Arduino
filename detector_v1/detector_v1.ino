/* Пины, к которым подключен энкодер */
enum { ENC_PIN1 = 2, ENC_PIN2 = 3, LEFT_LED = 4, RIGHT_LED = 5};

enum { FORWARD = 1, BACKWARD = -1 };

/* Если что, revolutions здесь и далее - обороты, а не революции (: */
long revolutions = 0, revolutions_at_last_display = 0;
int direction = FORWARD;
uint8_t previous_code = 0;

/* Реакция на событие поворота */
void turned(int new_direction)
{
  if (new_direction != direction)
  {
    revolutions = 0;
    revolutions_at_last_display = 0;
  }
  else
    ++revolutions;

  direction = new_direction;
}

/* Объеденил чтение кода Грея с энкодера с его декодированием */
uint8_t readEncoder(uint8_t pin1, uint8_t pin2)
{
  uint8_t gray_code = digitalRead(pin1) | (digitalRead(pin2) << 1), result = 0;

  for (result = 0; gray_code; gray_code >>= 1)
    result ^= gray_code;

  return result;
}


void setup()
{
  pinMode(ENC_PIN1, INPUT);
  pinMode(ENC_PIN2, INPUT);
  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);

  Serial.begin(9600);
}

void loop()
{
  /* Читаем значение с энкодера */
  uint8_t code = readEncoder(ENC_PIN1, ENC_PIN2);

  /* Обрабатываем его */
  if (code == 0)
  {
    if (previous_code == 3)
      turned(FORWARD);
    else if (previous_code == 1)
      turned(BACKWARD);
  }

  previous_code = code;

  /* Раз в секунду выводим накопленную информацию */

  static unsigned long millis_at_last_display = 0;

  if (millis() - millis_at_last_display >= 1000)
  {
    /* Отображаем направление вращения */
    if(revolutions!=0){
      if(direction == FORWARD){
        digitalWrite(LEFT_LED,HIGH);
        digitalWrite(RIGHT_LED,LOW);
      }else{
        digitalWrite(LEFT_LED,LOW);
        digitalWrite(RIGHT_LED,HIGH);
      }
    }else{
        digitalWrite(LEFT_LED,LOW);
        digitalWrite(RIGHT_LED,LOW);
    }
    
    /* ... скорость вращения в оборотах в секунду */
    Serial.print("Field frequency: ");
    Serial.print(revolutions - revolutions_at_last_display);
    Serial.print(" | Cycles: ");
    /* ... и общее число обротов в текущем направлении */
    Serial.println(revolutions);
    
    
    millis_at_last_display = millis();
    revolutions_at_last_display = revolutions;
  }
}
