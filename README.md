# RoboticTEC 🤖

**Proyecto Final – Curso Principios de Sistemas Operativos, I Semestre 2025**  
**Instituto Tecnológico de Costa Rica**

---

## Descripción

RoboticTEC es un simulador de sistema distribuido desarrollado en C que simula procesamiento paralelo para identificar la palabra más repetida en un archivo de texto y, a continuación, controla físicamente una mano robótica conectada por USB (Arduino UNO clónico) para “escribir” esa palabra. El proyecto integra:

- Arquitectura cliente-servidor con sockets TCP/IP.  
- Simulación de procesamiento paralelo mediante tres nodos independientes (workers).  
- Cifrado simétrico (XChaCha20-Poly1305) y generación de claves con Diffie–Hellman (libsodium).  
- Driver de kernel personalizado para comunicación USB bulk con Arduino.  
- Biblioteca de espacio de usuario (UART) para transmitir caracteres al microcontrolador.

---

## 👥 Autores

- Josué Granados Chacón  
- Luis Alfredo González Sánchez  
- Mariana Rojas Rojas  
- Andrés Molina Redondo  

---

## 📖 Documentación (Wiki)

Para ver el detalle técnico de cada módulo, consulta la [Wiki del Proyecto](https://github.com/<USUARIO>/<REPOSITORIO>/wiki):

1. 📘 **[Introducción](Introduccion)**  
2. 🧠 **[Fundamento teórico](Introduccion)**  
3. 🗂️ **[Diseño del sistema](Diseño-del-sistema)**  
4. 💻 **[Ambiente de desarrollo](Ambiente-de-desarrollo)**  
5. 🛠️ **[Compilación y ejecución](Compilación-y-ejecución)**  
6. ✅ **[Conclusiones](Conclusiones)**  

---

## 🚀 Cómo empezar

1. **Clonar el repositorio**  
    
        git clone https://github.com/<USUARIO>/<REPOSITORIO>.git  
        cd <REPOSITORIO>  

2. **Instalar dependencias**  
   - Ubuntu 24.04 LTS  
   - gcc (C99+), make, libsodium  
   - Arduino IDE (para cargar firmware)  

3. **Compilar el driver y la biblioteca**  
    
        cd Driver-Biblioteca/  
        make  
        sudo insmod usb_driver_arduino.ko  
        sudo chmod 666 /dev/proyecto_arduino_usb  

4. **Ejecutar nodos, servidor y cliente**  
   - Nodos (puertos 9001–9003):  
    
         cd Nodos/  
         ./nodo 9001 & ./nodo 9002 & ./nodo 9003 &  

   - Servidor (puerto 9000):  
    
         cd Servidor/  
         ./servidor &  

   - Cliente (archivo de entrada):  
    
         cd Cliente/  
         ./cliente ../test_input  

5. **Verificar**  
    
        netstat -ln | grep ':900'  

   Debes ver `:9000`, `:9001`, `:9002` y `:9003` en estado LISTEN.

---

## 🔍 Resumen de características

- **Distribución de carga** entre procesos concurrentes.  
- **Comunicación segura** con cifrado autenticado.  
- **Control de hardware** desde Linux mediante driver y UART.  
- **Modularidad**: capas separadas de cliente, servidor, nodos, driver y firmware de Arduino.  

--- 
