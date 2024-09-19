#include <LiquidCrystal.h>

// Definir los pines según tu conexión
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

    lcd.begin(16, 2);          // Inicializa una pantalla LCD de 16x 
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

		lcd.clear();
		lcd.print("Tomando");
		lcd.setCursor(0, 1);
		lcd.print("datos");
		delay(5000);

    }
}

void cleanup() {
    // Liberar memoria al final del uso
    delete[] arreglo;
}

void mostrar() {
    lcd.clear();
    lcd.setCursor(1, 0);
    int amplitud = calcularAmplitud(arreglo, tamArreglo);
    float frecuensia= calcularFrecuencia(int *segmento, int tamanoSegmento, 5000);

    // Mostrar los resultados en la pantalla LCD
    lcd.print(" Amp: ");
    lcd.print(amplitud);

	lcd.print(" Fr: ");
  	lcd.print(frecuensia);

	

    // Identificar el tipo de onda
    almacenarSegmentoYCalcularUmbral();
    delay(2000);
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
    return valorMax - valorMin;
}

void almacenarSegmentoYCalcularUmbral() {
    int tamBloque = 20;  // Tamaño del bloque
    for (int i = 0; i < tamArreglo; i += tamBloque) {
        int finBloque = min(i + tamBloque, tamArreglo);  // Asegurarse de no pasar el límite del arreglo

        // Calcular la media (umbral) del bloque
        int suma = 0;
        for (int j = i; j < finBloque; j++) {
            suma += arreglo[j];
        }

        int umbral = suma / (finBloque - i);
        Serial.print("Umbral del bloque ");
        Serial.print(i / tamBloque);
        Serial.print(": ");
        Serial.println(umbral);

        // Comparar los valores del bloque con el umbral para identificar el tipo de señal
        identificarTipoSenal(i, finBloque, umbral);
    }
}

void identificarTipoSenal(int inicio, int fin, int umbral) {
    bool esCuadrada = false;

    // Variables para detectar cambios de pendiente lineal
    for (int i = inicio + 1; i < fin; i++) {
        int diff = arreglo[i] - arreglo[i - 1];

        // Si la diferencia es mayor al umbral, es cuadrada
        if (abs(diff) > umbral) {
            esCuadrada = true;
            lcd.clear();
            lcd.print("S. Cuadrada");
            delay(10000);
            return;
        }
    }

    // Si no es cuadrada, comprobar si es senoidal
    if (esSenoidal(arreglo + inicio, fin - inicio, umbral)) {
        lcd.clear();
        lcd.print("S. Senoidal");
        delay(5000);
        return;
    }

    // Si no es cuadrada ni senoidal, es triangular
    lcd.clear();
    lcd.print("S. Triangular");
    delay(10000);
    mostrar();
}

bool esSenoidal(int *segmento, int tamanoSegmento, int umbral) {
    float desfase = calcularDesfase(segmento, tamanoSegmento, umbral);

    if (desfase < 0) {
        return false;
    }

    // Revisar los cambios en el segmento para confirmar que sean suaves
    for (int i = 1; i < tamanoSegmento - 1; i++) {
        int diff1 = segmento[i] - segmento[i - 1];
        int diff2 = segmento[i + 1] - segmento[i];

        if (abs(diff1 - diff2) > umbral) {
            return false;  // No es senoidal si los cambios son bruscos
        }
    }

    // Si el desfase es consistente y los cambios son suaves, es senoidal
    return true;
}

float calcularDesfase(int *segmento, int tamanoSegmento, int valorMedio) {
    int primerCruce = -1;
    int segundoCruce = -1;

    for (int i = 1; i < tamanoSegmento; i++) {
        if ((segmento[i - 1] < valorMedio && segmento[i] >= valorMedio) ||
            (segmento[i - 1] > valorMedio && segmento[i] <= valorMedio)) {
            if (primerCruce == -1) {
                primerCruce = i;
            } else if (segundoCruce == -1) {
                segundoCruce = i;
                break;
            }
        }
    }

    if (primerCruce != -1 && segundoCruce != -1) {
        float periodo = (float)(segundoCruce - primerCruce);
        return periodo;
    } else {
        return -1.0;
    }
}

float calcularFrecuencia(int *segmento, int tamanoSegmento, float tiempoSegmento) {
    // Contar los cruces por el valor medio
    int cruces = contarCrucesPorValorMedio(segmento, tamanoSegmento);

    // Si no hay suficientes cruces, no se puede calcular la frecuencia
    if (cruces < 2) {
        return -1.0;  // Retornar un valor especial indicando que no se pudo calcular la frecuencia
    }

    // Dividimos los cruces por 2 para obtener el número de ciclos completos
    int ciclosCompletos = cruces / 2;

    // Calcular la frecuencia (número de ciclos por segundo)
    float frecuencia = ciclosCompletos / tiempoSegmento;

    return frecuencia;
}

int contarCrucesPorValorMedio(int *segmento, int tamanoSegmento) {
    float valorMedio = calcularValorMedio(segmento, tamanoSegmento);  // Calcular el valor medio del segmento
    int cruces = 0;  // Contador de cruces por el valor medio

    // Recorrer el segmento buscando cruces
    for (int i = 1; i < tamanoSegmento; i++) {
        // Detectar cruce por el valor medio
        if ((segmento[i - 1] < valorMedio && segmento[i] >= valorMedio) || 
            (segmento[i - 1] > valorMedio && segmento[i] <= valorMedio)) {
            cruces++;  // Incrementar el contador de cruces cuando ocurre un cruce
        }
    }

    return cruces;  // Retorna el número de cruces encontrados en el segmento
}
