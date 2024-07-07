from distutils.core import setup
#Attempting to know how to integrate the current sources to ROS
from catkin_pkg.python_setup import generate_distutils_setup
d = generate_distutils_setup(
    packages=['baseConfigs', 'baseStateMessage', 'BaseVideoService', 'CameraHttpServer', 'cameraServerController', 'configsJetBotUtils', 'Jetbot_Joystick', 'joystickUtills', 'listener', 'remoteControlClient', 'serialControl', 'serialPortUtills', 'serialPortUtills', 'talker'],
    package_dir={'': 'scripts'}
)
setup(**d)