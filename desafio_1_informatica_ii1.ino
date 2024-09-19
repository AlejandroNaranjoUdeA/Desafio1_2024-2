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
    //analizarSegmento(segmento, tamanoSegmento, valorMedio);
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
bool esSenoidal(int *segmento, int tamanoSegmento, int valorMedio) {
    int umbral = calcularUmbral(segmento, tamanoSegmento);  // Calcular umbral dinámico
    float desfase = calcularDesfase(segmento, tamanoSegmento, valorMedio);  // Calcular el desfase

    // Si el desfase no es válido, no es senoidal
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

float calcularValorMedio(int *segmento, int tamanoSegmento) {
    long suma = 0;
    
    // Calcular la suma total de los valores del segmento
    for (int i = 0; i < tamanoSegmento; i++) {
        suma += segmento[i];
    }

    // El valor medio es la suma dividida por el número de muestras
    return (float)suma / tamanoSegmento;
}

// Función para calcular el umbral dinámico
int calcularUmbral(int *segmento, int tamanoSegmento) {
    int sumaDiferencias = 0;

    // Calcular el promedio de las diferencias entre puntos consecutivos
    for (int i = 1; i < tamanoSegmento; i++) {
        sumaDiferencias += abs(segmento[i] - segmento[i - 1]);
    }

    int promedioDiferencias = sumaDiferencias / (tamanoSegmento - 1);
    int umbral = promedioDiferencias * 1.5;  // Ajuste del umbral basado en el promedio
    return umbral;
}

// Función para calcular el desfase
float calcularDesfase(int *segmento, int tamanoSegmento, int valorMedio) {
    int primerCruce = -1;
    int segundoCruce = -1;

    // Encontrar los dos primeros cruces por el valor medio (aproximadamente 512)
    for (int i = 1; i < tamanoSegmento; i++) {
        if ((segmento[i - 1] < valorMedio && segmento[i] >= valorMedio) ||
            (segmento[i - 1] > valorMedio && segmento[i] <= valorMedio)) {
            if (primerCruce == -1) {
                primerCruce = i;
            } else if (segundoCruce == -1) {
                segundoCruce = i;
                break;  // Encontramos el segundo cruce, podemos salir del bucle
            }
        }
    }

    if (primerCruce != -1 && segundoCruce != -1) {
        // Calcular el desfase en términos del ciclo de la onda
        float periodo = (float)(segundoCruce - primerCruce);
        return periodo;  // Desfase en unidades de muestra
    } else {
        // Si no se encuentran cruces suficientes, el desfase no puede calcularse
        return -1.0;  // Valor que indica que no se encontró desfase
    }
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
void analizarSegmento(int *segmento, int tamanoSegmento, int valorMedio) {
    // Calcular si el segmento es senoidal
    if (esSenoidal(segmento, tamanoSegmento, valorMedio)) {
        Serial.println("El segmento es senoidal.");
    } else {
        Serial.println("El segmento no es senoidal.");
    }   
    
} 