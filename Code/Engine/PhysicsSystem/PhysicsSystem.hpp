#pragma once

#include "Engine/PhysicsSystem/CollisionDetection/BroadPhaseCollision.hpp"
#include "Engine/PhysicsSystem/CollisionDetection/NarrowPhaseCollision.hpp"

#include "Engine/PhysicsSystem/CollisionShape/CapsuleShape.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CircleShape.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"
#include "Engine/PhysicsSystem/CollisionShape/LineShape.hpp"
#include "Engine/PhysicsSystem/CollisionShape/PolygonShape.hpp"

#include "Engine/PhysicsSystem/ContactSolver/Contacts/Contact.hpp"
#include "Engine/PhysicsSystem/ContactSolver/ContactSolver.hpp"
#include "Engine/PhysicsSystem/ContactSolver/WorldContactHandler.hpp"

#include "Engine/PhysicsSystem/General/MathClasses.hpp"
#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"

#include "Engine/PhysicsSystem/PhysicsWorld/PhysicsWorld.hpp"
#include "Engine/PhysicsSystem/PhysicsWorld/SpacePartition.hpp"

#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"