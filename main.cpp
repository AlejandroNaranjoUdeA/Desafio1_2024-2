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

// Variable booleana para saber el estado de los pulsadores
bool mostrarDatos = false;

int *arreglo = nullptr;
int tamArreglo = 0;
int capacidad = 10;  // Inicializar capacidad del arreglo

// Inicializa el objeto lcd con los pines definidos
LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);

void setup() {
    pinMode(pulsador1, INPUT);
    pinMode(pulsador2, INPUT);

    lcd.begin(16, 2);          // Inicializa una pantalla LCD de 16x2
    lcd.print("Hola Mundo!");  // Muestra "Hola Mundo!" en la LCD
    Serial.begin(9600);

    // Inicializa el arreglo
    arreglo = new int[capacidad];
}

void loop() {
    // Lee los estados de los pulsadores
    int estadoPulsador1 = digitalRead(pulsador1);
    int estadoPulsador2 = digitalRead(pulsador2);

    // Activar y desactivar la bandera para mostrar datos
    if (estadoPulsador1 == HIGH) {
        mostrarDatos = true;

    } else if (estadoPulsador2 == HIGH) {
        mostrarDatos = false;
    }

    // Llamamos a la función para manejar el almacenamiento de datos
    manejarDatos();


    // Espera un poco para no llenar el buffer serial demasiado rápido
    delay(50);
    mostrar();

    // Llamamos a la funcion para manejar las senales
    analizarFuncion();
}

void manejarDatos() {
    if (mostrarDatos) {
        int valorGenerador = analogRead(generador);

        // Expandir el arreglo si es necesario
        if (tamArreglo >= capacidad) {
            capacidad *= 2;
            int *nuevoArreglo = new int[capacidad];

            // Copiar datos antiguos al nuevo arreglo
            for (int i = 0; i < tamArreglo; i++) {
                nuevoArreglo[i] = arreglo[i];
            }

            // Liberar la memoria antigua
            delete[] arreglo;

            // Actualizar el puntero al nuevo arreglo
            arreglo = nuevoArreglo;
        }

        // Guardar el valor del generador en el arreglo
        arreglo[tamArreglo] = valorGenerador;
        tamArreglo++;

        // Imprimir datos en el puerto serial
        Serial.println(valorGenerador);
    }
}


void cleanup() {
    // Liberar memoria al final del uso
    delete[] arreglo;
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
    //lcd.setCursor(1,0);
    //lcd.print("Tipo de onda: ");
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
//ahora necesitamos sabemos como es la senal, si es triangular, cuadrada o senosoidal

//para la funcion cuadrada, los cambios de valores son muy bruscos,
//mientras que para la triangular son mas suaves

void analizarFuncion() {
    if (mostrarDatos == false) {
        // Variables para identificar la naturaleza de la señal
        bool esCuadrada = true;
        bool esSenoidal = true;
        bool esTriangular = true;

        // Itera sobre los datos recolectados
        for (int i = 1; i < tamArreglo - 1; i++) {
            int diff1 = arreglo[i] - arreglo[i - 1];
            int diff2 = arreglo[i + 1] - arreglo[i];

            // Verifica si es cuadrada (cambios abruptos)
            if (abs(diff1) < 100 || abs(diff2) < 100) {
                esCuadrada = false;
            }

            // Verifica si es senoidal (cambios suaves y continuos)
            if (abs(diff1) > 50 || abs(diff2) > 50) {
                esSenoidal = false;
            }

            // Verifica si es triangular (cambios lineales)
            if (abs(diff1) != abs(diff2)) {
                esTriangular = false;
            }
        }

        // Mostrar el resultado en el puerto serial
        if (esCuadrada) {
            Serial.println("La senal es cuadrada.");
        } else if (esSenoidal) {
            Serial.println("La senal es senoidal.");
        } else if (esTriangular) {
            Serial.println("La senal es triangular.");
        } else {
            Serial.println("No se pudo determinar el tipo de senal.");
        }
    }
}
/*
frecuencia cuando es cuadrada hipotesis
int valorAnterior = 0;
int valorActual = 0;
unsigned long tiempoInicio = 0;
int contadorCambios = 0;
float frecuencia = 0;

void setup() {
  Serial.begin(9600);
  tiempoInicio = millis();  // Inicializa el tiempo
}

void loop() {
  // Leer el valor actual de la señal
  valorActual = analogRead(pinGenerador);

  // Detectar cambios de estado (para ondas cuadradas)
  if ((valorAnterior < 512 && valorActual >= 512) || (valorAnterior >= 512 && valorActual < 512)) {
    contadorCambios++;
  }

  // Cada segundo, calcular la frecuencia
  if (millis() - tiempoInicio >= 1000) {
    frecuencia = contadorCambios / 2.0;  // Cada cambio equivale a medio ciclo de la onda cuadrada
    Serial.print("Frecuencia de onda cuadrada (Hz): ");
    Serial.println(frecuencia);

    // Reiniciar variables para el siguiente segundo
    tiempoInicio = millis();
    contadorCambios = 0;
  }

  valorAnterior = valorActual;
}
*/
