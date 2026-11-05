import serial

puerto = 'COM3'  # Sustituir por tu COM al conectarse
baudios = 115200

try:
    conexion = serial.Serial(puerto, baudios, timeout=1)
    print(f"--- Conectado a {puerto}. Imprimiendo datos en tiempo real ---")

    while True:
        if conexion.in_waiting > 0:
            # Lee la línea transmitida por el Nano
            linea = conexion.readline().decode('utf-8', errors='ignore').strip()
            if linea:
                print(f"Datos recibidos: {linea}")

except KeyboardInterrupt:
    print("\nPrograma detenido por el usuario.")
except Exception as e:
    print(f"Error al acceder al puerto: {e}")
finally:
    if 'conexion' in locals() and conexion.is_open:
        conexion.close()
        print("Puerto cerrado.")