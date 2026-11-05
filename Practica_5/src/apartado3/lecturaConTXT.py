import serial

PUERTO_COM = 'COM3'   # Tu puerto COM
BAUDRATE = 115200   
NOMBRE_ARCHIVO = "datos_acelerometro.txt"

try:
    conexion = serial.Serial(PUERTO_COM, BAUDRATE, timeout=1)
    print(f"--- Conectado a {PUERTO_COM} ---")
    print(f"Guardando datos en '{NOMBRE_ARCHIVO}'. Presiona Ctrl+C para detener.\n")

    # Abrimos el fichero en modo escritura ('w')
    with open(NOMBRE_ARCHIVO, "w", encoding="utf-8") as archivo:
        # Cabecera para Excel
        archivo.write("X;Y;Z\n")
        archivo.flush()

        while True:
            if conexion.in_waiting > 0:
                linea = conexion.readline().decode('utf-8', errors='ignore').strip()
                
                if linea:
                    # Reemplaza espacios o comas por ';'
                    linea_csv = linea.replace(",", ";").replace(" ", ";")
                    
                    # Escribe la muestra con retorno de carro (\n)
                    archivo.write(linea_csv + "\n")
                    
                    # Forzar el guardado inmediato en disco
                    archivo.flush() 
                    
                    print(f"Guardado: {linea_csv}")

except KeyboardInterrupt:
    print("\n\nLectura finalizada por el usuario.")
except Exception as e:
    print(f"Error al acceder al puerto o fichero: {e}")
finally:
    if 'conexion' in locals() and conexion.is_open:
        conexion.close()
    print("Puerto cerrado y archivo guardado correctamente.")