#include <Arduino.h>
#include <math.h>

// Source temp: https://www.geekfactory.mx/tutoriales/tutoriales-arduino/termistor-ntc-con-arduino-como-sensor-de-temperatura/
// Source Motor: https://naylampmechatronics.com/blog/11_Tutorial-de-Uso-del-M%C3%B3dulo-L298N.html

// configurar el pin utilizado para la medicion de voltaje del divisor resistivo del NTC
#define CONFIG_THERMISTOR_ADC_PIN A0
// configurar el valor de la resistencia que va en serie con el termistor NTC en ohms
#define CONFIG_THERMISTOR_RESISTOR 10000l

// Pin control de motor 
int PinIN1 = 6;
int PinIN2 = 7;
// Pin de control de velocidad
int PinIN3 = A5;

/**
 * @brief Obtiene la resistencia del termistor resolviendo el divisor resistivo.
 * 
 * @param adcval Valor medido por el convertidor analógico a digital.
 * @return int32_t Resistencia electrica del termistor.
 */
int32_t thermistor_get_resistance(uint16_t adcval)
{
  // calculamos la resistencia del NTC a partir del valor del ADC
  return (CONFIG_THERMISTOR_RESISTOR * ((1023.0 / adcval) - 1));
}

/**
 * @brief Obtiene la temperatura en grados centigrados a partir de la resistencia
 * actual del componente.
 * 
 * @param resistance Resistencia actual del termistor.
 * @return float Temperatura en grados centigrados.
 */
float thermistor_get_temperature(int32_t resistance)
{
  // variable de almacenamiento temporal, evita realizar varias veces el calculo de log
  float temp;

  // calculamos logaritmo natural, se almacena en variable para varios calculos
  temp = log(resistance);

  // resolvemos la ecuacion de STEINHART-HART
  // http://en.wikipedia.org/wiki/Steinhart–Hart_equation
  temp = 1 / (0.001129148 + (0.000234125 * temp) + (0.0000000876741 * temp * temp * temp));

  // convertir el resultado de kelvin a centigrados y retornar
  return temp - 273.15;
}

void MotorHorario(int power)
{
  digitalWrite(PinIN1, HIGH);
  digitalWrite(PinIN2, LOW);
  analogWrite(PinIN3, power);
}
void MotorAntihorario(int power)
{
  digitalWrite(PinIN1, LOW);
  digitalWrite(PinIN2, HIGH);
  analogWrite(PinIN3, power);
}

void MotorStop()
{
  digitalWrite(PinIN1, LOW);
  digitalWrite(PinIN2, LOW);
  analogWrite(PinIN3, 0);
}

void setup()
{
  // preparar serial
  Serial.begin(9600);
  // configuramos los pines como salida
  pinMode(PinIN1, OUTPUT);
  pinMode(PinIN2, OUTPUT);
  pinMode(PinIN3, OUTPUT);
}

void loop()
{
  // variable para almacenar la temperatura y resistencia
  float temperatura;
  uint32_t resistencia;

  // calcular la resistencia electrica del termistor usando la lectura del ADC
  resistencia = thermistor_get_resistance(analogRead(CONFIG_THERMISTOR_ADC_PIN));
  // luego calcular la temperatura segun dicha resistencia
  temperatura = thermistor_get_temperature(resistencia);

  // imprimir resistencia y temperatura al monitor serial
  Serial.print(F("Resistencia del NTC: "));
  Serial.print(resistencia);
  Serial.print(" Temperatura: ");
  Serial.println(temperatura, 1);

  // esperar 5 segundos entre las lecturas
  delay(500);
  if (temperatura > 30.0) {
    MotorAntihorario(255);
  }
  else {
    MotorStop();
  }
}