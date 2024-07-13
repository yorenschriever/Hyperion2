from math import pi, cos, sin
from numpy import matrix, array

class Turtle3d:

    position = array([0,0,0])
    # https://en.wikipedia.org/wiki/Rotation_matrix
    rotation = matrix([
        [1., 0,  0 ],
        [0,  1., 0 ],
        [0,  0,  1.]
    ])

    trail=[]
    
    def yaw(self,angle):
        alpha = angle * 2. * pi / 360
        yawRotation = matrix([
            [cos(alpha),    -sin(alpha),    0 ],
            [sin(alpha),    cos(alpha),     0 ],
            [0,             0,              1.]
        ])
        self.rotation  = yawRotation * self.rotation


    def pitch(self,angle):
        beta = angle * 2. * pi / 360
        pitchRotation = matrix([
            [cos(beta),    0,   sin(beta)   ],
            [0,            1.0, 0           ],
            [-sin(beta),   0,   cos(beta)   ]
        ])
        self.rotation  = pitchRotation * self.rotation

    def roll(self,angle):
        gamma = angle * 2. * pi / 360
        rollRotation = matrix([
            [1.,    0,          0           ],
            [0,     cos(gamma), -sin(gamma) ],
            [0,     sin(gamma), cos(gamma)  ]
        ])
        self.rotation  = rollRotation * self.rotation

    def move(self,distance, addPositionToTrail=True):
        delta = array([distance,0,0]) * self.rotation
        self.position = self.position + delta
        
        if addPositionToTrail:
            self.addPositionToTrail()

    def addPositionToTrail(self):
        self.trail.append({'x': self.position[0,0], 'y': self.position[0,1], 'z': self.position[0,2]})

    def clearTrail(self):
        self.trail=[]

    def setPosition(self, x, y, z):
        self.position = array([x,y,z])

    def setRotation(self, yaw, pitch, roll):
        self.rotation = matrix([
            [1., 0,  0],
            [0,  1., 0],
            [0,  0,  1.]
        ])
        self.yaw(yaw)
        self.pitch(pitch)
        self.roll(roll)