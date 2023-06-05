import matplotlib.pyplot as plt

# Rutas de los archivos de datos
archivos = [
    "solapamiento_T_1E-1.txt",
    "solapamiento_T_1E-2.txt",
    "solapamiento_T_2E-2.txt",
    "solapamiento_T_3E-2.txt",
    "solapamiento_T_4E-2.txt",
    "solapamiento_T_5E-2.txt",
    "solapamiento_T_5E-3.txt",
    "solapamiento_T_1E-4.txt"
]

# Configuración de colores y etiquetas para cada archivo
colores = ['#FF0000', '#FF8400', '#BFFF00', '#00C624', '#00AFD1', '#0A00D1', '#A400D1', '#FF00A6']
etiquetas = ['1E-1', '1E-2', '2E-2', '3E-2', '4E-2', '5E-2', '5E-3', '1E-4']

# Crear la figura y los ejes
fig, ax = plt.subplots()

# Leer y trazar los datos de cada archivo
for archivo, color, etiqueta in zip(archivos, colores, etiquetas):
    # Leer los datos del archivo
    x, y = [], []
    with open(archivo, 'r') as f:
        for linea in f:
            valores = linea.strip().split()
            x.append(float(valores[0]))
            y.append(float(valores[1]))
    
    # Trazar los datos
    ax.plot(x, y, color=color, label=etiqueta)

# Configurar los ejes y la leyenda
ax.set_xlabel('Tiempo (PMC)')
ax.set_ylabel('Solapamiento')
ax.legend()

# Mostrar la figura
#plt.show()
plt.savefig('solapamiento_temperatura_deformado0.5.png', format='png', dpi=300, bbox_inches='tight')
