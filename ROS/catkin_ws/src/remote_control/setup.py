from distutils.core import setup
#Attempting to know how to integrate the current sources to ROS
from catkin_pkg.python_setup import generate_distutils_setup
d = generate_distutils_setup(
    packages=['remote_control'],
    package_dir={'': 'src'}
)
setup(**d)