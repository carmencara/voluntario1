import matplotlib.pyplot as plt
from mpl_toolkits.axes_grid1.inset_locator import zoomed_inset_axes, mark_inset

# Rutas de los archivos de datos
archivos = [
    "solapamiento_1patron_aleatorio - valor absoluto.txt",
    "solapamiento_1patron_deformado0.2.txt",
    "solapamiento_1patron_deformado0.5.txt",
    "solapamiento_1patron_deformado0.8.txt"
]

# Configuración de colores y etiquetas para cada archivo
colores = ['red', 'blue', 'green', 'orange']
etiquetas = ['Aleatorio', 'Deformado 20%', 'Deformado 50%', 'Deformado 80%']

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

# Crear un eje de zoom
ax_zoom = zoomed_inset_axes(ax, zoom=2, loc='center')

# Trazar la región de zoom
for archivo, color in zip(archivos, colores):
    # Leer los datos del archivo
    x, y = [], []
    with open(archivo, 'r') as f:
        for linea in f:
            valores = linea.strip().split()
            x.append(float(valores[0]))
            y.append(float(valores[1]))
    
    # Trazar los datos en el eje de zoom
    ax_zoom.plot(x, y, color=color)

# Configurar los ejes del zoom
x1, x2 = 0, 8  # Definir los límites en el eje x para el zoom
y1, y2 = 0.85, 1.02  # Definir los límites en el eje y para el zoom
ax_zoom.set_xlim(x1, x2)
ax_zoom.set_ylim(y1, y2)

# Dibujar rectángulo de zoom en la gráfica principal
mark_inset(ax, ax_zoom, loc1=2, loc2=3, fc="none", ec="0.5")

# Mostrar y guardar la figura
#plt.show()
plt.savefig('solapamiento_1patron.png', format='png', dpi=300, bbox_inches='tight')
