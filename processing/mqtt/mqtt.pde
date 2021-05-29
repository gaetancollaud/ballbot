

import processing.serial.*;
import processing.opengl.*;
import toxi.geom.*;
import toxi.processing.*;
import mqtt.*;

// NOTE: requires ToxicLibs to be installed in order to run properly.
// 1. Download from http://toxiclibs.org/downloads
// 2. Extract into [userdir]/Processing/libraries
//    (location may be different on Mac/Linux)
// 3. Run and bask in awesomeness

MQTTClient client;
ToxiclibsSupport gfx;

Serial port;                         // The serial port
char[] teapotPacket = new char[14];  // InvenSense Teapot packet
int serialCount = 0;                 // current packet byte position
int synced = 0;
int interval = 0;

float[] q = new float[4];
Quaternion quat = new Quaternion(1, 0, 0, 0);

float[] gravity = new float[3];
float[] euler = new float[3];
float[] ypr = new float[3];

void setup() {
  
    client = new MQTTClient(this);
    client.connect("mqtt://ballbot:4EQzcQi4e1WAXC6Oi7l7@192.168.1.12:1884", "processing");
  
    // Must be squared !
    size(800, 800, OPENGL);
    gfx = new ToxiclibsSupport(this);

    // setup lights and antialiasing
    lights();
    smooth();
    
}

void clientConnected() {
  println("client connected");

  client.subscribe("ballbot/status/#");
}

void messageReceived(String topic, byte[] payload) {
  
  if(topic.equals("ballbot/status/angleX")){
    euler[0] = Float.valueOf(new String(payload));
  }else if(topic.equals("ballbot/status/angleY")){
    euler[2] = -Float.valueOf(new String(payload));
  }else if(topic.equals("ballbot/status/angleZ")){
    euler[1] = -Float.valueOf(new String(payload));
  }
  
  quat = Quaternion.createFromEuler(euler[0], euler[1], euler[2]);
  
}

int relativeSize(int size){
  return width / 300 * size;
}

void draw() {
    // black background
    background(0);
    
    // translate everything to the middle of the viewport
    pushMatrix();
    translate(width / 2, height / 2);

    // 3-step rotation from yaw/pitch/roll angles (gimbal lock!)
    // ...and other weirdness I haven't figured out yet
    //rotateY(-ypr[0]);
    //rotateZ(-ypr[1]);
    //rotateX(-ypr[2]);

    // toxiclibs direct angle/axis rotation from quaternion (NO gimbal lock!)
    // (axis order [1, 3, 2] and inversion [-1, +1, +1] is a consequence of
    // different coordinate system orientation assumptions between Processing
    // and InvenSense DMP)
    float[] axis = quat.toAxisAngle();
    rotate(axis[0], -axis[1], axis[3], axis[2]);

    // draw main body in red
    fill(255, 0, 0, 200);
    box(relativeSize(10), relativeSize(10), relativeSize(200));
    
    // draw front-facing tip in blue
    fill(0, 0, 255, 200);
    pushMatrix();
    translate(0, 0, relativeSize(-120));
    rotateX(PI/2);
    drawCylinder(relativeSize(0), relativeSize(20), relativeSize(20), relativeSize(8));
    popMatrix();
    
    // draw wings and tail fin in green
    fill(0, 255, 0, 200);
    beginShape(TRIANGLES);
    vertex(relativeSize(-100),  relativeSize(2), relativeSize(30)); vertex(relativeSize(0),  relativeSize(2), relativeSize(-80)); vertex(relativeSize(100),  relativeSize(2), relativeSize(30));  // wing top layer
    vertex(relativeSize(-100), relativeSize(-2), relativeSize(30)); vertex(relativeSize(0), relativeSize(-2), relativeSize(-80)); vertex(relativeSize(100), relativeSize(-2), relativeSize(30));  // wing bottom layer
    vertex(relativeSize(-2), relativeSize(0), relativeSize(98)); vertex(relativeSize(-2), relativeSize(-30), relativeSize(98)); vertex(relativeSize(-2), relativeSize(0), relativeSize(70));  // tail left layer
    vertex(relativeSize( 2), relativeSize(0), relativeSize(98)); vertex(relativeSize( 2), relativeSize(-30), relativeSize(98)); vertex(relativeSize( 2), relativeSize(0), relativeSize(70));  // tail right layer
    endShape();
    beginShape(QUADS);
    vertex(relativeSize(-100), relativeSize(2), relativeSize(30)); vertex(relativeSize(-100), relativeSize(-2), relativeSize(30)); vertex(relativeSize(  0), relativeSize(-2), relativeSize(-80)); vertex(relativeSize(0), relativeSize(2), relativeSize(-80));
    vertex( relativeSize(100), relativeSize(2), relativeSize(30)); vertex( relativeSize(100), relativeSize(-2), relativeSize(30)); vertex(  relativeSize(0), relativeSize(-2), relativeSize(-80)); vertex(relativeSize(0), relativeSize(2), relativeSize(-80));
    vertex(relativeSize(-100), relativeSize(2), relativeSize(30)); vertex(relativeSize(-100), relativeSize(-2), relativeSize(30)); vertex(relativeSize(100), relativeSize(-2), relativeSize(30)); vertex(relativeSize(100), relativeSize(2),  relativeSize(30));
    vertex(relativeSize(-2),   relativeSize(0), relativeSize(98)); vertex(relativeSize(2),   relativeSize(0), relativeSize(98)); vertex(relativeSize(2), relativeSize(-30), relativeSize(98)); vertex(relativeSize(-2), relativeSize(-30), relativeSize(98));
    vertex(relativeSize(-2),   relativeSize(0), relativeSize(98)); vertex(relativeSize(2),   relativeSize(0), relativeSize(98)); vertex(relativeSize(2),   relativeSize(0), relativeSize(70)); vertex(relativeSize(-2),   relativeSize(0), relativeSize(70));
    vertex(relativeSize(-2), relativeSize(-30), relativeSize(98)); vertex(relativeSize(2), relativeSize(-30), relativeSize(98)); vertex(relativeSize(2),   relativeSize(0), relativeSize(70)); vertex(relativeSize(-2),   relativeSize(0), relativeSize(70));
    endShape();
    
    popMatrix();
}

void drawCylinder(float topRadius, float bottomRadius, float tall, int sides) {
    float angle = 0;
    float angleIncrement = TWO_PI / sides;
    beginShape(QUAD_STRIP);
    for (int i = 0; i < sides + 1; ++i) {
        vertex(topRadius*cos(angle), 0, topRadius*sin(angle));
        vertex(bottomRadius*cos(angle), tall, bottomRadius*sin(angle));
        angle += angleIncrement;
    }
    endShape();
    
    // If it is not a cone, draw the circular top cap
    if (topRadius != 0) {
        angle = 0;
        beginShape(TRIANGLE_FAN);
        
        // Center point
        vertex(0, 0, 0);
        for (int i = 0; i < sides + 1; i++) {
            vertex(topRadius * cos(angle), 0, topRadius * sin(angle));
            angle += angleIncrement;
        }
        endShape();
    }
  
    // If it is not a cone, draw the circular bottom cap
    if (bottomRadius != 0) {
        angle = 0;
        beginShape(TRIANGLE_FAN);
    
        // Center point
        vertex(0, tall, 0);
        for (int i = 0; i < sides + 1; i++) {
            vertex(bottomRadius * cos(angle), tall, bottomRadius * sin(angle));
            angle += angleIncrement;
        }
        endShape();
    }
}
