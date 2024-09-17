#include <LiquidCrystal.h>

// Define los pines según tu conexión
const int rs = 12;      // Sección de Registro (RS)
const int enable = 11;  // Activar (Enable)
const int d4 = 5;       // Pin DB4
const int d5 = 4;       // Pin DB5
const int d6 = 3;       // Pin DB6
const int d7 = 2;       // Pin DB7
int generador = A0;
int pulsador1 = 6;
int pulsador2 = 7;

//variable booleana para saber el estado de los pulsadores
bool mostrarDatos = false;

int *arreglo = nullptr;
int tamArreglo = 0;

//arreglo = new int[tamArreglo];

//guardarDatos(tamArreglo, arreglo);



// Inicializa el objeto lcd con los pines definidos
LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);

void setup() {
  pinMode(pulsador1, INPUT);
  pinMode(pulsador2, INPUT);

  lcd.begin(16, 2);          // Inicializa una pantalla LCD de 16x2
  lcd.print("Hola Mundo!");  // Muestra "Hola Mundo!" en la LCD
  Serial.begin(9600);
}

void loop() {

  // Lee los estados de los pulsadores
  int estadoPulsador1 = digitalRead(pulsador1);
  int estadoPulsador2 = digitalRead(pulsador2);

  //ACTIVAR Y DESACTIVAR VALOR BOOLEANO

  // Si el pulsador 1 es presionado, activa la bandera para mostrar datos
  if (estadoPulsador1 == HIGH) {
    mostrarDatos = true;
  }
	
  // Si el pulsador 2 es presionado, desactiva la bandera para dejar de mostrar datos
  else if (estadoPulsador2 == HIGH) {
    mostrarDatos = false;
  }


  int valorGenerador = analogRead(generador);


  //llamamos a la funcion guardarDatos
  arreglo = new int[tamArreglo];
  guardarDatos(tamArreglo, arreglo, valorGenerador);
  mostrar();

	
	
  
}

void guardarDatos(int &tamArreglo, int *&arreglo, int valorGenerador) {
    
	int capacidad = 10; // Inicializar capacidad
    int *nuevoArreglo = nullptr; // Inicializar el nuevo arreglo

    // Si arreglo es nulo, inicialízalo
    if (arreglo == nullptr) {
        arreglo = new int[capacidad];
        tamArreglo = 0; // Inicialmente el tamaño es 0
    }

    // Si el arreglo está lleno, expandimos la memoria
    if (tamArreglo >= capacidad) {
        // Aumentamos la capacidad
        capacidad *= 2;
        nuevoArreglo = new int[capacidad];

        // Copiamos los datos antiguos al nuevo arreglo
        for (int i = 0; i < tamArreglo; i++) {
            nuevoArreglo[i] = arreglo[i];
        }

        // Liberamos la memoria antigua
        delete[] arreglo;

        // Actualizamos el puntero al nuevo arreglo
        arreglo = nuevoArreglo;
    }

    // Guardamos el valor del generador en el arreglo
    arreglo[tamArreglo] = valorGenerador;
    tamArreglo++;

    // Continúa expandiendo si es necesario
    while (mostrarDatos) {
		Serial.println(valorGenerador);
        // No es necesario hacer nada dentro del while porque el ciclo de expansión ya está manejado arriba
    }

}
void mostrar() {
  lcd.clear();
  lcd.setCursor(0, 0);

  // Llamada a la función para calcular frecuencia
  //int frecuencia = calcularFrecuencia(arreglo, tamArreglo);

  // Llamada a la función para calcular la amplitud
  int amplitud = calcularAmplitud(arreglo, tamArreglo);

  // Llamada a la función para determinar el tipo de onda
  //String tipoOnda = identificarOnda(arreglo, tamArreglo);

  // Mostrar los resultados en la pantalla LCD
  lcd.print("Frecu: ");
  //lcd.print(frecuencia);
  lcd.setCursor(0, 1);
  lcd.print("Ampli: ");
  lcd.print(amplitud);

  // Muestra el tipo de onda en el Serial Monitor
  lcd.setCursor(1,0);
  lcd.print("Tipo de onda: ");
  //Serial.println(tipoOnda);
}

int calcularAmplitud(int *arreglo, int tamArreglo) {
  int valorMax = arreglo[0];
  int valorMin = arreglo[0];

  for (int i = 1; i < tamArreglo; i++) {
    if (arreglo[i] > valorMax) {
      valorMax = arreglo[i];
    }
    if (arreglo[i] < valorMin) {
      valorMin = arreglo[i];
    }
  }

  // Amplitud es la diferencia entre el valor máximo y el valor mínimo
  int amplitud = valorMax - valorMin;
  return amplitud;
}
