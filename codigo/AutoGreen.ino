// ============================================================
// AutoGreen - Código Final
// Universidad Mariano Gálvez de Guatemala
// Arquitectura de Computadoras I
// Grupo 6
// ============================================================

#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Pines sensores ---
#define PIN_DHT    2
#define PIN_SUELO  A0
#define PIN_LDR    A2
#define TIPO_DHT   DHT22

// --- Pines actuadores ---
#define PIN_VENTILADOR  8
#define PIN_BOMBA       9
#define PIN_CALEFACTOR  10
#define PIN_LUZ         11

// --- Umbrales de control (segun requerimientos del proyecto) ---
#define TEMP_MAX       30.0   // Activar ventilador si temp > 30C
#define TEMP_MIN       15.0   // Activar calefactor si temp < 15C
#define HUMEDAD_MIN    40     // Activar bomba si humedad suelo < 40%
#define HUMEDAD_MAX    60     // Apagar bomba si humedad suelo > 60% (histeresis)
#define LUZ_MIN        200    // Activar luz auxiliar si lux < 200

// --- Sensor DHT22 ---
DHT dht(PIN_DHT, TIPO_DHT);

// --- LCD I2C direccion 0x27, 16 columnas, 2 filas ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Variables globales ---
float temperatura;
float humedad_ambiente;
int   humedad_suelo;
float luz_lux;

// ============================================================
// SETUP: se ejecuta una sola vez al iniciar
// ============================================================
void setup() {
  Serial.begin(9600);
  dht.begin();

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("AutoGreen v1.0");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();

  // Configurar pines actuadores como salida
  pinMode(PIN_VENTILADOR, OUTPUT);
  pinMode(PIN_BOMBA,      OUTPUT);
  pinMode(PIN_CALEFACTOR, OUTPUT);
  pinMode(PIN_LUZ,        OUTPUT);

  // Reles Low Trigger: HIGH = apagado al inicio
  digitalWrite(PIN_VENTILADOR, HIGH);
  digitalWrite(PIN_BOMBA,      HIGH);
  digitalWrite(PIN_LUZ,        HIGH);
  // LED directo: LOW = apagado al inicio
  digitalWrite(PIN_CALEFACTOR, LOW);

  Serial.println("=== AutoGreen iniciando... ===");
}

// ============================================================
// LOOP: se ejecuta continuamente
// ============================================================
void loop() {
  // Apagar bomba y ventilador momentaneamente para lectura limpia
  // Evita ruido electrico en sensores analogicos
  digitalWrite(PIN_VENTILADOR, HIGH);
  digitalWrite(PIN_BOMBA,      HIGH);

  delay(200); // Espera que el ruido se disipe

  leerSensores();
  controlAutomatico();
  mostrarLCD();
  mostrarSerial();
  delay(1000);
}

// ============================================================
// Lee los 3 sensores y guarda valores en variables globales
// ============================================================
void leerSensores() {
  // DHT22: temperatura y humedad ambiente
  temperatura      = dht.readTemperature();
  humedad_ambiente = dht.readHumidity();

  if (isnan(temperatura) || isnan(humedad_ambiente)) {
    Serial.println("[ERROR] Fallo en lectura DHT22");
    temperatura      = 0;
    humedad_ambiente = 0;
  }

  // Sensor capacitivo de suelo: promedio de 50 lecturas para estabilizar
  // Tierra seca = RAW ~1014 = 0%, Agua = RAW ~242 = 100%
  long suma = 0;
  for (int i = 0; i < 50; i++) {
    suma += analogRead(PIN_SUELO);
    delay(5);
  }
  int raw_suelo = suma / 50;
  humedad_suelo = map(raw_suelo, 1014, 242, 0, 100);
  humedad_suelo = constrain(humedad_suelo, 0, 100);

  // LDR: promedio de 10 lecturas para estabilizar
  // Calibrado: oscuridad = RAW ~1014 = 0 lux, luz normal = RAW ~610 = 10000 lux
  int sumaLDR = 0;
  for (int i = 0; i < 10; i++) {
    sumaLDR += analogRead(PIN_LDR);
    delay(5);
  }
  int raw_ldr = sumaLDR / 10;
  luz_lux = map(raw_ldr, 1014, 610, 0, 10000);
  luz_lux = constrain(luz_lux, 0, 10000);
}

// ============================================================
// Logica de control automatico de actuadores
// Reles: Low Trigger (LOW = activado, HIGH = apagado)
// LED calefactor: logica directa (HIGH = activado, LOW = apagado)
// ============================================================
void controlAutomatico() {
  // Ventilador: ON si temperatura > 30C
  digitalWrite(PIN_VENTILADOR, temperatura > TEMP_MAX ? LOW : HIGH);

  // Bomba: ON si humedad suelo < 40%, OFF si > 60% (histeresis)
  if (humedad_suelo < HUMEDAD_MIN) {
    digitalWrite(PIN_BOMBA, LOW);   // Encender bomba
  } else if (humedad_suelo > HUMEDAD_MAX) {
    digitalWrite(PIN_BOMBA, HIGH);  // Apagar bomba
  }

  // Calefactor: ON si temperatura < 15C (LED directo, no rele)
  digitalWrite(PIN_CALEFACTOR, temperatura < TEMP_MIN ? HIGH : LOW);

  // Luz auxiliar: ON si luminosidad < 200 lux
  digitalWrite(PIN_LUZ, luz_lux < LUZ_MIN ? LOW : HIGH);
}

// ============================================================
// Muestra valores en pantalla LCD 16x2
// Fila 0: T:XX.XC H:XX%
// Fila 1: S:XX%  L:XXXX
// ============================================================
void mostrarLCD() {
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperatura, 1);
  lcd.print("C H:");
  lcd.print((int)humedad_ambiente);
  lcd.print("%  ");

  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(humedad_suelo);
  lcd.print("% L:");
  lcd.print((int)luz_lux);
  lcd.print("   ");
}

// ============================================================
// Muestra valores y estado de actuadores en Serial Monitor
// ============================================================
void mostrarSerial() {
  Serial.println("-----------------------------");
  Serial.print("Temperatura:      "); Serial.print(temperatura); Serial.println(" C");
  Serial.print("Humedad ambiente: "); Serial.print(humedad_ambiente); Serial.println(" %");
  Serial.print("Humedad suelo:    "); Serial.print(humedad_suelo); Serial.println(" %");
  Serial.print("Luminosidad:      "); Serial.print(luz_lux); Serial.println(" lux");
  Serial.println("--- Actuadores ---");
  Serial.print("Ventilador: "); Serial.println(temperatura > TEMP_MAX ? "ON" : "OFF");
  Serial.print("Bomba:      "); Serial.println(humedad_suelo < HUMEDAD_MIN ? "ON" : "OFF");
  Serial.print("Calefactor: "); Serial.println(temperatura < TEMP_MIN ? "ON" : "OFF");
  Serial.print("Luz:        "); Serial.println(luz_lux < LUZ_MIN ? "ON" : "OFF");
  Serial.println("-----------------------------");
}