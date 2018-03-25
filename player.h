#include "stdinc.h"
#include "scenegraph.h"

float GRAVITY = 15.0;

class Player {

public:

    Vec3 position;

    float yAngle = 0.0;
    float vVel = 0.0;
    float height = 1.2;

    SceneGraph* graph;

    bool enableCollisions = true;
    bool isTouchingGround = false;

public:

    bool canMoveTo(Vec3 pos)
    {
        return (!graph->getObjectAtPosition(pos)) ;
    }

    // try to move forwards
    void moveForward(float amount) {
        Vec3 newPosition = position;
        newPosition.x += sin(yAngle) * amount;
        newPosition.z -= cos(yAngle) * amount;
        if (canMoveTo(newPosition)) {
            position = newPosition;
        }
    }

    void jump()
    {
        if (isTouchingGround) {
            vVel = 9.0;
        }
    }

    // try to pan
    void pan(float amount) {
        Vec3 newPosition = position;
        newPosition.x += cos(yAngle) * amount;
        newPosition.z += sin(yAngle) * amount;
        if (canMoveTo(newPosition)) {
            position = newPosition;
        }
    }

    void turn(float amount) {
        yAngle += amount;
    }

    void update(float elapsed) {

        // add gravity.
        vVel -= elapsed * GRAVITY;

        Vec3 newPosition = position;
        newPosition.y += vVel * elapsed;
        if (canMoveTo(newPosition)) {
            isTouchingGround = false;
            position = newPosition;
            
        } else {
            isTouchingGround = true;
            // reset velocity on collision.
            vVel = 0.0;
        }
    }

    Player(SceneGraph* graph) {
        position = Vec3(0,0,0);
        this->graph = graph;
    }

};