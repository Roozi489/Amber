#include "GamePlaySystem.h"
#include "Globals.h"
#include "TransformComponent.h"
#include "PhysicsComponent.h"
#include "Plane.h"
#include "Sphere.h"

GameplaySystem::GameplaySystem()
{
}

GameplaySystem::~GameplaySystem()
{
}

void GameplaySystem::configure()
{
    soundSystem = gWorld.getSystem<SoundSystem>();
}

void GameplaySystem::update(float delta)
{
    if (gKeystate[SDL_SCANCODE_F1])
    {
        cameraFollowBall = false;
        gCamera.setPosition(Vector3f(0.f, 170.f, -120.f));
        gCamera.setViewTarget(Vector3f(0.f, -0.1f, 0.f));
    }
    if (gKeystate[SDL_SCANCODE_F2])
    {
        cameraFollowBall = false;
        gCamera.setPosition(Vector3f(0.f, 250.f, -0.1f));
        gCamera.setViewTarget(Vector3f(0.f, -0.1f, 0.f));
    }
    if (gKeystate[SDL_SCANCODE_F3])
    {
        cameraFollowBall = true;
    }

    if (gameState == GameState::Defeat || gameState == GameState::Victory)
        return;

    const uint8_t leftDown = gKeystate[SDL_SCANCODE_LEFT];
    const uint8_t rightDown = gKeystate[SDL_SCANCODE_RIGHT];

    int brickCount = 0;

    if (gameState == GameState::Start && gKeystate[SDL_SCANCODE_SPACE])
    {
        for (Entity& entity : gWorld.entityManager.entities_with_components<TransformComponent, PhysicsComponent>())
        {
            TransformComponent& transformComp = entity.getComponent<TransformComponent>();
            PhysicsComponent& physicsComp = entity.getComponent<PhysicsComponent>();
            if (entity.tag == Tag::Ball)
            {
                physicsComp.velocity = normalize(Vector3f(-transformComp.position.x, 0.f, -transformComp.position.z)) * startingSpeed;
            }
        }
        gameState = GameState::Playing;
    }

    for (Entity& entity : gWorld.entityManager.entities_with_components<TransformComponent, PhysicsComponent>())
    {
        TransformComponent& transformComp = entity.getComponent<TransformComponent>();
        PhysicsComponent& physicsComp = entity.getComponent<PhysicsComponent>();

        if (physicsComp.bodyType != BodyType::Static)
        {
            bool moveFreely = true;

            if (entity.tag == Tag::Brick)
            {
                brickCount++;
                // Falling bricks
                if (physicsComp.velocity.y < 0.f)
                {
                    if (transformComp.position.y <= 0.f)
                    {
                        for (Entity& otherEntity : gWorld.entityManager.entities_with_components<TransformComponent, PhysicsComponent>())
                        {
                            TransformComponent& otherTransformComp = otherEntity.getComponent<TransformComponent>();
                            PhysicsComponent& otherPhysicsComp = otherEntity.getComponent<PhysicsComponent>();

                            if (entity != otherEntity && otherTransformComp.position.x == transformComp.position.x && otherTransformComp.position.z == transformComp.position.z)
                            {
                                otherPhysicsComp.velocity = Vector3f::zero();
                            }
                        }
                        transformComp.position.y = 0.f;
                    }
                }
            }

            // Move with pad
            if (entity.tag == Tag::Pad)
            {
                float angle = angleFromCenter(transformComp.position);
                if (leftDown)
                {
                    angle -= 1.5f * delta;
                    transformComp.position = Vector3f(sinf(angle) * 25, 0.f, cosf(angle) * 25);
                    transformComp.angle = pi + angle;
                }
                if (rightDown)
                {
                    angle += 1.5f * delta;
                    transformComp.position = Vector3f(sinf(angle) * 25, 0.f, cosf(angle) * 25);
                    transformComp.angle = pi + angle;
                }

                // If pad moved check for collision with ball
                if (leftDown || rightDown)
                {
                    for (Entity& otherEntity : gWorld.entityManager.entities_with_components<TransformComponent, PhysicsComponent>())
                    {
                        if (otherEntity.tag == Tag::Ball)
                        {
                            TransformComponent& ballTransformComp = otherEntity.getComponent<TransformComponent>();
                            PhysicsComponent& ballPhysicsComp = otherEntity.getComponent<PhysicsComponent>();

                            Sphere sphere(ballTransformComp.position, 1.f);

                            CollisionResult result = sphereMeshCollisionFast(sphere, transformComp, physicsComp.collisionMesh);
                            if (result.collisionOccured)
                            {
                                soundSystem->playSound("Bleep");
                                Vector3f tmp = sphere.origin - result.collisionPoint;
                                Vector3f moveVector = normalize(Vector3f(tmp.x, 0.f, tmp.z));
                                Vector3f resultNormal = normalize(Vector3f(result.collisionNormal.x, 0.f, result.collisionNormal.z));
                                float speed = ballPhysicsComp.velocity.length() * speedIncrease;
                                Vector3f vectorToCenter = normalize(Vector3f(-sphere.origin.x, 0.f, -sphere.origin.z));
                                float angle = dot(moveVector, vectorToCenter);
                                // If ball is on the side on the pad
                                if (angle < cosf(toRadians(45.f)))
                                {
                                    // Give a lot speed to simulate pad giving the ball speed
                                    speed = 40.f;
                                }

                                ballPhysicsComp.velocity = moveVector * speed;
                                // Move out of the pad
                                while (result.collisionOccured)
                                {
                                    ballTransformComp.position += moveVector * 0.1f;
                                    sphere.origin = ballTransformComp.position;
                                    result = sphereMeshCollisionFast(sphere, transformComp, physicsComp.collisionMesh);
                                }
                            }
                        }
                    }
                }
            }

            if (entity.tag == Tag::Ball)
            {
                if (cameraFollowBall)
                {
                    gCamera.setPosition(transformComp.position + Vector3f(0.f, 30.f, -0.1f) - physicsComp.velocity * 3.f);
                    gCamera.setViewTarget(transformComp.position);
                }

                Sphere sphere(transformComp.position, 1.f);
                std::vector<Vector3f> destroyedBricksPosition;

                // Bounce ball from the edge
                float ballSize = 1.f;
                float ballDistFromCenter = distFromCenter(transformComp.position);
                if (ballDistFromCenter + ballSize >= 30.f)
                {
                    if (lives <= 0)
                    {
                        gameState = GameState::Defeat;
                        soundSystem->playSound("defeat");
                        return;
                    }
                    else
                    {
                        lives--;
                    }

                    soundSystem->playSound("Bonk");
                    transformComp.position -= physicsComp.velocity * delta;
                    Vector3f norm = normalize(Vector3f(transformComp.position.x, 0.f, transformComp.position.z));
                    physicsComp.velocity = normalize(reflect(physicsComp.velocity, norm)) * startingSpeed;
                    moveFreely = false;
                }

                bool movedToPad = false;
                Vector3f resultNormal;
                for (Entity& otherEntity : gWorld.entityManager.entities_with_components<TransformComponent, PhysicsComponent>())
                {
                    TransformComponent& otherTransformComp = otherEntity.getComponent<TransformComponent>();
                    PhysicsComponent& otherPhysicsComp = otherEntity.getComponent<PhysicsComponent>();

                    if (otherEntity.tag == Tag::Brick)
                    {
                        // Bounce from brick
                        if (distance(sphere.origin, otherTransformComp.position) <= 2 * otherPhysicsComp.collisionMesh->boundingSphereRadiusFast() + sphere.radius)
                        {
                            // TODO: check for collisions with all objects and respond to the first one, then repeat the collision check
                            CollisionResult result = movingSphereMeshCollision(sphere, physicsComp.velocity * delta, otherTransformComp, otherPhysicsComp.collisionMesh);
                            if (result.collisionOccured)
                            {
                                transformComp.position += physicsComp.velocity * delta * result.collisionTime;

                                resultNormal = normalize(Vector3f(result.collisionNormal.x, 0.f, result.collisionNormal.z));
                                physicsComp.velocity = reflect(physicsComp.velocity, resultNormal);

                                // Don't add the remaining velocity since the ball might end up in some object
                                //transformComp.position += physicsComp.velocity * delta * (1.f - result.collisionTime);

                                score += 100;
                                soundSystem->playSound("Blop");
                                moveFreely = false;
                                destroyedBricksPosition.push_back(otherTransformComp.position);
                                gWorld.entityManager.destroyEntity(&otherEntity);
                                break;
                            }
                        }
                    }
                    else if (otherEntity.tag == Tag::Pad)
                    {
                        // Move ball with pad when in start state
                        if (gameState == GameState::Start && !movedToPad)
                        {
                            transformComp.position.x = otherTransformComp.position.x - normalize(otherTransformComp.position).x * 1.9f;
                            transformComp.position.z = otherTransformComp.position.z - normalize(otherTransformComp.position).z * 1.9f;
                            movedToPad = true;
                        }

                        // Bounce from pad
                        if (distance(sphere.origin, otherTransformComp.position) <= otherPhysicsComp.collisionMesh->boundingSphereRadiusFast() + sphere.radius)
                        {
                            CollisionResult result = movingSphereMeshCollision(sphere, physicsComp.velocity * delta, otherTransformComp, otherPhysicsComp.collisionMesh);
                            if (result.collisionOccured)
                            {
                                transformComp.position += physicsComp.velocity * delta * result.collisionTime;

                                Vector3f normalWithoutY = normalize(Vector3f(result.collisionNormal.x, 0.f, result.collisionNormal.z));
                                physicsComp.velocity = reflect(physicsComp.velocity, normalWithoutY) * speedIncrease;

                                transformComp.position += physicsComp.velocity * delta * (1.f - result.collisionTime);

                                soundSystem->playSound("Bleep");
                                moveFreely = false;
                            }
                        }
                    }
                }

                // Falling of bricks
                for (Vector3f destroyBrickPosition : destroyedBricksPosition)
                {
                    for (Entity& otherEntity : gWorld.entityManager.entities_with_components<TransformComponent, PhysicsComponent>())
                    {
                        TransformComponent& otherTransformComp = otherEntity.getComponent<TransformComponent>();
                        PhysicsComponent& otherPhysicsComp = otherEntity.getComponent<PhysicsComponent>();

                        if (otherEntity.tag == Tag::Brick &&
                            destroyBrickPosition.x == otherTransformComp.position.x &&
                            destroyBrickPosition.z == otherTransformComp.position.z)
                        {
                            otherPhysicsComp.velocity = Vector3f(0.f, -10.0f, 0.f);
                        }
                    }
                }
            }
            // Move with entities
            if (moveFreely)
                transformComp.position += physicsComp.velocity * delta;
        }
    }

    if (brickCount == 0)
    {
        gameState = GameState::Victory;
        soundSystem->playSound("victory");
    }
}

float GameplaySystem::distFromCenter(Vector3f position)
{
    return distance(position, Vector3f::zero());
}

float GameplaySystem::angleFromCenter(Vector3f position)
{
    Vector3f positionNorm = normalize(position);
    return std::atan2(positionNorm.x, positionNorm.z);
}

CollisionResult GameplaySystem::sphereMeshCollisionFast(const Sphere& sphere, TransformComponent& transformComp, Mesh* mesh)
{
    CollisionResult result;
    result.collisionOccured = false;
    Matrix4x4f modelMatrix = Matrix4x4f::translate(transformComp.position) * Matrix4x4f::rotate(transformComp.angle, transformComp.axis);
    Matrix4x4f normalMatrix = transpose(inverse(modelMatrix));

    for (size_t i = 0; i < mesh->indices.size(); ++i)
    {
        Vector3f vertex = modelMatrix * mesh->vertices[mesh->indices[i]];

        float dist = distance(vertex, sphere.origin);
        if ((dist <= sphere.radius && !result.collisionOccured) || (result.collisionOccured && dist <= sphere.radius && dist < result.collisionDistance))
        {
            result.collisionOccured = true;
            result.collisionDistance = dist;
            result.collisionPoint = vertex;
            result.collisionHit = CollisionHit::Point;
            result.collisionTime = 0.f;
            result.collisionNormal = normalMatrix * mesh->normals[mesh->indices[i]];
        }
    }

    return result;
}

CollisionResult GameplaySystem::movingSphereMeshCollision(const Sphere& sphere, const Vector3f& sphereVelocity, TransformComponent& transformComp, Mesh* mesh)
{
    CollisionResult result;
    result.collisionOccured = false;
    Matrix4x4f modelMatrix = Matrix4x4f::translate(transformComp.position) * Matrix4x4f::rotate(transformComp.angle, transformComp.axis);
    Matrix4x4f normalMatrix = transpose(inverse(modelMatrix));

    for (size_t i = 0; i < mesh->indices.size(); i += 3)
    {
        Vector3f p1 = modelMatrix * mesh->vertices[mesh->indices[i]];
        Vector3f p2 = modelMatrix * mesh->vertices[mesh->indices[i + 1]];
        Vector3f p3 = modelMatrix * mesh->vertices[mesh->indices[i + 2]];
        Vector3f n1 = normalMatrix * mesh->normals[mesh->indices[i]];
        Vector3f n2 = normalMatrix * mesh->normals[mesh->indices[i + 1]];
        Vector3f n3 = normalMatrix * mesh->normals[mesh->indices[i + 2]];
        Triangle triangle(p1, p2, p3, n1, n2, n3);
        CollisionResult tmpResult = movingSphereTriangleCollision(sphere, sphereVelocity, triangle);
        if (tmpResult.collisionOccured && (!result.collisionOccured || (tmpResult.collisionTime < result.collisionTime) ||
            (tmpResult.collisionTime == result.collisionTime && tmpResult.collisionDistance < result.collisionDistance)))
        {
            result = tmpResult;
        }
    }

    return result;
}

CollisionResult GameplaySystem::movingSphereTriangleCollision(const Sphere& sphere, const Vector3f& sphereVelocity, const Triangle& triangle)
{
    // Taken from http://www.peroxide.dk/papers/collision/collision.pdf

    // TODO: Convert to proper base so it supports spheres of any size

    CollisionResult result;
    result.collisionOccured = false;

    Plane trianglePlane(triangle);

    float sphereOriginDistance = trianglePlane.signedDistanceTo(sphere.origin);
    float normalDotVelocity = dot(trianglePlane.normal, sphereVelocity);

    float t0;
    float t1;

    bool embeddedInPlane = false;
    // Velocity is parallel to the plane
    if (normalDotVelocity == 0.f)
    {
        if (fabs(sphereOriginDistance) >= 1.0f)
        {
            return result;
        }
        else
        {
            // It intersects in the whole range [0..1]
            embeddedInPlane = true;
            t0 = 0.0;
            t1 = 1.0;        }
    }
    else
    {
        t0 = (1.f - sphereOriginDistance) / normalDotVelocity;
        t1 = (-1.f - sphereOriginDistance) / normalDotVelocity;

        // Swap so t0 < t1
        if (t0 > t1)
        {
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }
        // If both outside [0,1] -> no collision
        if (t0 > 1.0f || t1 < 0.0f)
        {
            return result;
        }

        t0 = clamp(t0);
        t1 = clamp(t1);        result.collisionTime = 1.f;
        if (!embeddedInPlane)
        {
            Vector3f planeIntersectionPoint = sphere.origin - trianglePlane.normal + t0 * sphereVelocity;
            if (checkPointInTriangle(planeIntersectionPoint, triangle))
            {
                result.collisionOccured = true;
                result.collisionPoint = planeIntersectionPoint;
                result.collisionNormal = trianglePlane.normal;
                result.collisionDistance = t0 * sphereVelocity.length();
                result.collisionTime = t0;
                result.collisionHit = CollisionHit::Face;

                return result;
            }
        }

        float velocityLengthSquared = sphereVelocity.lengthSquared();
        float a = velocityLengthSquared;

        // Collision with points
        auto pointCollision = [a, &sphereVelocity, &sphere, &result](const Vector3f& point, const Vector3f& normal)
        {
            float b = 2.f * (dot(sphereVelocity, (sphere.origin - point)));
            float c = (point - sphere.origin).lengthSquared() - 1.f;
            float newT;
            float sphereVelocityLength = sphereVelocity.length();
            // Collision happens, also checks for lowest time if collision with other stuff already happened
            if (getLowestPosRoot(a, b, c, result.collisionTime, &newT) && (!result.collisionOccured || (result.collisionOccured && newT < result.collisionTime) ||
                (result.collisionOccured && result.collisionTime == newT && newT * sphereVelocityLength < result.collisionDistance)))
            {
                result.collisionOccured = true;
                result.collisionPoint = point;
                result.collisionNormal = normal;
                result.collisionTime = newT;
                result.collisionDistance = result.collisionTime * sphereVelocityLength;
                result.collisionHit = CollisionHit::Point;
            }
        };

        pointCollision(triangle.p1, triangle.n1);
        pointCollision(triangle.p2, triangle.n2);
        pointCollision(triangle.p3, triangle.n3);

        // Collision with edges
        auto edgeCollision = [a, &velocityLengthSquared, &sphereVelocity, &sphere, &result](const Vector3f& p1, const Vector3f& p2, const Vector3f& n1, const Vector3f& n2)
        {
            Vector3f edge = p2 - p1;
            Vector3f sphereOriginToVertex = p1 - sphere.origin;
            float edgeLengthSquared = edge.lengthSquared();
            float edgeDotVelocity = dot(edge, sphereVelocity);
            float edgeDotSphereOriginToVertex = dot(edge, sphereOriginToVertex);

            float a = edgeLengthSquared * -velocityLengthSquared + edgeDotVelocity * edgeDotVelocity;
            float b = edgeLengthSquared * (2.f * dot(sphereVelocity, sphereOriginToVertex)) - 2.f * edgeDotVelocity * edgeDotSphereOriginToVertex;
            float c = edgeLengthSquared * (1.f - sphereOriginToVertex.lengthSquared()) + edgeDotSphereOriginToVertex * edgeDotSphereOriginToVertex;
            float newT;
            float sphereVelocityLength = sphereVelocity.length();
            // Collide with infinitely long edge
            if (getLowestPosRoot(a, b, c, result.collisionTime, &newT) && (!result.collisionOccured || (result.collisionOccured && newT < result.collisionTime) ||
                (result.collisionOccured && result.collisionTime == newT && newT * sphereVelocityLength < result.collisionDistance)))
            {
                // Line segment
                float f = (edgeDotVelocity * newT - edgeDotSphereOriginToVertex) / edgeLengthSquared;
                if (f >= 0.f && f <= 1.f)
                {
                    result.collisionOccured = true;
                    result.collisionPoint = p1 + f * edge;
                    result.collisionNormal = (n1 + n2) / 2.f;
                    result.collisionTime = newT;
                    result.collisionDistance = result.collisionTime * sphereVelocityLength;
                    result.collisionHit = CollisionHit::Edge;
                }
            }
        };

        edgeCollision(triangle.p1, triangle.p2, triangle.n1, triangle.n2);
        edgeCollision(triangle.p2, triangle.p3, triangle.n2, triangle.n3);
        edgeCollision(triangle.p3, triangle.p1, triangle.n3, triangle.n1);
    }

    return result;
}