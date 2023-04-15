"""
# Sample Python/Pygame Programs
# Simpson College Computer Science
# http://programarcadegames.com/
# http://simpson.edu/computer-science/
"""
 
import pygame
import socket
import time

# Definimos algunos colores
NEGRO = (0, 0, 0)
BLANCO = (255, 255, 255)

s = socket.socket()
#host = '10.6.12.128' # needs to be in quote
host = '192.168.1.141'
port = 8990 #1234
s.connect((host, port))
 
class TextPrint(object):
    '''
    Esta es una sencilla clase que nos ayudara a imprimir sobre la pantalla
    No tiene nada que ver con los joysticks, tan solo imprime informacion
    '''
    def __init__(self):
        """Constructor"""
        self.reset()
        self.x_pos = 10
        self.y_pos = 10
        self.font = pygame.font.Font(None, 20)
 
    def print(self, mi_pantalla, text_string):
        textBitmap = self.font.render(text_string, True, NEGRO)
        mi_pantalla.blit(textBitmap, [self.x, self.y])
        self.y += self.line_height
         
    def reset(self):
        self.x = 10
        self.y = 10
        self.line_height = 15
         
    def indent(self):
        self.x += 10
         
    def unindent(self):
        self.x -= 10
     
 
pygame.init()
  
# Establecemos el largo y alto de la pantalla [largo,alto]
dimensiones = [500, 700]
pantalla = pygame.display.set_mode(dimensiones)
 
pygame.display.set_caption("Mi Juego")
 
#Iteramos hasta que el usuario pulsa el boton de salir.
hecho = False
 
# Lo usamos para gestionar cuan rapido de refresca la pantalla.
reloj = pygame.time.Clock()
 
# Inicializa los joysticks
pygame.joystick.init()
     
# Se prepara para imprimir
text_print = TextPrint()
 
# -------- Bucle Principal del Programa -----------
while not hecho:
    # PROCESAMIENTO DEL EVENTO
    for evento in pygame.event.get(): 
        if evento.type == pygame.QUIT: 
            hecho = True
         
        # Acciones posibles del joystick: JOYAXISMOTION JOYBALLMOTION JOYBUTTONDOWN JOYBUTTONUP JOYHATMOTION
        if evento.type == pygame.JOYBUTTONDOWN:
            print("Boton presionado del joystick.")
        if evento.type == pygame.JOYBUTTONUP:
            print("Boton liberado del joystick.")
             
  
    # DIBUJAMOS
    # Primero, limpiamos la pantalla con color blanco. No pongas otros comandos de dibujo 
    # encima de esto, de lo contrario seran borrados por el comando siguiente.
    pantalla.fill(BLANCO)
    text_print.reset()
 
    # Contamos el numero de joysticks
    joystick_count = pygame.joystick.get_count()
 
    text_print.print(pantalla, "Numero de joysticks: {}".format(joystick_count) )
    text_print.indent()
     
    # Para cada joystick:
    for i in range(joystick_count):
        joystick = pygame.joystick.Joystick(i)
        joystick.init()
     
        text_print.print(pantalla, "Joystick {}".format(i) )
        text_print.indent()
     
        # Obtiene el nombre del Sistema Operativo del controlador/joystick
        nombre = joystick.get_name()
        text_print.print(pantalla, "Nombre del joystick: {}".format(nombre))
         
        # Habitualmente, los ejes van en pareja, arriba/abajo para uno, e izquierda/derecha
        # para el otro.
        ejes = joystick.get_numaxes()
        text_print.print(pantalla, "Numero de ejes: {}".format(ejes))
        text_print.indent()
         
        CMD = ''
        Dir1 = 'f'
        Dir2 = 'f'
        POWER_M1 = 0
        POWER_M2 = 0
         
        for i in range(ejes):
            eje = joystick.get_axis(i)
		    
            
            
            if(i == 1):
			    #valor del primer eje
                POWER_M2 = int(abs(eje*253))
                if(eje < 0):
                    #valor menor que 0
                    Dir2 = 'r'
                else:
                    #valor mayor que 0
                    Dir2 = 'f'
            
            if(i == 3):
                #valor del segundo eje
                POWER_M1 = int(abs(eje*253))
                if(eje < 0):
                    #valor menor que 0
                    Dir1 = 'r'
                else:
                    #valor mayor que 0
                    Dir1 = 'f'
            text_print.print(pantalla, "Eje {} valor: {:>6.3f}".format(i, eje))
        text_print.unindent()
         
        if(Dir1 == 'f' and Dir2 == 'f'):
            CMD = 'c' + ',' + str(POWER_M1)+','+str(POWER_M2)+'#'
            #print(CMD)
            s.send(CMD.encode("utf-8"))
        if(Dir1 == 'f' and Dir2 == 'r'):
            CMD = 'e' + ',' + str(POWER_M1)+','+str(POWER_M2)+'#'
            #print(CMD)
            s.send(CMD.encode("utf-8"))
        if(Dir1 == 'r' and Dir2 == 'f'):
            CMD = 'd' + ',' + str(POWER_M1)+','+str(POWER_M2)+'#'
            #print(CMD)
            s.send(CMD.encode("utf-8"))
        if(Dir1 == 'r' and Dir2 == 'r'):
            CMD = 'b' + ',' + str(POWER_M1)+','+str(POWER_M2)+'#'
            #print(CMD)
            s.send(CMD.encode("utf-8"))
         
        time.sleep(0.2)
         
        botones = joystick.get_numbuttons()
        text_print.print(pantalla, "Numero de botones: {}".format(botones))
        text_print.indent()
         
        for i in range(botones):
            boton = joystick.get_button(i)
            text_print.print(pantalla, "Boton {:>2} valor: {}".format(i,boton))
        text_print.unindent()
             
        # Hat switch. Todo o nada para la direccion, no como en los joysticks.
        # El valor vuelve en un array.
        hats = joystick.get_numhats()
        text_print.print(pantalla, "Numero de hats: {}".format(hats))
        text_print.indent()
 
        for i in range(hats):
            hat = joystick.get_hat(i)
            text_print.print(pantalla, "Hat {} valor: {}".format(i, str(hat)))
        text_print.unindent()
         
        text_print.unindent()
 
     
    # TODO EL CODIGO DE DIBUJO DEBERIA IR ENCIMA DE ESTE COMENTARIO
     
    # Avanzamos y actualizamos la pantalla con lo que hemos dibujado.
    pygame.display.flip()
 
    # Limitamos a 60 fotogramas por segundo.
    reloj.tick(60)
     
pygame.quit()
