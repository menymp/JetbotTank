#!/usr/bin/env python

'''
converts the raw messages into usable oddometry data for the
jetbot joystick tank

tf only works with python 2
'''

import tf # <---- not supported in melodig for python 3

from nav_msgs.msg import Odometry
from geometry_msgs.msg import Point, Pose, Quaternion, Twist, Vector3
import time
import math
#ROS packages
import rospy
from std_msgs.msg import String

# Encoder characteristics
STEP_LENGTH = 0.01868
TRACK_DISTANCE = 0.2675

#Global positions
x_pos = 0.0
y_pos = 0.0
actualTheta = 0.0
last_time = rospy.Time.now()

odom_pub = rospy.Publisher("odom", Odometry, queue_size=50)
odom_broadcaster = tf.TransformBroadcaster() # <--- not supported in melodic for python 3

#moving all this into an external module
def handleIncomingData(data):
	#calculate oddometry
	print(data)

	tokens = data.split(':')

	#calculate differential oddometry
	dir1 = int(tokens[0])
	dir2 = int(tokens[1])
	steps1 = int(tokens[7])
	steps2 = int(tokens[8])

	current_time = rospy.Time.now() 
	dt = (current_time - last_time).toSec()

	currentEnc1Distance = steps1 * STEP_LENGTH
	currentEnc2Distance = steps2 * STEP_LENGTH

	if (dir1):
		currentEnc1Distance = -currentEnc1Distance
	else:
		currentEnc1Distance = currentEnc1Distance

	if (dir2):
		currentEnc2Distance = -currentEnc2Distance
	else:
		currentEnc2Distance = currentEnc2Distance

	Davg = float(currentEnc1Distance + currentEnc2Distance) / 2.0
	deltaTheta = float(currentEnc1Distance - currentEnc2Distance) / float(TRACK_DISTANCE)

	# Keep angle between -PI and PI  
	if (newTheta> math.pi):
		newTheta = newTheta - (2 * math.pi)
	if (newTheta < -math.pi):
		newTheta = newTheta + (2 * math.pi)

	deltaX = Davg * math.cos(newTheta) 
	deltaY = Davg * math.sin(newTheta)

	# calculate new positions with differences
	x_pos = x_pos + deltaX
	y_pos = y_pos + deltaY
	newTheta = actualTheta + deltaTheta

	# obtain XY speeds
	vx = deltaX / dt
	vy = deltaY / dt
	vth = deltaTheta / dt
	#convert to odom message and publish
	# since all odometry is 6DOF we'll need a quaternion created from yaw
	# tf is not supported by melodic, moved this to an isolated converter module
	
	odom_quat = tf.transformations.quaternion_from_euler(0, 0, newTheta)
	odom_broadcaster.sendTransform(
        (x_pos, y_pos, 0.),
        odom_quat,
        current_time,
        "base_link",
        "odom"
    )

    # next, we'll publish the odometry message over ROS
	odom = Odometry()
	odom.header.stamp = current_time
	odom.header.frame_id = "odom"

    # set the position
	odom.pose.pose = Pose(Point(x_pos, y_pos, 0.), Quaternion(*odom_quat))

    # set the velocity
	odom.child_frame_id = "base_link"
	odom.twist.twist = Twist(Vector3(vx, vy, 0), Vector3(0, 0, vth))

    # publish the message
	odom_pub.publish(odom)
	

	last_time = rospy.Time.now()
	pass

def listener():

    # In ROS, nodes are uniquely named. If two nodes with the same
    # name are launched, the previous one is kicked off. The
    # anonymous=True flag means that rospy will choose a unique
    # name for our 'listener' node so that multiple listeners can
    # run simultaneously.
    rospy.init_node('oddometry_node_transform', anonymous=True)

    rospy.Subscriber('/encoder/raw', String, handleIncomingData)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    listener()
