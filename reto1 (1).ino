#include <Adafruit_LiquidCrystal.h>

#define PIN_BOTON_INICIO 2
#define PIN_BOTON_FINAL 3
#define PIN_GENERADOR A0

Adafruit_LiquidCrystal lcd(0);

bool capturando = false;

void setup() {
  pinMode(PIN_BOTON_INICIO, INPUT);
  pinMode(PIN_BOTON_FINAL, INPUT);
  pinMode(PIN_GENERADOR, INPUT);
  

}

void loop() {
  if (digitalRead(PIN_BOTON_INICIO) == HIGH) {
    capturando = true;
    lcd.clear();
    lcd.print("Capturando...");
    delay(500); // Para evitar rebote del botón
  }

  if (digitalRead(PIN_BOTON_FINAL) == HIGH) {
    capturando = false;
    lcd.clear();
    lcd.print("Pausado");
    delay(500); // Para evitar rebote del botón
  }

  if (capturando) {
    sense(); // Función de captura de la señal
  }
}

// Función para capturar los datos del generador
void sense() {
  int valorAnalogico = analogRead(PIN_GENERADOR); // Leer valor analógico de la señal
  
  // Suponemos que los valores de la señal están mapeados para un rango de amplitud y frecuencia
  float amplitud = map(valorAnalogico, 0, 1023, 0, 5); // Convertir a voltios
  float frecuencia = calcularFrecuencia(); // Función hipotética para calcular la frecuencia

  // Determinar el tipo de onda basado en la señal leída (se puede ajustar este método)
  String tipoOnda = identificarOnda(valorAnalogico);
  
  // Actualizar el LCD con la información
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Frecuencia: ");
  lcd.print(frecuencia);
  lcd.setCursor(0, 1);
  lcd.print("Amplitud: ");
  lcd.print(amplitud);
  delay(1000); // Actualizar cada segundo
}

float calcularFrecuencia() {
  // Implementar un método para calcular la frecuencia de la señal
  // Ejemplo básico
  return 60.0; // Valor estático temporal
}

String identificarOnda(int valor) {
  // Implementar lógica para identificar si es senoidal, cuadrada, etc.
  // Este es un ejemplo básico para devolver una onda genérica
  return "Senoidal";
}
