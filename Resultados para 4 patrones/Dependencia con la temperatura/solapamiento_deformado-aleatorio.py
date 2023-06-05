import matplotlib.pyplot as plt

# Función para leer los datos de un archivo de texto
def leer_datos(archivo):
    x = []
    y = []
    
    with open(archivo, 'r') as file:
        for line in file:
            data = line.strip().split()
            x.append(float(data[0]))
            y.append(float(data[1]))
    
    return x, y

# Archivos de solapamiento aleatorio
archivos_aleatorio = ['solapamiento_aleatorio_T_9E-2.txt', 'solapamiento_aleatorio_T_7E-2.txt', 'solapamiento_aleatorio_T_5E-2.txt', 'solapamiento_aleatorio_T_4E-3.txt', 'solapamiento_aleatorio_T_1E-3.txt', 'solapamiento_aleatorio_T_1E-4.txt']

# Archivos de solapamiento deformado
archivos_deformado = ['solapamiento_deformado_T_9E-2.txt', 'solapamiento_deformado_T_7E-2.txt', 'solapamiento_deformado_T_5E-2.txt', 'solapamiento_deformado_T_4E-3.txt', 'solapamiento_deformado_T_1E-3.txt', 'solapamiento_deformado_T_1E-4.txt']
etiquetas_deformado = {'solapamiento_deformado_T_9E-2.txt': '9E-2', 'solapamiento_deformado_T_7E-2.txt': '7E-2', 'solapamiento_deformado_T_5E-2.txt': '5E-2', 'solapamiento_deformado_T_4E-3.txt': '4E-3', 'solapamiento_deformado_T_1E-3.txt': '1E-3', 'solapamiento_deformado_T_1E-4.txt': '1E-4'}


# Crear la figura y las subfiguras
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 5))

# Graficar los datos de solapamiento aleatorio
for archivo in archivos_aleatorio:
    x_aleatorio, y_aleatorio = leer_datos(archivo)
    ax1.plot(x_aleatorio, y_aleatorio)

ax1.set_xlabel('Tiempo (PMC)')
ax1.set_ylabel('Solapamiento')
ax1.set_ylim([-0.12,1.12])
ax1.set_title('Configuración inicial: aleatoria', fontsize='10.5')


# Graficar los datos de solapamiento deformado
for archivo in archivos_deformado:
    x_deformado, y_deformado = leer_datos(archivo)
    etiqueta = etiquetas_deformado.get(archivo, archivo)  # Obtener la etiqueta correspondiente o usar el nombre de archivo si no hay etiqueta definida
    ax2.plot(x_deformado, y_deformado, label=etiqueta)

ax2.set_xlabel('Tiempo (PMC)')
#ax2.set_yticks([])
ax2.get_yaxis().set_ticklabels([])
ax2.set_ylim([-0.12,1.12])
ax2.set_title('Configuración inicial: patrón 1 deformado al 50%', fontsize='10.5')

# Añadir una leyenda
ax2.legend(loc='upper right')

# Ajustar los espacios entre subfiguras
plt.subplots_adjust(wspace=0.05)

# Mostrar la figura
#plt.show()

# Guardar la figura en formato PNG con una resolución de 300 ppp
plt.savefig('solapamiento_deformado-aleatorio.png', format='png', dpi=300, bbox_inches='tight')