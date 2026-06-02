# AutoGreen 🌱
Sistema de Monitoreo y Control de Invernadero

**Universidad Mariano Gálvez de Guatemala**  
Facultad de Ingeniería en Sistemas de Información y Ciencias de la Computación  
Arquitectura de Computadoras I — Grupo 6

---

## Descripción
AutoGreen es un sistema embebido que monitorea y controla automáticamente las condiciones ambientales de un invernadero utilizando Arduino Uno como microcontrolador.

## Sensores
- **DHT22** — Temperatura y humedad ambiente (Pin D2)
- **Sensor capacitivo de suelo v1.2** — Humedad del suelo (Pin A0)
- **Módulo LDR** — Luminosidad (Pin A2)

## Actuadores
- **Ventilador 5V** — Activa cuando temperatura > 30°C (Pin D8)
- **Bomba peristáltica 12V** — Activa cuando humedad suelo < 40% (Pin D9)
- **LED Amarillo** — Calefactor simulado, activa cuando temperatura < 15°C (Pin D10)
- **Módulos LED 12V** — Luz auxiliar, activa cuando luminosidad < 200 lux (Pin D11)

## Librerías requeridas
- DHT sensor library (Adafruit)
- LiquidCrystal_I2C (Frank de Brabander)
- Wire (built-in Arduino)

## Instalación
1. Instalar Arduino IDE
2. Instalar las librerías desde Tools → Manage Libraries
3. Abrir `codigo/AutoGreen.ino`
4. Seleccionar placa Arduino Uno
5. Subir el código

## Visualización
- LCD 16x2 I2C (dirección 0x27) — Pines A4/A5
- Serial Monitor a 9600 baudios
