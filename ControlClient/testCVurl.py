import numpy as np
import urllib.request as request
import cv2


def url_to_image(url):
    resp = request.urlopen(url)
    #print(resp.read())
    #nparr = np.frombuffer(resp.read(), np.uint8)
    #np.frombuffer(f.read(), np.uint8), 1 # cv2.IMREAD_COLOR in OpenCV 3.1 
    image = cv2.imdecode(np.frombuffer(resp.read(), np.uint8), 1)
    return image


# Window name in which image is displayed
window_name = 'image'
  
# Using cv2.imshow() method
# Displaying the image
while True:
	cv2.imshow(window_name, url_to_image("http://192.168.1.71:9090/video_feed"))
	cv2.waitKey(1)
  
# closing all open windows
cv2.destroyAllWindows()