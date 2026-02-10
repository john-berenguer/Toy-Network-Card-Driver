# Toy-Network-Card-Driver

Este proyecto es una implementación desde cero de un stack de protocolos de red funcional, desarrollado en C. El sistema interactúa directamente con la interfaz de red mediante **Raw Sockets**, permitiendo gestionar el tráfico de red sin depender del stack TCP/IP del kernel de Linux.

## 🚀 Características

El proyecto implementa los niveles fundamentales del modelo TCP/IP para servir una página web básica:

* **Capa de Enlace:** Gestión de tramas Ethernet II y abstracción de hardware (HAL).
* **Capa de Red:** * **IPv4:** Manejo de cabeceras, validación de checksum y filtrado de IP.
* **ARP:** Resolución de direcciones MAC y mantenimiento de tabla ARP estática.
* **ICMP:** Soporte de Echo Request/Reply (Ping).


* **Capa de Transporte:** * **TCP:** Máquina de estados mínima (Handshake de 3 vías) para permitir conexiones HTTP.
* **Capa de Aplicación:**
* **HTTP:** Servidor minimalista que responde "Hola mundo" ante peticiones en el puerto 80.



---

## 📂 Estructura del Proyecto

| Archivo | Descripción |
| --- | --- |
| `hal.c / .h` | Capa de Abstracción de Hardware. Usa `AF_PACKET` para leer la red. |
| `interface.c / .h` | Driver de la NIC. Gestiona hilos de RX/TX y callbacks. |
| `arp.c / .h` | Protocolo de resolución de direcciones MAC. |
| `ip.c / .h` | Implementación del protocolo IPv4. |
| `icmp.c / .h` | Manejo de mensajes de control (Ping). |
| `tcp.c / .h` | Gestión de flujo y estados de conexión TCP. |
| `http.c / .h` | Lógica de servidor web básico. |
| `main.c` | Punto de entrada, configuración de IP y despacho de paquetes. |

---

## 🛠️ Requisitos e Instalación

1. **Entorno:** Linux (se recomienda Ubuntu o Debian).
2. **Privilegios:** Es necesario ejecutar el binario como **root** (`sudo`) para abrir sockets raw.
3. **Interfaz:** El código está configurado por defecto para la interfaz `eth0`. Puedes cambiar esto en `hal.h`.

### Compilación

```bash
gcc -o webserver main.c hal.c interface.c arp.c ip.c icmp.c tcp.c http.c -lpthread

```

### Ejecución

```bash
sudo ./webserver

```

---

## 🔄 Flujo de Datos

Cuando llega un paquete al servidor, sigue esta ruta ascendente:

1. **Ethernet (`hal`)** -> Captura el frame bruto.
2. **Dispatcher (`main`)** -> Analiza el EtherType.
3. **IP (`ip`)** -> Verifica la dirección destino y el protocolo (TCP o ICMP).
4. **TCP (`tcp`)** -> Gestiona el handshake.
5. **HTTP (`http`)** -> Genera la respuesta "Hola mundo".

---

## ⚠️ Notas de Implementación

* **IP Estática:** El servidor utiliza por defecto la IP `192.168.56.100`.
* **Sin DHCP:** Para este ejercicio, se ha omitido la autoconfiguración por DHCP en favor de un diseño más ligero.
* **TCP Minimalista:** Se enfoca en el establecimiento de conexión y envío de datos, sin implementar control de congestión complejo o retransmisiones avanzadas.
