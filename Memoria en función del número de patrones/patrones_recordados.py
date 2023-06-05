import matplotlib.pyplot as plt

# Leer datos del archivo
x = []
y = []
error_x = []
error_y = []
with open('patrones_recordados_promedio.txt', 'r') as archivo:
    for linea in archivo:
        datos = linea.split('	')
        x.append(float(datos[0]))
        y.append(float(datos[1]))
        error_y.append(float(datos[2]))

# Representar los datos simulados
plt.errorbar(x, y, yerr=error_y, color='red', ecolor='salmon', linestyle=' ', marker='.')

# Nombres de los ejes
plt.xlabel('Número de patrones almacenados')
plt.ylabel('Número de patrones recordados')

# Mostrar y guardar la gráfica
#plt.show()
plt.savefig('patrones_recordados.png', dpi=300, bbox_inches='tight')