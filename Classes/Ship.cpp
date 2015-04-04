//
//  Ship.cpp
//  ShipDemo
//
//  Created by Walker White on 1/18/15.
//
#include "Ship.h"
#include "FilmStrip.h"
#include "math.h"
#include "ResourceLoader.h"
#include "Box2D/Box2D.h"

// Macros to eliminate magic numbers
// Some of these are carried over from the 3152 lab and unused.
#define ANGLE_RADIANS (PI/180.0f)
#define SHIP_MAX_TURN      1.0f
#define SHIP_MAX_SPEED    10.0f
#define SHIP_THRUST_FACTOR 0.4f
#define IMPULSE 2875

#define DCOS_90(a)  (cos(M_PI*(a+90.0f)/180.0f))
#define DSIN_90(a)  (sin(M_PI*(a+90.0f)/180.0f))

#define RANGE_CLAMP(x,y,z)  (x < y ? y : (x > z ? z : x))

/**
 * Creates a new ship at the specified position (in world space).
 *
 * @param x The world space x-coordinate
 * @param y The world space y-coordinate
 */
Ship::Ship(b2World *world, float x, float y, float mx, float my) {
   // position.set(x,y);
   // velocity.set(0.0f,0.0f);
    
   // angle   = 0.0f;
   // turning = 0.0f;
   // forward = 0.0f;
	boostFrames = 0;
	frameRate = 0;
	isDestroyed = false;
	hasWeapon = false;
	isActivatingEnvironment = false;
	currentWeapon = NULL;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(x, y);
	bodyDef.fixedRotation = true;
	body = world->CreateBody(&bodyDef);
	type = Type(this);
	body->SetUserData(&type);
	dynamicBox.SetAsBox(SHIP_WIDTH, SHIP_HEIGHT);
	fixture.shape = &dynamicBox;
	fixture.density = 0.1f;
	fixture.friction = 0.5f;
	body->CreateFixture(&fixture);
	body->SetLinearDamping(3.0f);
	//body->SetAngularDamping(0.5f);
    sprite  = NULL;
	//setSprite(FilmStrip::create(ResourceLoader::getInstance()->getTexture("ricky"), 1, 2, 2), mx, my);
	setSprite(FilmStrip::create(ResourceLoader::getInstance()->getTexture("ricky"), 5, 3, 13), mx, my);
	lastPosition = body->GetPosition();
}

/**
 * Disposes the ship, releasing all resources.
 */
Ship::~Ship() {
    // Release the film strip if we have a reference
    if (sprite != NULL) {
        sprite->release();
    }
    sprite = NULL;
}

/**
 * Sets the film strip representing this ship.
 *
 * Setting this to NULL clears the value
 *
 * @param value The ship film strip.
 */
void Ship::setSprite(FilmStrip* value, float mx, float my) {
    // Release the current sprite if we have a reference
    if (sprite != NULL) {
        sprite->release();
    }
    sprite = value;
	value->setPhysicsBody(0);
	sprite->setScale(0.8f);
    if (sprite != NULL) {
        sprite->retain(); // Do not delete it until we are done.
        sprite->setFrame(SHIP_IMG_FLAT);
        sprite->setPosition(Vec2(mx, my));
        sprite->setAnchorPoint(Vec2(0.5f,0.5f));
    }
}

/**
 * Updates the state of the model
 *
 * This method moves the ship forward, dampens the forces (if necessary)
 * and updates the sprite if it exists.
 *
 * @param deltaTime Time elapsed since last called.
 */
bool Ship::update(float deltaTime, Vec2 dir) {
	if (boostFrames <= 0) return false;
	boostFrames--;
	frameRate++;
	if (frameRate > 10000) {
		frameRate = 0;
	}
	//float32 angle = body->GetAngle();
	body->ApplyLinearImpulse(b2Vec2(dir.x * IMPULSE, dir.y * IMPULSE), body->GetPosition(), true);
	//body->ApplyForceToCenter(b2Vec2(thrust.y * 1000 * DCOS_90(angle)  * SHIP_THRUST_FACTOR, thrust.y * 1000 * (-DSIN_90(angle)) * SHIP_THRUST_FACTOR), true);
	//body->ApplyTorque(10*thrust.x, true);
	//body->ApplyAngularImpulse(4000.0f * thrust.x, true);

    // Adjust the active forces.
   // forward = RANGE_CLAMP(forward, -SHIP_MAX_SPEED, SHIP_MAX_SPEED);
   // turning = RANGE_CLAMP(turning, -SHIP_MAX_TURN, SHIP_MAX_TURN);
    if (sprite != NULL && hasWeapon) {
		sprite->setFrame(12);
    }
	else if(sprite!=NULL && !hasWeapon) {
		if (frameRate % FRAME_INTERVAL == 0) {
			Vec2* moveDir = new Vec2(round(body->GetPosition().x - lastPosition.x), round(body->GetPosition().y - lastPosition.y));
			advanceFrame(moveDir);
		}
	}
    
    // Process the ship thrust.
   /* if (forward != 0.0f) {
        // Thrust key pressed; increase the ship velocity.
        velocity.x = forward * DCOS_90(angle)  * SHIP_THRUST_FACTOR;
        velocity.y = forward * (-DSIN_90(angle)) * SHIP_THRUST_FACTOR;
    }
    
    // Move the ship, updating it.
    // Adjust the angle by the change in angle
    angle += turning;  // INVARIANT: -360 < ang < 720
    if (angle > 360) angle -= 360;
    if (angle < 0)   angle += 360;
    
    // Move the ship
    position += velocity;*/
	lastPosition = body->GetPosition();
	return true;
}

/**
 * Determines the next animation frame for the ship and applies it to the sprite.
 *
 * This method includes some dampening of the turn, and should be called before
 * moving the ship.
 */
void Ship::advanceFrame(Vec2* dir) {
    // Our animation depends on the current frame.
    unsigned int frame = sprite->getFrame();
    

	if (dir->y > 0) {
		if (frame == 0 || frame == 1) {
			frame++;
		}
		else {
			frame = 0;
		}
	}
	else if (dir->y < 0) {
		if (frame == 3 || frame == 4) {
			frame++;
		}
		else {
			frame = 3;
		}
	}
	else if (dir->x > 0) {
		if (frame == 6 || frame == 7) {
			frame++;
		}
		else {
			frame = 6;
		}
	}
	else if (dir->x < 0) {
		if (frame == 9 || frame == 10) {
			frame++;
		}
		else {
			frame = 9;
		}
	}
	else {
		frame = sprite->getFrame();
	}
	
    sprite->setFrame(frame);
}

